#ifndef TILE_VIEW_HXX_INCLUDED
#define TILE_VIEW_HXX_INCLUDED

#include "abstract-tile.hxx"

namespace game_of_life
{

class TileView : public AbstractTile
{
public:
    TileView();
    explicit TileView(AbstractTile& viewed);
    TileView(const TileView& viewed);
    TileView(AbstractTile& viewed, const CoordRect& r);

    TileView& operator = (const TileView& that);

    size_t getWidth() const;
    size_t getHeight() const;

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    TileView makeSlice(const CoordRect& reg);

    CoordRect getWindow() const;

    AbstractTile& getViewed();
    const AbstractTile& getViewed() const;

private:
    AbstractTile* viewed;
    CoordRect window;
};


class TorusView : public TileView
{
public:
    explicit TorusView(AbstractTile& viewed);

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    coord_t normalizeRow(coord_t r) const;
    coord_t normalizeColumn(coord_t c) const;
protected:
    static coord_t normalizeCoord(coord_t c, coord_t dimen);
};

class FrameView : public TorusView
{
public:
    FrameView(AbstractTile& center, AbstractTile& top, AbstractTile& bottom);

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    coord_t normalizeRowForPart(coord_t r) const;
    const AbstractTile& getPartAt(coord_t r, coord_t c) const;
    AbstractTile& getPartAt(coord_t r, coord_t c);

private:
    AbstractTile *top;
    AbstractTile *bottom;
};

}

#endif

