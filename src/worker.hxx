#ifndef GOL_H_INCLUDED
#define GOL_H_INCLUDED

#include "tile.hxx"

namespace game_of_life
{

class WorkerHelper
{
public:
    virtual std::pair<AbstractTile*, AbstractTile*> beginIteration() = 0;
    virtual void endIteration() = 0;
};


class Worker
{
public:
    explicit Worker(WorkerHelper* c);

    void workInfinitely();

protected:
    void makeIteration();

private:
    static int countNeighborsAlive(AbstractTile* t, coord_t r, coord_t c);

    WorkerHelper* helper;
};

}

#endif

