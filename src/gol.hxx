#ifndef GOL_H_INCLUDED
#define GOL_H_INCLUDED

#include "tile.hxx"

namespace game_of_life
{

class WorkerCommunicator
{
public:
    virtual void beginIteration() = 0;

    virtual void sendBorders(AbstractTile* t) = 0;
    virtual Borders receiveBorders() = 0;
    virtual void waitForBordersRead() = 0;

    virtual void endIteration() = 0;
};


class Worker
{
public:
    Worker(WorkerCommunicator* c, AbstractTile* t);

    void workInfinitely();

protected:
    void makeIteration();

private:
    WorkerCommunicator* communicator;
    AbstractTile* tile;
};

}

#endif

