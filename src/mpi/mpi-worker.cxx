#include "mpi-worker.hxx"

#include "../tiles/matrix.hxx"
#include "../tiles/tile-view.hxx"
#include "msg.hxx"

#include "../output.hxx"

using namespace mpi;

namespace
{
}

namespace game_of_life
{

MpiWorker::MpiWorker():
    stopper(0),
    iterCompleted(0)
{}

void MpiWorker::makeIteration()
{
    assert(iterCompleted < stopper);
    MpiRequest topReq =
        comm.asyncSend(workMatrix.getData(), workMatrix.getWidth(), topNeigh, 0);
    MpiRequest bottomReq =
        comm.asyncSend(
                workMatrix.getData() + workMatrix.getWidth() * (workMatrix.getHeight()-1),
                workMatrix.getWidth(), bottomNeigh, 0);

    comm.receive(topLine.getData(), topLine.getWidth(), topNeigh, 0);
    comm.receive(bottomLine.getData(), bottomLine.getWidth(), bottomNeigh, 0);

    Worker::makeIteration(workWindow, tempWorkMatrix);
    std::swap(workMatrix, tempWorkMatrix);

    topReq.wait();
    bottomReq.wait();

    ++iterCompleted;
}

void MpiWorker::stop()
{
    comm.allreduce(&iterCompleted, &stopper, 1, MPI_MAX);
}

void MpiWorker::updateStatus()
{
    int minIter;
    comm.allreduce(&iterCompleted, &minIter, 1, MPI_MIN);
    // TODO gather
}

void MpiWorker::shutdown()
{
    comm.barrier();
}

void MpiWorker::run(MpiCommunicator comm_)
{
    comm = comm_;

    int me = comm.getRank();
    assert(me);

    debug("waiting for actual worker count");

    comm.broadcast(&workerCount, 1, 0);
    if (me > (int)workerCount)
    {
        comm.split(MPI_UNDEFINED);
        return;
    }
    comm = comm.split(0);

    size_t sz[2];
    comm.receive(sz, 2, 0, 0);
    workMatrix = Matrix{sz[0], sz[1]};
    tempWorkMatrix = workMatrix;

    debug("receiving the initial matrix");
    comm.receive(workMatrix.getData(), sz[0]*sz[1], 0, 0);

    topLine = Matrix{1, sz[1]};
    bottomLine = Matrix{1, sz[1]};
    frame = FrameView{workMatrix, topLine, bottomLine};
    workWindow = frame.getInner();

    topNeigh = (me + workerCount - 2) % workerCount + 1;
    bottomNeigh = me % workerCount + 1;

    iterCompleted = stopper = 0;

    debug("finished initialization, ready to go");

    int msgBuf[2] = {-1, -1};
    MpiRequest req = comm.asyncBroadcast(msgBuf, 2, 0);
    while (true)
    {
        bool gotMessage;
        if (iterCompleted >= stopper)
        {
            debug() << "reached stopper " << stopper << ", waiting for broadcast";
            req.wait();
            gotMessage = true;
        }
        else
            gotMessage = req.test();

        if (!gotMessage)
        {
            makeIteration();
            continue;
        }
        debug() << "got broadcasted message " << msgBuf[0];

        MsgType type = static_cast<MsgType>(msgBuf[0]);
        int msgArg = msgBuf[1];

        switch (type)
        {
        case MsgType::UPDATE_STOPPER: stopper += msgArg; break;
        case MsgType::STOP: stop(); break;
        case MsgType::SHUTDOWN: return shutdown();
        case MsgType::UPDATE_STATUS: updateStatus(); break;
        default:
            assert(false);
        }

        req = comm.asyncBroadcast(msgBuf, 2, 0);
    }
}

}

