#include "mpi-worker.hxx"

#include "../tiles/matrix.hxx"
#include "../tiles/tile-view.hxx"

using namespace mpi;

namespace
{
}

namespace game_of_life
{

void MpiWorker::run(MpiCommunicator comm_)
{
    comm = comm_;

    int me = comm.getRank();
    assert(me);

    comm.broadcast(&workerCount, 1, 0);
    if (me > (int)workerCount)
    {
        comm.split(MPI_UNDEFINED);
        return;
    }
    comm = comm.split(0);

    size_t sz[2];
    comm.receive(sz, 2, 0, 0);
    Matrix workMatrix{sz[0], sz[1]};
    comm.receive(workMatrix.getData(), sz[0]*sz[1], 0, 0);

    Matrix topLine{1, sz[1]};
    Matrix bottomLine{1, sz[1]};
    FrameView frame(workMatrix, topLine, bottomLine);
    TileView workWindow = frame.getInner();

    int topNeigh = (me + workerCount - 2) % workerCount + 1;
    int bottomNeigh = me % workerCount + 1;
}

}

