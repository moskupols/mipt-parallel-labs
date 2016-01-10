#include "tile-view.hxx"

#include <cassert>

namespace game_of_life
{

TileView::TileView()
{}

TileView::TileView(AbstractTile& viewed):
    viewed(&viewed),
    window(0, 0, -10, -10)
{}

TileView::TileView(const TileView& that):
    viewed(that.viewed),
    window(that.window)
{}

TileView::TileView(AbstractTile& viewed, const CoordRect& r):
    viewed(&viewed),
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

TileView TileView::makeSlice(const CoordRect& reg)
{
    return TileView(*viewed, reg.shifted(window.r1, window.c1));
}

CoordRect TileView::getWindow() const
{
    return window;
}

AbstractTile& TileView::getViewed()
{
    return *viewed;
}

const AbstractTile& TileView::getViewed() const
{
    return *viewed;
}


TorusView::TorusView(AbstractTile& viewed):
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



FrameView::FrameView(AbstractTile& center, AbstractTile& top, AbstractTile& bottom):
    TorusView(center),
    top(&top),
    bottom(&bottom)
{
    assert(top.getHeight() == 1);
    assert(bottom.getHeight() == 1);
}

bool FrameView::at(coord_t r, coord_t c) const
{
    return getPartAt(r, c)
        .at(normalizeRowForPart(r), normalizeColumn(c));
}

void FrameView::set(coord_t r, coord_t c, bool v)
{
    getPartAt(r, c)
        .set(normalizeRowForPart(r), normalizeColumn(c), v);
}

coord_t FrameView::normalizeRowForPart(coord_t r) const
{
    if (r >= 0 && r < (coord_t)getHeight())
        return r;
    return 0;
}

const AbstractTile& FrameView::getPartAt(coord_t r, coord_t) const
{
    if (r == -1)
        return *top;
    if (r == (coord_t)getHeight())
        return *bottom;
    return getViewed();
}

AbstractTile& FrameView::getPartAt(coord_t r, coord_t)
{
    if (r == -1)
        return *top;
    if (r == (coord_t)getHeight())
        return *bottom;
    return getViewed();
}

}
