#ifndef MPI_MANAGER_HXX_INCLUDED
#define MPI_MANAGER_HXX_INCLUDED

#include "mpi.hxx"
#include "../manager.hxx"
#include "../tiles/matrix.hxx"

#include <vector>

namespace game_of_life
{

class AbstractTile;

class MpiManager : public Manager
{
public:
    MpiManager();

    void start(AbstractTile& tile, mpi::MpiCommunicator comm, unsigned workerCount);
    void runForMore(int runs);
    void pauseAll();
    void shutdown();
    void updateStatus();

    int getStop();

private:
    AbstractTile* cleanTile;
    Matrix workMatrix;
    mpi::MpiCommunicator globalComm, comm;
    unsigned workerCount;
    std::vector<bool*> domainBoundaries;

    int stop;
};

}

#endif

