#include "thread.hxx"
#include "output.hxx"

#include <map>

using namespace std;

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

int Thread::getId() const
{
    return getIdFor(descriptor);
}

int Thread::getCurrentId()
{
    return getIdFor(pthread_self());
}

int Thread::getIdFor(pthread_t p)
{
    static map<pthread_t, int> ids;
    auto it = ids.find(p);
    if (it == ids.end())
    {
        int ret = ids.size();
        ids[p] = ret;
        return ret;
    }
    return it->second;
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
    debug() << "STARTING (pthreads id " << descriptor << ")";
    run();
    debug() << "STOPPING (pthreads id " << descriptor << ")";
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



MutexLocker::MutexLocker(AbstractMutex& m):
    m(&m),
    valid(true)
{
    m.lock();
}

MutexLocker::MutexLocker(MutexLocker&& temp):
    m(temp.m),
    valid(temp.valid)
{
    temp.valid = false;
}

MutexLocker::~MutexLocker()
{
    if (isValid())
        m->unlock();
}

bool MutexLocker::isValid() const
{
    return valid;
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
    debug("SLEEPING");
    if (pthread_cond_wait(&descriptor, &m.descriptor))
        THROW_C_ERROR();
    debug("AWAKE");
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



Semaphore::Semaphore(int value)
{
    if (sem_init(&sem, 0, value)) // 0 stands for 'don't share with other processes'
        THROW_C_ERROR();
}

Semaphore::~Semaphore()
{
    if (sem_destroy(&sem))
        THROW_C_ERROR();
}

void Semaphore::post()
{
    if (sem_post(&sem))
        THROW_C_ERROR();
}

void Semaphore::wait()
{
    if (sem_wait(&sem))
        THROW_C_ERROR();
}



SemaphoreMutex::SemaphoreMutex():
    sem(1)
{}

void SemaphoreMutex::lock()
{
    sem.wait();
}

void SemaphoreMutex::unlock()
{
    sem.post();
}

