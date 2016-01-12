#ifndef MPI_WORKER_HXX
#define MPI_WORKER_HXX

#include "mpi.hxx"
#include "../worker.hxx"
#include "../tiles/matrix.hxx"
#include "../tiles/tile-view.hxx"
#include "msg.hxx"

namespace game_of_life
{

class MpiWorker : public Worker
{
public:
    MpiWorker();

    void run(mpi::MpiCommunicator comm);

protected:
    bool initialize();
    void loop();

    void processMessage(MsgType type, int arg);

    void startIteration();
    void calcBorder(int);
    void finishIteration();

    void stop();
    void updateStatus();
    void shutdown();

private:
    mpi::MpiCommunicator comm;
    unsigned workerCount;

    int neighs[2];
    Matrix neighLines[2];

    Matrix workMatrix, tempWorkMatrix;
    FrameView frame;

    Borders borders;
    TileView inner;

    Borders tempBorders;
    TileView tempInner;

    mpi::MpiRequest requests[5];

    int stopper, iterCompleted;
    int bordersNotCalced;
    int neighsNotReceived;

    mpi::MpiRequest &broadcastReq;
};

}

#endif

