#ifndef THREADED_MANAGER_HXX_INCLUDED
#define THREADED_MANAGER_HXX_INCLUDED

#include "thread.hxx"
#include "manager.hxx"

namespace game_of_life
{

class ThreadedWorker;
class ThreadedManager;
class Matrix;

class ThreadedManagerShared
{
public:
    ThreadedManagerShared(ThreadedManager& manager);

    bool wakeWhenNextIterationNeeded(int iterationPublished);

    int getStop() const;

protected:
    friend class ThreadedManager;

    int getWorkersWaiting() const;

    void incWorkersWaiting();
    void decWorkersWaiting();

    void setWorkersCount(int count);

    void setStop(int newStop);

private:
    int workersCount;
    int workersWaiting;
    SemaphoreMutex workersMutex;

    int stop;
    Mutex stopMutex;
    Cond stopCond;

    ThreadedManager& manager;
};

class ThreadedManager : public Manager
{
public:
    ThreadedManager();
    ~ThreadedManager();

    void start(Matrix& m, int concurrency);

    ThreadedManagerShared& getShared();

    void pauseAll();
    void runForMore(int iterations);
    void shutdown();

    void updateState();

protected:
    void run();

private:
    ThreadedManagerShared myShared;

    Matrix* matrix;
    int concurrency;

    bool pauseFlag;
    int runMore;
    bool shutdownFlag;
    bool updateFlag;

    Mutex mutex;
    Cond cond;
};

} // namespace game_of_life

#endif

