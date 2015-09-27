#ifndef THREADED_WORKER_HXX_INCLUDED
#define THREADED_WORKER_HXX_INCLUDED

#include <vector>

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

    int getIterationCalced(bool south) const;
    int getIterationPublished(bool south) const;
    int getIterationCompleted() const;

    void wake();

protected:
    void incIterationCalced(bool south);
    void incIterationPublished(bool south);

    void sleep();

private:
    friend class ThreadedWorker;

    int iterationCalced[2];
    int iterationPublished[2];

    Semaphore sem;
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

