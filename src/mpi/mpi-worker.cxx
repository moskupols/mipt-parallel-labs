#include "mpi-worker.hxx"

#include "../tiles/matrix.hxx"
#include "../tiles/tile-view.hxx"
#include "msg.hxx"

#include "../output.hxx"

#include <algorithm>
#include <sstream>

using namespace mpi;

namespace
{
}

namespace game_of_life
{

MpiWorker::MpiWorker():
    stopper(0),
    iterCompleted(0),
    broadcastReq(requests[4])
{}

void MpiWorker::shutdown()
{
    comm.barrier();
}

bool MpiWorker::initialize()
{
    int me = comm.getRank();
    assert(me);

    debug("waiting for actual worker count");

    comm.broadcast(&workerCount, 1, 0);
    if (me > (int)workerCount)
    {
        comm.split(MPI_UNDEFINED);
        debug("nothing to do here");
        return false;
    }
    comm = comm.split(0);

    size_t sz[2];
    comm.receive(sz, 2, 0, 0);
    workMatrix = Matrix{sz[0], sz[1]};
    tempWorkMatrix = Matrix{sz[0], sz[1]};

    debug("receiving the initial matrix");
    comm.receive(workMatrix.getData(), sz[0]*sz[1], 0, 0);

    neighLines[0] = neighLines[1] = Matrix{1, sz[1]};
    frame = FrameView {workMatrix, neighLines[0], neighLines[1]};

    borders = frame.getBorders();
    inner = frame.getInner();

    tempBorders = tempWorkMatrix.getBorders();
    tempInner = tempWorkMatrix.getInner();

    neighs[0] = (me + workerCount - 2) % workerCount + 1;
    neighs[1] = me % workerCount + 1;

    iterCompleted = stopper = 0;

    debug("finished initialization, ready to go");
    bordersNotCalced = 0;
    neighsNotReceived = 0;

    return true;
}

void MpiWorker::loop()
{
    int msgBuf[2] = {-1, -1};
    broadcastReq = comm.asyncBroadcast(msgBuf, 2, 0);
    int indices[5];
    int indCount;
    while (true)
    {
        if (bordersNotCalced == 0 && neighsNotReceived == 0 && iterCompleted < stopper)
            startIteration();
        else if (iterCompleted == stopper)
            debug() << "reached stopper " << stopper << " so not calculating";

        if (workerCount == 1 && iterCompleted < stopper)
        {
            indices[0] = 4;
            indCount = broadcastReq.test() ? 1 : 0;
        }
        else
        {
            indCount = MpiRequest::waitSome(5, requests, indices);
            std::sort(indices, indices + indCount);
        }
        for (int i = indCount-1; i >= 0; --i)
        {
            if (indices[i] < 2)
                calcBorder(indices[i]);
            else if (indices[i] == 4)
            {
                MsgType type = static_cast<MsgType>(msgBuf[0]);
                processMessage(type, msgBuf[1]);
                broadcastReq = comm.asyncBroadcast(msgBuf, 2, 0);
                if (type == MsgType::SHUTDOWN)
                    return;
            }
            else
                --neighsNotReceived;
        }
    }
}

void MpiWorker::processMessage(MsgType type, int msgArg)
{
    debug() << "got broadcasted message " << (int)type;
    switch (type)
    {
    case MsgType::UPDATE_STOPPER: stopper = msgArg; break;
    case MsgType::STOP: stop(); break;
    case MsgType::SHUTDOWN: return shutdown();
    case MsgType::UPDATE_STATUS: updateStatus(); break;
    default:
        assert(false);
    }
}

void MpiWorker::startIteration()
{
    assert(iterCompleted < stopper);

    bool* data = workMatrix.getData();
    size_t width = workMatrix.getWidth();
    bool* starts[2] = {data, data + width * (workMatrix.getHeight()-1)};

    if (workerCount != 1)
    {
        for (int i = 0; i < 2; ++i)
        {
            requests[i+2] = comm.asyncSend(starts[i], width, neighs[i], 0);
            requests[i] = comm.asyncReceive(neighLines[i^1].getData(), width, neighs[i], 0);
        }

        makeIteration(inner, tempInner);
        bordersNotCalced = 2;
        neighsNotReceived = 2;
    }
    else
    {
        makeIteration(workMatrix, tempWorkMatrix);
        finishIteration();
    }
}

void MpiWorker::calcBorder(int b)
{
    makeIteration(borders[b], tempBorders[b]);
    if (!--bordersNotCalced)
        finishIteration();
    assert(bordersNotCalced >= 0);
}

void MpiWorker::finishIteration()
{
#ifdef DEBUG_OUTPUT
    std::ostringstream d;
    d << "after iteration " << iterCompleted << ":\n";
    frame.output(d);
    debug(d.str());
#endif

    std::swap(workMatrix, tempWorkMatrix);
    ++iterCompleted;
}

void MpiWorker::stop()
{
    int nextIter = std::min(stopper, iterCompleted);
    comm.allreduce(&nextIter, &stopper, 1, MPI_MAX);
}

void MpiWorker::updateStatus()
{
    int minIter;
    comm.allreduce(&iterCompleted, &minIter, 1, MPI_MIN);
    if (minIter == stopper)
        comm.send(workMatrix.getData(),
                workMatrix.getWidth() * workMatrix.getHeight(),
                0, 0);
}

void MpiWorker::run(MpiCommunicator comm_)
{
    comm = comm_;
    if (!initialize())
        return;
    loop();
}

}

