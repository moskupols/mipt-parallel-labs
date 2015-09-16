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
    bool wakeWhenNextIterationNeeded(int iterationPublished);

    int getStop() const;

private:
    friend class ThreadedManager;

    void setStop(int newStop);
    int stop;
    Mutex stopMutex;
    Cond stopCond;
};

class ThreadedManager : public Manager
{
public:
    ThreadedManager();
    ~ThreadedManager();

    void start(Matrix* m, int concurrency);

    ThreadedManagerShared* getShared();

    void pauseAll();
    void runForMore(int iterations);
    void shutdown();

protected:
    void run();

private:
    ThreadedManagerShared myShared;

    Matrix* matrix;
    int concurrency;

    bool pauseFlag;
    int runMore;
    bool shutdownFlag;

    Mutex mutex;
    Cond cond;
};

} // namespace game_of_life

#endif

