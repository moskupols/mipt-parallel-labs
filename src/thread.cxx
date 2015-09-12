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

