#ifndef WORKER_HXX_INCLUDED
#define WORKER_HXX_INCLUDED

#include "tiles/abstract-tile.hxx"

namespace game_of_life
{

class Worker
{
public:
    static void makeIteration(const AbstractTile& prev, AbstractTile& next);
};

}

#endif

