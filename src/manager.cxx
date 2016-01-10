#include "manager.hxx"
#include "tile.hxx"
#include "output.hxx"

#include <set>

using namespace std;

namespace game_of_life
{

vector<CoordRect> Manager::chooseDomains(const AbstractTile& t, int parts)
{
    vector<CoordRect> ret(parts);

    coord_t h = t.getHeight();
    coord_t w = t.getWidth();

    int actualParts = min((int)h, parts);
    coord_t stride = h / actualParts;
    for (int i = 0; i < actualParts; ++i)
        ret[i] = CoordRect(i*stride, 0, (i+1)*stride, w);
    ret[actualParts-1].r2 = h;

    return ret;
}

vector<vector<int> > Manager::makeNeighbors(const vector<CoordRect>& r)
{
    int n = r.size();
    vector<vector<int> > ret(n);
    for (int i = 0; i < n; ++i)
        if (!r[i].isEmpty())
        {
            ret[i].push_back((i-1+n)%n);
            ret[i].push_back((i+1)%n);
        }
    return ret;
}

Manager::Manager():
    s(NOT_STARTED)
{}

Manager::State Manager::getState() const
{
    return s;
}

string Manager::getStateStr() const
{
    return stateStr(getState());
}

string Manager::stateStr(State s)
{
    switch (s)
    {
#define HANDLE(c) case c: return #c;
        HANDLE(NOT_STARTED)
        HANDLE(RUNNING)
        HANDLE(STOPPING)
        HANDLE(STOPPED)
        HANDLE(FINISHED)
#undef HANDLE
    default:
        assert(false);
    }
}

void Manager::wakeWhenStateIsNot(State s) const
{
    MutexLocker locker(stateMutex);
    while (getState() == s)
        stateCond.wait(stateMutex);
}

void Manager::wakeWhenStateIs(State s) const
{
    MutexLocker locker(stateMutex);
    while (getState() != s)
        stateCond.wait(stateMutex);
}

void Manager::setState(State s)
{
    if (this->s == s)
        return;
    MutexLocker locker(stateMutex);
    debug() << "manager: new state: " << stateStr(s);
    this->s = s;
    stateCond.wakeAll();
}

} // namespace game_of_life

