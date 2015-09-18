#ifndef MATRIX_HXX_INCLUDED
#define MATRIX_HXX_INCLUDED

#include <iostream>
#include "abstract-tile.hxx"

namespace game_of_life
{

class Matrix : public AbstractTile
{
public:
    Matrix();
    Matrix(size_t height, size_t width);
    explicit Matrix(const AbstractTile& t);
    explicit Matrix(const Matrix& m);
    ~Matrix();

    size_t getHeight() const;
    size_t getWidth() const;

    bool at(coord_t r, coord_t c) const;
    void set(coord_t r, coord_t c, bool v);

    void operator=(const Matrix& m);

private:
    size_t height;
    size_t width;
    bool* data;
};

} // namespace game_of_life

#endif
