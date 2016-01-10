#ifndef MPI_MANAGER_HXX_INCLUDED
#define MPI_MANAGER_HXX_INCLUDED

#include "mpi.hxx"
#include "../manager.hxx"

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

    int getStop() const;

protected:
    void run();

private:
    AbstractTile* cleanTile;
    mpi::MpiCommunicator globalComm;
    unsigned workerCount;
};

}

#endif

