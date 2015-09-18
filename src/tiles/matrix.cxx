#include "matrix.hxx"

#include <cstring>

namespace game_of_life
{
Matrix::Matrix()
{}

Matrix::Matrix(size_t height, size_t width):
    height(height),
    width(width),
    data(new bool[height * width])
{
    memset(data, 0, height * width * sizeof(bool));
}

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
}

