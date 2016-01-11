#ifndef MPI_WORKER_HXX
#define MPI_WORKER_HXX

#include "mpi.hxx"
#include "../worker.hxx"

namespace game_of_life
{

class MpiWorker : public Worker
{
public:
    void run(mpi::MpiCommunicator comm);

private:
    mpi::MpiCommunicator comm;
    unsigned workerCount;
};

}

#endif

