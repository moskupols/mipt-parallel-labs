#ifndef ABSTRACT_TILE_HXX_INCLUDED
#define ABSTRACT_TILE_HXX_INCLUDED

#include "tiles-utils.hxx"

#include <iostream>

namespace game_of_life
{

class AbstractTile
{
public:
    AbstractTile();
    virtual ~AbstractTile();

    virtual size_t getHeight() const = 0;
    virtual size_t getWidth() const = 0;

    virtual bool at(coord_t r, coord_t c) const = 0;
    virtual void set(coord_t r, coord_t c, bool v) = 0;

    virtual void copyValues(const AbstractTile& t);

    CoordRect getBorderRect(Side s) const;
    CoordRect getInnerRect() const;

    Border* getBorders();
    Border getBorder(Side s);
    TileView getInner();

    virtual TileView makeSlice(const CoordRect& r);

    virtual void output(std::ostream& out) const;
private:
    Border* borders;
};

}

namespace std
{
ostream& operator<<(ostream& out, const game_of_life::AbstractTile& t);
}

#endif

