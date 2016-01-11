#include "mpi-manager.hxx"

#include "../tiles/abstract-tile.hxx"
#include "../tiles/matrix.hxx"
#include "../tiles/tile-view.hxx"

#include <vector>

using namespace std;
using namespace mpi;

namespace game_of_life
{

MpiManager::MpiManager()
{}

void MpiManager::start(
        AbstractTile& tile, MpiCommunicator comm, unsigned workerCount)
{
    assert(getState() != NOT_STARTED);
    assert(comm.getRank() == 0);
    assert(workerCount + 1 <= (unsigned)comm.getSize());

    this->cleanTile = &tile;
    this->globalComm = comm;
    this->workerCount = workerCount;

    Manager::start();
    wakeWhenStateIs(STOPPED);
}

void MpiManager::runForMore(int /*runs*/)
{
}

void MpiManager::pauseAll()
{
}

void MpiManager::shutdown()
{
}

int MpiManager::getStop() const
{
    return -1;
}

void MpiManager::run()
{
    Matrix workMatrix(*cleanTile);

    vector<CoordRect> domains{chooseDomains(workMatrix, workerCount)};
    for (workerCount = 0; workerCount < domains.size(); ++workerCount)
        if (domains[workerCount].isEmpty())
            break;
    domains.erase(domains.begin() + workerCount, domains.end());

    globalComm.broadcast(&workerCount, 1, 0);
    MpiCommunicator comm = globalComm.split(0);

    vector<bool*> domainBoundaries;
    for (auto rect : domains)
        domainBoundaries.push_back(
                workMatrix.getData() + rect.r1 * workMatrix.getWidth());
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
    setState(STOPPED);
}

};

