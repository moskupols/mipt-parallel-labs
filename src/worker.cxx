#include "gol.hxx"

#include <cassert>

namespace game_of_life
{
Worker::Worker(WorkerHelper* h):
    helper(h)
{}

void Worker::workInfinitely()
{
    while (true)
    {
        makeIteration();
    }
}

void Worker::makeIteration()
{
    std::pair<AbstractTile*, AbstractTile*> tiles = helper->beginIteration();
    AbstractTile* extendedTile = tiles.first;
    AbstractTile* tempTile = tiles.second;

    for (coord_t r = 0; r < (coord_t)tempTile->getHeight(); ++r)
        for (coord_t c = 0; c < (coord_t)tempTile->getWidth(); ++c)
        {
            int neigh = countNeighborsAlive(extendedTile, r, c);
            tempTile->set(r, c, neigh == 3 || neigh == 4);
        }

    helper->endIteration();
}

int Worker::countNeighborsAlive(AbstractTile* t, coord_t r, coord_t c)
{
    int ret = 0;
    for (size_t i = 0; i < SIDE_COUNT; ++i)
        if (t->at(r + SIDE_DELTAS[i][0], c + SIDE_DELTAS[i][1]))
            ++ret;
    return ret;
}

}

