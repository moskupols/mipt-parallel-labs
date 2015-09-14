#include "tile.hxx"

#include <cstring>

namespace game_of_life
{

CoordRect::CoordRect() {}

CoordRect::CoordRect(coord_t r1, coord_t c1, coord_t r2, coord_t c2):
    r1(r1), c1(c1), r2(r2), c2(c2)
{}

size_t CoordRect::getHeight() const { return r2 - r1; }
size_t CoordRect::getWidth() const { return c2 - c1; }

bool CoordRect::contains(coord_t r, coord_t c) const
{ return (r >= r1 && r < r2) && (c >= c1 && c < c2); }

CoordRect CoordRect::shifted(coord_t dr, coord_t dc) const
{ return CoordRect(r1 + dr, c1 + dc, r2 + dr, c2 + dc); }

AbstractTile::AbstractTile(size_t height, size_t width):
    height(height),
    width(width)
{
    for (size_t i = 0; i < SIDE_COUNT; ++i)
        borders[i] = NULL;
    inner = NULL;
}

AbstractTile::~AbstractTile()
{
    for (size_t i = 0; i < SIDE_COUNT; ++i)
        delete borders[i];
    delete inner;
}

size_t AbstractTile::getHeight() const { return height; }
size_t AbstractTile::getWidth() const { return width; }

void AbstractTile::copyValues(const AbstractTile& t)
{
    for (size_t r = 0; r < height; ++r)
        for (size_t c = 0; c < width; ++c)
            set(r, c, t.at(r, c));
}

CoordRect AbstractTile::getBorderRect(Side s) const
{
    const coord_t h = height;
    const coord_t w = width;
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
}

CoordRect AbstractTile::getInnerRect() const
{
    return CoordRect(1, 1, (coord_t)height-1, (coord_t)width-1);
}

Borders AbstractTile::getBorders()
{
    for (size_t i = 0; i < SIDE_COUNT; ++i)
        getBorder(static_cast<Side>(i));
    return borders;
}

Border AbstractTile::getBorder(Side s)
{
    return borders[s] ? borders[s] : borders[s] = makeSlice(getBorderRect(s));
}

TileView* AbstractTile::getInner()
{
    return inner ? inner : inner = makeSlice(getInnerRect());
}

TileView* AbstractTile::makeSlice(const CoordRect& r)
{
    return new TileView(this, r);
}


TileView::TileView():
    AbstractTile(0, 0)
{}

TileView::TileView(AbstractTile* viewed):
    AbstractTile(viewed->getHeight(), viewed->getWidth()),
    viewed(viewed),
    off_r(0),
    off_c(0)
{}

TileView::TileView(TileView* that):
    AbstractTile(*that),
    viewed(that->viewed),
    off_r(that->off_r),
    off_c(that->off_c)
{}

TileView::TileView(AbstractTile* viewed, const CoordRect& r):
    AbstractTile(r.getHeight(), r.getWidth()),
    viewed(viewed),
    off_r(r.r1),
    off_c(r.c1)
{}

TileView& TileView::operator=(const TileView& that)
{
    viewed = that.viewed;
    off_r = that.off_r;
    off_c = that.off_c;
    return *this;
}

bool TileView::at(coord_t r, coord_t c) const
{ return viewed->at(off_r + r, off_c + c); }

void TileView::set(coord_t r, coord_t c, bool v)
{ return viewed->set(off_r + r, off_c + c, v); }

TileView* TileView::makeSlice(const CoordRect& reg)
{
    return new TileView(viewed, reg.shifted(off_r, off_c));
}


TorusView::TorusView(AbstractTile* viewed):
    TileView(viewed)
{}

bool TorusView::at(coord_t r, coord_t c) const
{
    return TileView::at(
            normalizeCoord(r, getHeight()),
            normalizeCoord(c, getWidth()));
}

void TorusView::set(coord_t r, coord_t c, bool v)
{ return TileView::set(normalizeRow(r), normalizeColumn(c), v); }

coord_t TorusView::normalizeRow(coord_t r) const
{ return normalizeCoord(r, getHeight()); }

coord_t TorusView::normalizeColumn(coord_t c) const
{ return normalizeCoord(c, getWidth()); }

coord_t TorusView::normalizeCoord(coord_t c, coord_t dimen)
{ return ((c % dimen) + dimen) % dimen; }


Matrix::Matrix(size_t height, size_t width):
    AbstractTile(height, width),
    data(new bool(height * width))
{}

Matrix::Matrix(const AbstractTile& t):
    AbstractTile(t.getHeight(), t.getWidth()),
    data(new bool(t.getHeight() * t.getWidth()))
{
    AbstractTile::copyValues(t);
}

Matrix::~Matrix() { delete[] data; }

bool Matrix::at(coord_t r, coord_t c) const { return data[r * getWidth() + c]; }

void Matrix::set(coord_t r, coord_t c, bool v) { data[r * getWidth() + c] = v; }

void Matrix::copyValues(const Matrix& m)
{
    assert(getWidth() == m.getWidth());
    assert(getHeight() == m.getHeight());

    memcpy(data, m.data, getWidth() * getHeight() * sizeof(data[0]));
}

Matrix m(2, 2);
TileView t(&m);

} // namespace game_of_life

