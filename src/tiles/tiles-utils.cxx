#include "tiles-utils.hxx"

#include <algorithm>
#include <string>
#include <iostream>

using namespace std;

namespace game_of_life
{

CoordRect::CoordRect() {}

CoordRect::CoordRect(coord_t r1, coord_t c1, coord_t r2, coord_t c2):
    r1(r1), c1(c1), r2(r2), c2(c2)
{}

size_t CoordRect::getHeight() const { return max(coord_t(0), r2 - r1); }
size_t CoordRect::getWidth() const { return max(coord_t(0), c2 - c1); }
bool CoordRect::isEmpty() const
{
    return !getWidth() || !getHeight();
}

bool CoordRect::contains(coord_t r, coord_t c) const
{ return (r >= r1 && r < r2) && (c >= c1 && c < c2); }

CoordRect CoordRect::shifted(coord_t dr, coord_t dc) const
{ return CoordRect(r1 + dr, c1 + dc, r2 + dr, c2 + dc); }

}

namespace std
{
ostream& operator<<(ostream& out, const game_of_life::CoordRect& r)
{
    return out << "{" << r.r1 << ", " << r.c1 << "; " << r.r2 << ", " << r.c2 << "}";
}
}

