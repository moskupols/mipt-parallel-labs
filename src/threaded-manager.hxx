#ifndef THREADED_MANAGER_HXX_INCLUDED
#define THREADED_MANAGER_HXX_INCLUDED

#include "threaded-worker.hxx"

namespace game_of_life
{

class ThreadedManager
{
public:
    bool wakeWhenNextIterationNeeded(int iterationPublished);
};

} // namespace game_of_life

#endif

