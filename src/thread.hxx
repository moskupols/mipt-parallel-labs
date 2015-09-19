#ifndef THREAD_HXX_INCLUDED
#define THREAD_HXX_INCLUDED

#include <pthread.h>
#include <semaphore.h>
#include <memory>

#include "utils.hxx"

class Thread : Noncopyable
{
public:
    virtual ~Thread();

    void join();

    bool isRunning() const;

    bool isCurrent() const;

    int getId() const;
    static int getCurrentId();

protected:
    Thread();

    void exit();

    void start();
    virtual void run() = 0;

private:
    static int getIdFor(pthread_t d);

    static void* threadRunner(void* me);

    void wrappedRun();

    pthread_t descriptor;
    bool running;
};

class AbstractMutex
{
public:
    virtual ~AbstractMutex() {}

    virtual void lock() = 0;
    virtual void unlock() = 0;
};

class Mutex : public AbstractMutex
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
    explicit MutexLocker(AbstractMutex& m);
    MutexLocker(MutexLocker&& temp);
    ~MutexLocker();

protected:
    bool isValid() const;

private:
    AbstractMutex* m;
    bool valid;
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
class ResourceLocker : public MutexLocker
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

class Semaphore
{
public:
    explicit Semaphore(int value = 0);
    ~Semaphore();

    void post();
    void wait();

private:
    sem_t sem;
};

class SemaphoreMutex : public AbstractMutex
{
public:
    SemaphoreMutex();

    void lock();
    void unlock();

private:
    Semaphore sem;
};

#endif

