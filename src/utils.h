#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <cassert>

class Noncopyable
{
protected:
    Noncopyable() {}

private:
    Noncopyable(const Noncopyable&);
    void operator=(const Noncopyable&);
};

#ifndef NULL
#define NULL 0
#endif

#define THROW_C_ERROR()

#endif

