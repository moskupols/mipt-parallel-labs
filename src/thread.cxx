#include "thread.hxx"

Thread::~Thread()
{
    assert(!isRunning());
}

void Thread::start()
{
    if (pthread_create(&descriptor, NULL, threadRunner, (void*)this))
        THROW_C_ERROR();
}

void Thread::join()
{
    assert(!isCurrent());
    if (pthread_join(descriptor, NULL))
        THROW_C_ERROR();
}

bool Thread::isRunning() const
{
    return running;
}

bool Thread::isCurrent() const
{
    return pthread_self() == descriptor;
}

Thread::Thread():
    running(false)
{}

void* Thread::threadRunner(void* me)
{
    static_cast<Thread*>(me)->wrappedRun();
    return NULL;
}

void Thread::wrappedRun()
{
    running = true;
    run();
    running = false;
}



Mutex::Mutex()
{
    pthread_mutex_t d = PTHREAD_MUTEX_INITIALIZER;
    descriptor = d;
}

Mutex::~Mutex()
{}

void Mutex::lock()
{
    pthread_mutex_lock(&descriptor);
}

void Mutex::unlock()
{
    if (pthread_mutex_unlock(&descriptor))
        THROW_C_ERROR();
}



MutexLocker::MutexLocker(Mutex& m):
    m(m)
{
    m.lock();
}

MutexLocker::~MutexLocker()
{
    m.unlock();
}



Cond::Cond()
{
    pthread_cond_t d = PTHREAD_COND_INITIALIZER;
    descriptor = d;
}

Cond::~Cond()
{}

void Cond::wait(Mutex& m)
{
    if (pthread_cond_wait(&descriptor, &m.descriptor))
        THROW_C_ERROR();
}

void Cond::wakeOne()
{
    if (pthread_cond_signal(&descriptor))
        THROW_C_ERROR();
}

void Cond::wakeAll()
{
    if (pthread_cond_broadcast(&descriptor))
        THROW_C_ERROR();
}

