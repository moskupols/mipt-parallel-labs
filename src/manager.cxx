#include "manager.hxx"
#include "tile.hxx"

#include <set>

using namespace std;

namespace
{
using namespace game_of_life;

template<class RAIt>
void chooseDomains(RAIt begin, RAIt end,
        coord_t r1, coord_t c1, coord_t r2, coord_t c2)
{
    assert(r2 > r1);
    assert(c2 > c1);
    if (begin + 1 == end)
    {
        *begin = CoordRect(r1, c1, r2, c2);
        return;
    }
    assert(r2-r1 + c2-c1 > 2);

    RAIt mid = begin + (end - begin) / 2;
    if (r2 - r1 > c2 - c1)
    {
        chooseDomains(begin, mid, r1,        c1, (r1+r2)/2, c2);
        chooseDomains(mid,   end, (r1+r2)/2, c1, r2,        c2);
    }
    else
    {
        chooseDomains(begin, mid, r1,        c1, r2, (c1+c2)/2);
        chooseDomains(mid,   end, r1, (c1+c2)/2, r2, c2);
    }
}

bool near(coord_t r, coord_t c, const CoordRect& rect, const TorusView& t)
{
    for (size_t i = 0; i < DIRECTION_COUNT; ++i)
        if (rect.contains(
                    t.normalizeRow(r + DIRECTION[i][0]),
                    t.normalizeColumn(c + DIRECTION[i][1])))
            return true;
    return false;
}

}

namespace game_of_life
{

vector<CoordRect> Manager::chooseDomains(const AbstractTile& t, int parts)
{
    vector<CoordRect> ret(parts);

    coord_t h = t.getHeight();
    coord_t w = t.getWidth();

    if (parts > h * w)
        parts = h * w;

    ::chooseDomains(ret.begin(), ret.begin() + parts, 0, 0, h, w);
    return ret;
}

vector<vector<int> > Manager::makeNeighbors(
        const TorusView& t, const vector<CoordRect>& r)
{
    vector<vector<int> > ret(r.size());
    set<pair<int, int> > pairs;
    for (size_t i = 0; i < r.size(); ++i)
        for (size_t j = 0; j < r.size(); ++j)
            if (i != j)
            {
                const CoordRect& a = r[i];
                const CoordRect& b = r[j];
                if (near(a.r1, a.c1, b, t)
                        || near(a.r1, a.c2-1, b, t)
                        || near(a.r2-1, a.c1, b, t)
                        || near(a.r2-1, a.c2-1, b, t))
                    pairs.insert(make_pair(min(i, j), max(i, j)));
            }
    for (set<pair<int, int> >::iterator it = pairs.begin(); it != pairs.end(); ++it)
    {
        ret[it->first].push_back(it->second);
        ret[it->second].push_back(it->first);
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
    this->s = s;
    stateCond.wakeAll();
}

void Manager::start()
{
    setState(RUNNING);
    Thread::start();
    setState(FINISHED);
}

} // namespace game_of_life

