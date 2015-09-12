#ifndef GOL_H_INCLUDED
#define GOL_H_INCLUDED

#include <vector>

#include <cstddef>

namespace game_of_life
{

typedef std::ssize_t coord_t;
typedef std::size_t size_t;

class Tile;

typedef Tile Border;
typedef std::vector<Border*> Borders;


class Tile
{
public:
    enum Side
    {
        SIDE_NW = 0,
        SIDE_N = 1,
        SIDE_NE = 2,
        SIDE_E = 3,
        SIDE_SE = 4,
        SIDE_S = 5,
        SIDE_SW = 6,
        SIDE_W = 7,
    };
    const size_t SIDE_COUNT = SIDE_W + 1;


    Tile(size_t height, size_t width);
    virtual ~Tile() {}

    size_t getHeight() const;
    size_t getWidth() const;

    virtual bool at(coord_t r, coord_t c) const = 0;
    virtual bool& at(coord_t r, coord_t c) = 0;

    Borders makeBorders const;
    virtual Border* makeBorder(Side s) const = 0;

    Borders makeFrame() const;

private:
    size_t height;
    size_t width;
};


class WorkerCommunicator
{
public:
    virtual void beginIteration() = 0;

    virtual void sendBorders(Borders t) = 0;
    virtual Borders receiveBorders() = 0;
    virtual void waitForBordersRead() = 0;

    virtual void endIteration() = 0;
};


class Worker
{
public:
    Worker(
        WorkerCommunicator* c,
        Tile* m);

    void workInfinitely();

protected:
    void makeIteration();

private:
    WorkerCommunicator* communicator;
    Tile* tile;
};

}

#endif

