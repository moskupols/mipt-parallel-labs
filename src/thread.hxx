#ifndef THREAD_HXX_INCLUDED
#define THREAD_HXX_INCLUDED

#include <pthread.h>

#include "utils.h"

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

#endif

