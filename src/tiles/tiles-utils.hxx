#ifndef TILES_UTILS_HXX_INCLUDED
#define TILES_UTILS_HXX_INCLUDED

#include <cstddef>

namespace game_of_life
{

typedef long ssize_t;
typedef ssize_t coord_t;
using std::size_t;


struct CoordRect
{
    CoordRect();
    CoordRect(coord_t r1, coord_t c1, coord_t r2, coord_t c2);

    coord_t r1, c1, r2, c2;

    size_t getHeight() const;
    size_t getWidth() const;

    bool contains(coord_t r, coord_t c) const;

    CoordRect shifted(coord_t dr, coord_t dc) const;
};


class TileView;

typedef TileView Border;
typedef Border* Borders;

enum Side
{
    SIDE_N = 0,
    SIDE_E = 1,
    SIDE_S = 2,
    SIDE_W = 3
};
static const size_t SIDE_COUNT = SIDE_W + 1;

static const size_t DIRECTION_COUNT = 8;
static const int DIRECTION[DIRECTION_COUNT][2] =
{
    {-1, -1}, {-1, 0}, {-1, 1},
    {0, -1},           {0, 1},
    {1, -1},  {1, 0},  {1, 1}
};

}
#endif
