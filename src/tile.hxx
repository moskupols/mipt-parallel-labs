#ifndef TILE_HXX_INCLUDED
#define TILE_HXX_INCLUDED

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

    bool contains(coord_t r, coord_t c) const;

    CoordRect shifted(coord_t dr, coord_t dc) const;
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
static const size_t SIDE_COUNT = SIDE_W + 1;

static const int SIDE_DELTAS[SIDE_COUNT][2] =
{
    {-1, -1}, {-1, 0}, {-1, 1},
    {0, -1},           {0, 1},
    {1, -1},  {1, 0},  {1, 1}
};

Side oppositeSide(Side s);


class AbstractTile
{
public:
    AbstractTile(size_t height, size_t width);
    virtual ~AbstractTile();

    size_t getHeight() const;
    size_t getWidth() const;

    virtual bool at(coord_t r, coord_t c) const = 0;
    virtual void set(coord_t r, coord_t c, bool v) = 0;

    void assign(AbstractTile* t);

    const CoordRect& getBorderRect(Side s) const;

    // TODO: solve problem with const-ness:
    // immutable slices or smth
    Borders makeBorders();
    Border* makeBorder(Side s);

    virtual AbstractTile* makeSlice(const CoordRect& r) = 0;

private:
    const size_t height;
    const size_t width;

    /*const*/ CoordRect borderRects[SIDE_COUNT];
};


class TileView : public AbstractTile
{
public:
    explicit TileView(AbstractTile* viewed);
    explicit TileView(TileView* viewed);
    TileView(AbstractTile* viewed, const CoordRect& r);

    TileView& operator = (const TileView& that);

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    TileView* makeSlice(const CoordRect& r);

private:
    AbstractTile* viewed;
    coord_t off_r;
    coord_t off_c;
};


class TorusView : public TileView
{
public:
    explicit TorusView(AbstractTile* viewed);

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    coord_t normalizeRow(coord_t r) const;
    coord_t normalizeColumn(coord_t c) const;
protected:
    static coord_t normalizeCoord(coord_t c, coord_t dimen);
};


class Matrix : public AbstractTile, Noncopyable
{
public:
    Matrix(size_t height, size_t width);
    ~Matrix();

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    Matrix* makeSlice(const CoordRect& r);

private:
    bool* data;
};

/*

class FramingTileView : public AbstractTile
{
public:
    FramingTileView(
            AbstractTile* innerTile,
            Borders frame);

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);
    AbstractTile* makeSlice(const CoordRect& r) const;

protected:
    int determineSide(coord_t r, coord_t c) const;
    coord_t normalizeRow(coord_t r) const { return normalizeCoord(r, getHeight()); }
    coord_t normalizeColumn(coord_t c) const { return normalizeCoord(c, getWidth()); }

private:
    coord_t normalizeCoord(coord_t x, coord_t dimen) const
    { return x >= 0 && x < dimen ? x : 0; }

    AbstractTile* innerTile;
    Borders frame;
};

FramingTileView::FramingTileView(
        AbstractTile* innerTile,
        Borders frame):
    AbstractTile(
        innerTile->getHeight(),
        innerTile->getWidth()),
    innerTile(innerTile),
    frame(frame)
{}

bool FramingTileView::at(coord_t r, coord_t c) const
{
    int side = determineSide(r, c);
    AbstractTile* t = (side == -1 ? innerTile : frame[side]);
    return t->at(normalizeRow(r), normalizeColumn(c));
}

void FramingTileView::set(coord_t r, coord_t c, bool v)
{
    int side = determineSide(r, c);
    AbstractTile* t = (side == -1 ? innerTile : frame[side]);
    return t->set(normalizeRow(r), normalizeColumn(c), v);
}

AbstractTile* FramingTileView::makeSlice(const CoordRect& r) const
{
    assert(r.r1 >= 0 && r.c1 >= 0
            && r.r2 < (coord_t)getHeight() && r.c2 < (coord_t)getWidth());
    return innerTile->makeSlice(r);
}

int FramingTileView::determineSide(coord_t r, coord_t c) const
{
    static int sideMap[3][3] =
    {
        {SIDE_NW, SIDE_N, SIDE_NE},
        {SIDE_W,  -1,     SIDE_W},
        {SIDE_SW, SIDE_S, SIDE_SE}
    };

    r = (r == coord_t(getHeight()) ? 2 : (r >= 0 ? 1 : 0));
    c = (c == coord_t(getWidth()) ? 2 : (c >= 0 ? 1 : 0));

    return sideMap[r][c];
}

*/

} // namespace game_of_life

#endif

