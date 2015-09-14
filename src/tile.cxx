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
    coord_t h = height, w = width;
    borderRects[SIDE_NW] = CoordRect(0,   0,   1, 1);
    borderRects[SIDE_N ] = CoordRect(0,   0,   1, w);
    borderRects[SIDE_NE] = CoordRect(0,   w-1, 1, w);
    borderRects[SIDE_W ] = CoordRect(0,   0,   h, 1);
    borderRects[SIDE_E ] = CoordRect(0,   w-1, h, w);
    borderRects[SIDE_SW] = CoordRect(h-1, 0,   h, 1);
    borderRects[SIDE_S ] = CoordRect(h-1, 0,   h, w);
    borderRects[SIDE_SE] = CoordRect(h-1, w-1, h, 1);
}

AbstractTile::~AbstractTile() {}

size_t AbstractTile::getHeight() const { return height; }
size_t AbstractTile::getWidth() const { return width; }

void AbstractTile::assign(AbstractTile* t)
{
    for (size_t r = 0; r < height; ++r)
        for (size_t c = 0; c < width; ++c)
            set(r, c, t->at(r, c));
}

const CoordRect& AbstractTile::getBorderRect(Side s) const
{ return borderRects[s]; }

Borders AbstractTile::makeBorders()
{
    Borders ret(SIDE_COUNT);
    for (size_t s = 0; s < SIDE_COUNT; ++s)
        ret[s] = makeBorder(static_cast<Side>(s));
    return ret;
}

Border* AbstractTile::makeBorder(Side s)
{ return makeSlice(getBorderRect(s)); }


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
{ return new TileView(this, reg); }


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

Matrix::~Matrix() { delete[] data; }

bool Matrix::at(coord_t r, coord_t c) const { return data[r * getWidth() + c]; }

void Matrix::set(coord_t r, coord_t c, bool v) { data[r * getWidth() + c] = v; }

Matrix* Matrix::makeSlice(const CoordRect& reg)
{
    Matrix* ret = new Matrix(reg.getHeight(), reg.getWidth());

    size_t copyWidth = reg.getWidth();
    size_t copyStride = getWidth();
    bool* beginStart = data + getWidth() * reg.r1 + reg.c1;
    bool* endStart = beginStart + copyStride * reg.getHeight();

    for (bool* p = beginStart, t = ret->data;
         p != endStart;
         p += copyStride, t += copyWidth)
        memcpy((void*)t, (void*)p, copyWidth * sizeof(bool));

    return ret;
}

Matrix m(2, 2);
TileView t(&m);

} // namespace game_of_life

