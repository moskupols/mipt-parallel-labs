#include "matrix.hxx"

#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "../exceptions.hxx"

using namespace std;

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

Matrix::Matrix(Matrix&& temp):
    height(temp.height),
    width(temp.width),
    data(temp.data)
{
    temp.data = NULL;
}

Matrix::~Matrix() { delete[] data; }

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

size_t Matrix::getHeight() const { return height; }
size_t Matrix::getWidth() const { return width; }

bool Matrix::at(coord_t r, coord_t c) const { return data[r * getWidth() + c]; }
void Matrix::set(coord_t r, coord_t c, bool v) { data[r * getWidth() + c] = v; }

bool* Matrix::getData()
{
    return data;
}

Matrix Matrix::fromCsv(std::istream& in)
{
    vector<pair<int, int>> alive;
    string line;
    int h = 0;
    int w = 0;
    for (; getline(in, line); ++h)
    {
        istringstream iss(line);
        int i = 0;
        string s;
        for (; getline(iss, s, ';'); ++i)
        {
            if (s == "1" || s == "#")
                alive.push_back(make_pair(h, i));
            else if (s != "0" && s != ".")
                throw InputError("CSV incorrect");
        }
        w = max(w, i);
    }
    Matrix ret(h, w);
    for (size_t i = 0; i < alive.size(); ++i)
        ret.set(alive[i].first, alive[i].second, true);
    return ret;
}

Matrix Matrix::random(size_t height, size_t width, int seed)
{
    Matrix ret(height, width);

    srand(seed);
    int n = rand() % (height * width + 1);

    memset(ret.data, 1, n * sizeof(bool));
    random_shuffle(ret.data, ret.data + height * width);

    return ret;
}

}

