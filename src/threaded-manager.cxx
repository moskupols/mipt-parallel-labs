#include <memory>

#include "threaded-manager.hxx"
#include "threaded-worker.hxx"

#include "tile.hxx"

#include "utils.hxx"

namespace game_of_life
{

bool ThreadedManagerShared::wakeWhenNextIterationNeeded(int have)
{
    MutexLocker locker(stopMutex);
    while (stop != -1 && stop <= have)
        stopCond.wait(stopMutex);
    return stop != -1;
}

int ThreadedManagerShared::getStop() const
{ return stop; }

void ThreadedManagerShared::setStop(int newStop)
{
    MutexLocker locker(stopMutex);
    stop = newStop;
    stopCond.wakeAll();
}

ThreadedManager::ThreadedManager():
    pauseFlag(false),
    runMore(0),
    shutdownFlag(false)
{}

ThreadedManager::~ThreadedManager()
{
    assert(getState() == NOT_STARTED || getState() == FINISHED);
}

void ThreadedManager::start(Matrix& t, int concurrency)
{
    this->matrix = &t;
    this->concurrency = concurrency;

    Manager::start();
}

ThreadedManagerShared& ThreadedManager::getShared()
{ return myShared; }

void ThreadedManager::pauseAll()
{
    MutexLocker locker(mutex);
    pauseFlag = true;
    cond.wakeOne();
}

void ThreadedManager::runForMore(int iterations)
{
    MutexLocker locker(mutex);
    pauseFlag = false;
    runMore += iterations;
}

void ThreadedManager::shutdown()
{
    MutexLocker locker(mutex);
    shutdownFlag = true;
}

void ThreadedManager::run()
{
    UniqueArray<ThreadedWorker> workers(concurrency);
    UniqueArray<TileView> domains(concurrency);

    TorusView torus(*matrix);

    std::vector<CoordRect> domainRects = chooseDomains(torus, concurrency);
    for (int i = 0; i < concurrency; ++i)
        domains[i] = TileView(torus, domainRects[i]);

    std::vector<std::vector<int> > neigs = makeNeighbors(torus, domainRects);

    for (int i = 0; i < concurrency; ++i)
    {
        std::vector<ThreadedWorkerShared*> shareds(neigs[i].size());
        for (size_t j = 0; j < neigs[i].size(); ++j)
            shareds[j] = &workers[neigs[i][j]].getShared();
        workers[i].start(myShared, domains[i], shareds);
    }

    int stop = 0;
    setState(STOPPED);
    while (true)
    {
        int runMore = 0;
        bool pauseFlag = false;
        {
            MutexLocker locker(mutex);
            while (!this->pauseFlag
                    && !this->runMore
                    && !this->shutdownFlag)
                cond.wait(mutex);

            if (this->shutdownFlag)
                break;
            else if (this->pauseFlag)
                pauseFlag = true;
            else
                runMore = this->runMore;
            this->pauseFlag = false;
            this->runMore = 0;
        }

        if (pauseFlag)
        {
            myShared.setStop(0);
            for (int i = 0; i < concurrency; ++i)
            {
                int newStop = std::min(stop,
                        workers[i].getShared().getIterationPublished() + 1);
                if (newStop > myShared.stop)
                    myShared.setStop(newStop);
            }
            for (int i = 0; i < concurrency; ++i)
                workers[i].getShared().wakeWhenPublishes(myShared.stop);
            stop = myShared.stop;
            setState(STOPPED);
        }

        if (runMore)
        {
            stop += runMore;
            myShared.setStop(stop);
            setState(RUNNING);
        }
    }
    myShared.setStop(-1);

    for (int i = 0; i < concurrency; ++i)
        workers[i].join();
}

} // namespace game_of_life

