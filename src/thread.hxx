#ifndef THREAD_HXX_INCLUDED
#define THREAD_HXX_INCLUDED

#include <pthread.h>

#include "utils.hxx"

class Thread : Noncopyable
{
public:
    virtual ~Thread();

    void start();
    void join();

    bool isRunning() const;

    bool isCurrent() const;

protected:
    Thread();

    void exit();

    virtual void run() = 0;

private:
    static void* threadRunner(void* me);

    void wrappedRun();

    pthread_t descriptor;
    bool running;
};

class Mutex
{
public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();

private:
    friend class Cond;

    pthread_mutex_t descriptor;
};

class Cond
{
public:
    Cond();
    ~Cond();

    void wait(Mutex& m);
    void wakeOne();
    void wakeAll();

private:
    pthread_cond_t descriptor;
};

#endif

