#include "mpi-manager.hxx"

#include "../tiles/abstract-tile.hxx"
#include "../tiles/matrix.hxx"
#include "../tiles/tile-view.hxx"

#include "../output.hxx"

#include "msg.hxx"

#include <vector>

using namespace std;
using namespace mpi;

namespace game_of_life
{

MpiManager::MpiManager():
    stop(0)
{}

void MpiManager::start(
        AbstractTile& cleanTile_, MpiCommunicator comm_, unsigned workerCount)
{
    this->cleanTile = &cleanTile_;
    comm = globalComm = comm_;
    this->workerCount = workerCount;

    debug("starting manager...");

    assert(getState() == NOT_STARTED);
    assert(comm.getRank() == 0);
    assert(workerCount + 1 <= (unsigned)comm.getSize());

    workMatrix = Matrix(*cleanTile);

    vector<CoordRect> domains(chooseDomains(workMatrix, workerCount));
    for (workerCount = 0; workerCount < domains.size(); ++workerCount)
        if (domains[workerCount].isEmpty())
            break;
    domains.erase(domains.begin() + workerCount, domains.end());

    debug() << "domains ready, the actual worker count is " << workerCount;

    globalComm.broadcast(&workerCount, 1, 0);
    comm = globalComm.split(0);

    debug("worker count broadcasted");

    for (unsigned i = 0; i < workerCount; ++i)
        domainBoundaries.push_back(
                workMatrix.getData() + domains[i].r1 * workMatrix.getWidth());
    domainBoundaries.push_back(
            workMatrix.getData() + workMatrix.getWidth() * workMatrix.getHeight());

    for (unsigned i = 0; i < workerCount; ++i)
    {
        size_t sz[2] = {domains[i].getHeight(), domains[i].getWidth()};
        comm.send(sz, 2, i+1, 0);
        comm.send(
                domainBoundaries[i], domainBoundaries[i+1]-domainBoundaries[i],
                i+1, 0);
    }
    debug("domains are sent");

    setState(STOPPED);
}

void MpiManager::runForMore(int runs)
{
    stop += runs;
    int msg[2] = {static_cast<int>(MsgType::UPDATE_STOPPER), stop};
    debug("broadcasting update stopper");
    comm.asyncBroadcast(msg, 2, 0).wait();
}

void MpiManager::pauseAll()
{
    int msg[2] = {static_cast<int>(MsgType::STOP), 0};
    setState(STOPPING);
    debug("broadcasting stop");
    comm.asyncBroadcast(msg, 2, 0).wait();
    int maxIter = 0;
    comm.allreduce(MPI_IN_PLACE, &maxIter, 1, MPI_MAX);
    assert(maxIter <= stop);
    stop = maxIter;
}

void MpiManager::shutdown()
{
    int msg[2] = {static_cast<int>(MsgType::SHUTDOWN), 0};
    setState(STOPPING);
    debug("broadcasting shutdown");
    comm.asyncBroadcast(msg, 2, 0).wait();
    comm.barrier();
    setState(FINISHED);
}

void MpiManager::updateStatus()
{
    if (getState() == NOT_STARTED)
        return;

    int msg[2] = {static_cast<int>(MsgType::UPDATE_STATUS), stop};
    debug("broadcasting update");
    comm.asyncBroadcast(msg, 2, 0).wait();
    int minIter = stop;
    comm.allreduce(MPI_IN_PLACE, &minIter, 1, MPI_MIN);
    if (minIter == stop)
    {
        setState(STOPPED);
        for (unsigned i = 0; i < workerCount; ++i)
        {
            bool* cur = domainBoundaries[i];
            bool* next = domainBoundaries[i+1];
            comm.receive(cur, next-cur, i+1, 0);
        }
        cleanTile->copyValues(workMatrix);
    }
    else
        setState(RUNNING);
}

int MpiManager::getStop()
{
    return stop;
}

}


