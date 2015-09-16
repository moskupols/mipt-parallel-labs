#ifndef MANAGER_HXX_INCLUDED
#define MANAGER_HXX_INCLUDED

#include <vector>

#include "thread.hxx"

namespace game_of_life
{

class CoordRect;
class AbstractTile;
class TorusView;

class Manager : protected Thread
{
public:
    static std::vector<CoordRect> chooseDomains(AbstractTile* t, int parts);
    static std::vector<std::vector<int> > makeNeighbors(
            TorusView* t, const std::vector<CoordRect>& r);

    enum State
    {
        NOT_STARTED,
        RUNNING,
        STOPPING,
        STOPPED,
        FINISHED
    };

    Manager();

    State getState() const;

    void wakeWhenStateIs(State s) const;
    void wakeWhenStateIsNot(State s) const;

protected:
    void setState(State s);

    void start();

private:
    State s;
    mutable Mutex stateMutex;
    mutable Cond stateCond;
};

}

#endif

