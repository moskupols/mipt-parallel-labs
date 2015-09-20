#include "tiles-utils.hxx"

namespace game_of_life
{

CoordRect::CoordRect() {}

CoordRect::CoordRect(coord_t r1, coord_t c1, coord_t r2, coord_t c2):
    r1(r1), c1(c1), r2(r2), c2(c2)
{}

size_t CoordRect::getHeight() const { return r2 - r1; }
size_t CoordRect::getWidth() const { return c2 - c1; }
bool CoordRect::isEmpty() const
{
    return getWidth() <= 0 || getHeight() <= 0;
}

bool CoordRect::contains(coord_t r, coord_t c) const
{ return (r >= r1 && r < r2) && (c >= c1 && c < c2); }

CoordRect CoordRect::shifted(coord_t dr, coord_t dc) const
{ return CoordRect(r1 + dr, c1 + dc, r2 + dr, c2 + dc); }

}
