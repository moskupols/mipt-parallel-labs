#include "abstract-tile.hxx"

#include <iostream>
#include <stdexcept>

#include "tile-view.hxx"

namespace game_of_life
{

AbstractTile::AbstractTile():
    borders(NULL)
{ }

AbstractTile::~AbstractTile()
{
    delete[] borders;
}

void AbstractTile::copyValues(const AbstractTile& t)
{
    size_t h = getHeight(), w = getWidth();
    for (size_t r = 0; r < h; ++r)
        for (size_t c = 0; c < w; ++c)
            set(r, c, t.at(r, c));
}

CoordRect AbstractTile::getBorderRect(Side s) const
{
    const coord_t h = getHeight();
    const coord_t w = getWidth();
    // borders are distributed like so:
    // NNNN
    // W  E
    // SSSS
    switch (s)
    {
    case SIDE_N: return CoordRect(0,   0,   1,   w);
    case SIDE_W: return CoordRect(1,   0,   h-1, 1);
    case SIDE_E: return CoordRect(1,   w-1, h-1, w);
    case SIDE_S: return CoordRect(h-1, 0,   h,   w);
    }
    throw std::logic_error("unusual side");
}

CoordRect AbstractTile::getInnerRect() const
{
    return CoordRect(1, 1, (coord_t)getHeight()-1, (coord_t)getWidth()-1);
}

Borders AbstractTile::getBorders()
{
    if (borders)
        return borders;
    borders = new Border[SIDE_COUNT];
    for (size_t i = 0; i < SIDE_COUNT; ++i)
        borders[i] = makeSlice(getBorderRect(static_cast<Side>(i)));
    return borders;
}

Border AbstractTile::getBorder(Side s)
{
    if (!borders)
        getBorders();
    return borders[s];
}

TileView AbstractTile::getInner()
{
    return makeSlice(getInnerRect());
}

TileView AbstractTile::makeSlice(const CoordRect& r)
{
    return TileView(*this, r);
}

void AbstractTile::output(std::ostream& out) const
{
    out << getHeight() << " x " << getWidth() << std::endl;
    for (size_t i = 0; i < getHeight(); ++i)
        for (size_t j = 0; j < getWidth(); ++j)
        {
            out << ".#"[at(i, j)];
            if (j + 1 == getWidth())
                out << '\n';
        }
}

}

namespace std
{
ostream& operator<<(ostream& out, const game_of_life::AbstractTile& t)
{
    t.output(out);
    return out;
}
}

