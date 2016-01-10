#include "mpi-manager.hxx"

#include "../tiles/abstract-tile.hxx"
#include "../tiles/matrix.hxx"
#include "../tiles/tile-view.hxx"

#include <vector>

using namespace std;

namespace game_of_life
{

MpiManager::MpiManager()
{}

void MpiManager::start(
        AbstractTile& tile, mpi::MpiCommunicator comm, unsigned workerCount)
{
    assert(getState() != NOT_STARTED);
    assert(comm.getRank() == 0);
    assert(workerCount + 1 <= (unsigned)comm.getSize());

    this->cleanTile = &tile;
    this->globalComm = comm;
    this->workerCount = workerCount;

    Manager::start();
}

void MpiManager::runForMore(int runs)
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

    vector<bool*> domainBoundaries;
    for (auto rect : domains)
        domainBoundaries.push_back(
                workMatrix.getData() + rect.r1 * workMatrix.getWidth());
    domainBoundaries.push_back(
            workMatrix.getData() + workMatrix.getWidth() * workMatrix.getHeight());

    for (unsigned i = 0; i < workerCount; ++i)
    {
        int neigh[2];
        neigh[0] = (i + workerCount) % (workerCount + 1);
        neigh[1] = i % workerCount + 1;
        globalComm.send(neigh, 2, i+1, 0);
    }
    for (unsigned i = 0; i < workerCount; ++i)
        globalComm.send(
                domainBoundaries[i], domainBoundaries[i+1]-domainBoundaries[i],
                i+1, 0);
    setState(STOPPED);
}

};

