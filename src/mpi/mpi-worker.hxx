#ifndef MPI_WORKER_HXX
#define MPI_WORKER_HXX

#include "mpi.hxx"
#include "../worker.hxx"
#include "../tiles/matrix.hxx"
#include "../tiles/tile-view.hxx"

namespace game_of_life
{

class MpiWorker : public Worker
{
public:
    MpiWorker();

    void run(mpi::MpiCommunicator comm);

protected:
    void makeIteration();
    void stop();
    void updateStatus();

private:
    mpi::MpiCommunicator comm;
    unsigned workerCount;

    Matrix workMatrix, tempWorkMatrix;
    Matrix topLine, bottomLine;
    FrameView frame;
    TileView workWindow;

    int topNeigh, bottomNeigh;

    int stopper, iterCompleted;
};

}

#endif

