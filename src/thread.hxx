#ifndef THREAD_HXX_INCLUDED
#define THREAD_HXX_INCLUDED

#include <pthread.h>
#include <memory>

#include "utils.hxx"

class Thread : Noncopyable
{
public:
    virtual ~Thread();

    void join();

    bool isRunning() const;

    bool isCurrent() const;

protected:
    Thread();

    void exit();

    void start();
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

class MutexLocker : Noncopyable
{
public:
    explicit MutexLocker(Mutex& m);
    MutexLocker(MutexLocker&& temp);
    ~MutexLocker();

private:
    Mutex* m;
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

template<class R> class ResourceLocker;

template<class R>
class ResourceMutex
{
public:
    ResourceMutex(R& r):
        res(r)
    {}

private:
    friend class ResourceLocker<R>;

    R& res;
    Mutex m;
};

template<class R>
class ResourceLocker : MutexLocker
{
public:
    ResourceLocker(ResourceMutex<R>& r):
        MutexLocker(r.m),
        r(r)
    {}
    ResourceLocker(ResourceLocker<R>&& temp):
        MutexLocker(std::move(temp)),
        r(temp.r)
    {}

    R& get() { return r.res; }
private:
    ResourceMutex<R>& r;
};

#endif

