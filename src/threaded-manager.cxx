#include <memory>
#include <cassert>
#include <cstdlib>
#include <algorithm>

#include "threaded-manager.hxx"
#include "threaded-worker.hxx"

#include "tile.hxx"

#include "utils.hxx"
#include "output.hxx"
#include "exceptions.hxx"

using namespace std;

namespace game_of_life
{

ThreadedManagerShared::ThreadedManagerShared(ThreadedManager& manager):
    workersWaiting(0),
    stop(0),
    manager(manager)
{}

bool ThreadedManagerShared::wakeWhenNextIterationNeeded(int have)
{
    if (stop != -1 && stop <= have)
    {
        MutexLocker locker(stopMutex);
        incWorkersWaiting();
        while (stop != -1 && stop <= have)
            stopCond.wait(stopMutex);
        decWorkersWaiting();
    }
    return stop != -1;
}

void ThreadedManagerShared::wakeWhenStopIs(int needed)
{
    MutexLocker locker(stopMutex);
    while (getStop() != needed)
        stopCond.wait(stopMutex);
}

int ThreadedManagerShared::getStop() const
{ return stop; }

int ThreadedManagerShared::getWorkersWaiting() const
{
    return workersWaiting;
}

void ThreadedManagerShared::incWorkersWaiting()
{
    MutexLocker locker(workersMutex);
    ++workersWaiting;
    debug() << workersWaiting << "/" << workersCount << " workers waiting";
    if (workersWaiting == workersCount)
        manager.updateState();
}

void ThreadedManagerShared::decWorkersWaiting()
{
    MutexLocker locker(workersMutex);
    --workersWaiting;
    debug() << workersWaiting << "/" << workersCount << " workers waiting";
}

void ThreadedManagerShared::setWorkersCount(int count)
{
    workersCount = count;
}

void ThreadedManagerShared::setStop(int newStop)
{
    MutexLocker locker(stopMutex);
    debug() << "setting iteration stopper to " << newStop;
    stop = newStop;
    stopCond.wakeAll();
}

ThreadedManager::ThreadedManager():
    myShared(*this),
    pauseFlag(false),
    runMore(0),
    shutdownFlag(false),
    updateFlag(false)
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
    wakeWhenStateIsNot(NOT_STARTED);
}

ThreadedManagerShared& ThreadedManager::getShared()
{ return myShared; }

void ThreadedManager::pauseAll()
{
    assert(getState() != NOT_STARTED);
    {
        MutexLocker locker(mutex);
        pauseFlag = true;
        cond.wakeOne();
    }
    wakeWhenStateIs(STOPPED);
}

void ThreadedManager::runForMore(int iterations)
{
    int oldStop;
    {
        MutexLocker locker(mutex);
        oldStop = getShared().getStop();
        pauseFlag = false;
        runMore = iterations;
        cond.wakeOne();
    }
    getShared().wakeWhenStopIs(oldStop + iterations);
}

void ThreadedManager::shutdown()
{
    {
        MutexLocker locker(mutex);
        shutdownFlag = true;
        cond.wakeOne();
    }
    getShared().wakeWhenStopIs(-1);
}

void ThreadedManager::updateState()
{
    MutexLocker locker(mutex);
    updateFlag = true;
    cond.wakeOne();
}

void ThreadedManager::run()
{
    debug("initializing workers");
    concurrency = min(concurrency, (int)matrix->getHeight());
    getShared().setWorkersCount(concurrency);

    UniqueArray<ThreadedWorker> workers(concurrency);
    TorusView torus(*matrix);
    vector<TileView> domains;
    vector<std::vector<int> > neigs;
    try
    {
        std::vector<CoordRect> domainRects = chooseDomains(torus, concurrency);
        for (int i = 0; i < concurrency; ++i)
            domains.push_back(TileView(torus, domainRects[i]));

        neigs = makeNeighbors(torus, domainRects);

        for (int i = 0; i < concurrency; ++i)
        {
            std::vector<ThreadedWorkerShared*> shareds(neigs[i].size());
            for (size_t j = 0; j < neigs[i].size(); ++j)
                shareds[j] = &workers[neigs[i][j]].getShared();
            workers[i].start(myShared, domains[i], shareds);
        }

        {
            auto d = debug();
            d << "Workers scheme:\n";
            for (size_t i = 0; i < neigs.size(); ++i)
            {
                if (neigs[i].empty())
                    continue;
                d << "\t" << workers[neigs[i][0]].getId() << "\n";
                d << workers[i].getId() << " " << domainRects[i] << "\n";
                d << "\t" << workers[neigs[i][1]].getId() << "\n";
            }
        }
    }
    catch (exception& e)
    {
        err() << "Failed to initialize workers: " << e.what() << "\n";
        ::exit(1);
    }

    debug("waiting for workers to initialize themselves");
    {
        MutexLocker locker(mutex);
        while (!this->updateFlag)
            cond.wait(mutex);
        this->updateFlag = false;
    }

    debug("all workers are initialized");
    int stop = 0;
    setState(STOPPED);
    while (true)
    {
        int runMore = 0;
        bool pauseFlag = false;
        bool updateFlag = false;
        {
            MutexLocker locker(mutex);
            while (!this->pauseFlag
                    && !this->runMore
                    && !this->shutdownFlag
                    && !this->updateFlag)
                cond.wait(mutex);

            if (this->shutdownFlag)
            {
                debug("manager received shutdownFlag");
                break;
            }
            else if (this->pauseFlag)
            {
                debug("manager received pauseFlag");
                pauseFlag = true;
            }
            else
            {
                runMore = this->runMore;
                updateFlag = this->updateFlag;
                if (runMore)
                    debug() << "manager received runMore = " << runMore;
                if (updateFlag)
                    debug("manager received updateFlag");
            }
            this->pauseFlag = false;
            this->runMore = 0;
            this->updateFlag = false;
        }

        if (pauseFlag)
        {
            myShared.setStop(0);
            for (int i = 0; i < concurrency; ++i)
            {
                int newStop = std::min(stop,
                        workers[i].getShared().getIterationCompleted() + 1);
                if (newStop > myShared.stop)
                    myShared.setStop(newStop);
            }
            {
                MutexLocker locker(mutex);
                while (!this->updateFlag)
                    cond.wait(mutex);
            }
            stop = myShared.stop;
            setState(STOPPED);
        }

        if (runMore)
        {
            stop += runMore;
            myShared.setStop(stop);
            setState(RUNNING);
        }

        if (updateFlag)
        {
            if (getState() == RUNNING
                    && getShared().getWorkersWaiting() == concurrency)
            {
                bool over = true;
                for (int i = 0; i < concurrency; ++i)
                    over &= workers[i].getShared().getIterationCompleted() == stop;
                if (over)
                    setState(STOPPED);
            }
        }
    }
    myShared.setStop(-1);

    for (int i = 0; i < concurrency; ++i)
    {
        int id = workers[i].getId();
        workers[i].join();
        debug() << "manager joined worker thread " << id;
    }

    setState(FINISHED);
}

} // namespace game_of_life

