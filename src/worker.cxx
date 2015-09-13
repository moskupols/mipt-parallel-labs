#include "worker.hxx"

namespace game_of_life
{

namespace
{
int countNeighborsAlive(const AbstractTile* t, coord_t r, coord_t c)
{
    int ret = 0;
    for (size_t i = 0; i < SIDE_COUNT; ++i)
        if (t->at(r + SIDE_DELTAS[i][0], c + SIDE_DELTAS[i][1]))
            ++ret;
    return ret;
}
} // namespace anonymous

void Worker::makeIteration(const AbstractTile* prev, AbstractTile* next)
{
    coord_t h = next->getHeight();
    coord_t w = next->getWidth();
    for (coord_t r = 0; r < h; ++r)
        for (coord_t c = 0; c < w; ++c)
        {
            int neigh = countNeighborsAlive(prev, r, c);
            next->set(r, c, neigh == 3 || neigh == 4);
        }
}


}

