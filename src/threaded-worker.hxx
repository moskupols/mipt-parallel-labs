#ifndef THREADED_WORKER_HXX_INCLUDED
#define THREADED_WORKER_HXX_INCLUDED

#include "thread.hxx"
#include "tile.hxx"
#include "worker.hxx"

namespace game_of_life
{

class ThreadedManagerShared;
class ThreadedWorker;

class ThreadedWorkerShared
{
public:
    ThreadedWorkerShared();

    int getIterationCalced() const;
    int getIterationPublished() const;

    void wakeWhenCalcs(int iteration);
    void wakeWhenPublishes(int iteration);

private:
    friend class ThreadedWorker;

    void incIterationCalced();
    int iterationCalced;
    Mutex calcedMutex;
    Cond calcedCond;

    void incIterationPublished();
    int iterationPublished;
    Mutex publishMutex;
    Cond publishCond;
};

class ThreadedWorker : public Thread, protected Worker
{
public:
    void start(
            ThreadedManagerShared& manager,
            AbstractTile& domain,
            const std::vector<ThreadedWorkerShared*>& neighShared);

    ThreadedWorkerShared& getShared();

protected:
    void run();

private:
    ThreadedWorkerShared myShared;

    ThreadedManagerShared* manager;
    AbstractTile* domain;
    std::vector<ThreadedWorkerShared*> neighShared;
};

} // namespace game_of_life

#endif

