#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED

#include <vector>
#include <cstddef>

#include "utils.hxx"

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
};


class AbstractTile;

typedef AbstractTile Border;
typedef std::vector<Border*> Borders;


enum Side
{
    SIDE_NW = 0,
    SIDE_N = 1,
    SIDE_NE = 2,
    SIDE_E = 3,
    SIDE_SE = 4,
    SIDE_S = 5,
    SIDE_SW = 6,
    SIDE_W = 7
};
const size_t SIDE_COUNT = SIDE_SE + 1;

static Side oppositeSide(Side s);


class AbstractTile
{
public:
    AbstractTile(size_t height, size_t width);
    virtual ~AbstractTile();

    size_t getHeight() const;
    size_t getWidth() const;

    virtual bool at(coord_t r, coord_t c) const = 0;
    virtual void set(coord_t r, coord_t c, bool v) = 0;

    const CoordRect& getBorderRect(Side s) const;

    Borders makeBorders() const;
    Border* makeBorder(Side s) const;

    virtual AbstractTile* makeSlice(const CoordRect& r) const = 0;

private:
    const size_t height;
    const size_t width;

    /*const*/ CoordRect borderRects[SIDE_COUNT];
};


class TileView : public AbstractTile
{
public:
    explicit TileView(AbstractTile* viewed, CoordRect r = CoordRect(0, 0, 0, 0));

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    TileView* makeSlice(const CoordRect& r) const;

private:
    AbstractTile* viewed;
    coord_t off_r;
    coord_t off_c;
};


class Matrix : public AbstractTile, Noncopyable
{
public:
    Matrix(size_t height, size_t width);
    ~Matrix();

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    Matrix* makeSlice(const CoordRect& r) const;

private:
    bool* data;
};

} // namespace game_of_life

#endif

