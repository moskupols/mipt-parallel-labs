#ifndef GAME_HXX_INCLUDED
#define GAME_HXX_INCLUDED

#include "tile.hxx"

namespace game_of_life
{

class GameIteratorHelper
{
public:
    virtual std::pair<AbstractTile*, AbstractTile*> beginIteration() = 0;
    virtual void endIteration() = 0;
};


class GameIterator
{
public:
    explicit GameIterator(GameIteratorHelper* c);

    void workInfinitely();

protected:
    void makeIteration();

private:
    static int countNeighborsAlive(AbstractTile* t, coord_t r, coord_t c);

    GameIteratorHelper* helper;
};

}

#endif

