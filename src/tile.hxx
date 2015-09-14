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


class TileView;

typedef TileView* Border;
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


class AbstractTile
{
public:
    AbstractTile(size_t height, size_t width);
    virtual ~AbstractTile();

    size_t getHeight() const;
    size_t getWidth() const;

    virtual bool at(coord_t r, coord_t c) const = 0;
    virtual void set(coord_t r, coord_t c, bool v) = 0;

    virtual void copyValues(const AbstractTile& t);

    CoordRect getBorderRect(Side s) const;
    CoordRect getInnerRect() const;

    Borders getBorders();
    Border getBorder(Side s);
    TileView* getInner();

    virtual TileView* makeSlice(const CoordRect& r);

private:
    const size_t height;
    const size_t width;

    Border borders[SIDE_COUNT];
    TileView* inner;
};


class TileView : public AbstractTile
{
public:
    TileView();
    explicit TileView(AbstractTile* viewed);
    explicit TileView(TileView* viewed);
    TileView(AbstractTile* viewed, const CoordRect& r);

    TileView& operator = (const TileView& that);

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    TileView* makeSlice(const CoordRect& reg);

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
    explicit Matrix(const AbstractTile& t);
    explicit Matrix(const Matrix& m);
    ~Matrix();

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    void copyValues(const Matrix& m);

private:
    bool* data;
};


} // namespace game_of_life

#endif

