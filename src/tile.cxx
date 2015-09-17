#include "tile.hxx"

#include <cstring>
#include <iostream>

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

AbstractTile::AbstractTile()
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
}

CoordRect AbstractTile::getInnerRect() const
{
    return CoordRect(1, 1, (coord_t)getHeight()-1, (coord_t)getWidth()-1);
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

void AbstractTile::output(std::ostream& out) const
{
    out << getHeight() << " x " << getWidth() << std::endl;
    for (size_t i = 0; i < getHeight(); ++i)
        for (size_t j = 0; j < getWidth(); ++j)
            out << (at(i, j) ? 1 : 0) << " \n"[j+1 == getWidth()];
}


TileView::TileView()
{}

TileView::TileView(AbstractTile* viewed):
    viewed(viewed),
    window(0, 0, -10, -10)
{}

TileView::TileView(TileView* that):
    viewed(that->viewed),
    window(that->window)
{}

TileView::TileView(AbstractTile* viewed, const CoordRect& r):
    viewed(viewed),
    window(r)
{}

TileView& TileView::operator=(const TileView& that)
{
    viewed = that.viewed;
    window = that.window;
    return *this;
}

size_t TileView::getWidth() const
{
    return window.r2 != -10 ? window.getWidth() : viewed->getWidth();
}

size_t TileView::getHeight() const
{
    return window.r2 != -10 ? window.getHeight() : viewed->getHeight();
}

bool TileView::at(coord_t r, coord_t c) const
{ return viewed->at(window.r1 + r, window.c1 + c); }

void TileView::set(coord_t r, coord_t c, bool v)
{ return viewed->set(window.r1 + r, window.c1 + c, v); }

TileView* TileView::makeSlice(const CoordRect& reg)
{
    return new TileView(viewed, reg.shifted(window.r1, window.c1));
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


Matrix::Matrix()
{}

Matrix::Matrix(size_t height, size_t width):
    height(height),
    width(width),
    data(new bool[height * width])
{}

Matrix::Matrix(const AbstractTile& t):
    height(t.getHeight()),
    width(t.getWidth()),
    data(new bool[height * width])
{
    AbstractTile::copyValues(t);
}

Matrix::~Matrix() { delete[] data; }

size_t Matrix::getHeight() const { return height; }
size_t Matrix::getWidth() const { return width; }

bool Matrix::at(coord_t r, coord_t c) const { return data[r * getWidth() + c]; }

void Matrix::set(coord_t r, coord_t c, bool v) { data[r * getWidth() + c] = v; }

void Matrix::operator=(const Matrix& m)
{
    if (width != m.width || height != m.height)
    {
        width = m.width;
        height = m.height;
        delete[] data;
        data = new bool[width * height];
    }
    memcpy(data, m.data, height * width * sizeof(data[0]));
}

Matrix m(2, 2);
TileView t(&m);

} // namespace game_of_life

