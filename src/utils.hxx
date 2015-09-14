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

template<class T>
class UniqueArray
{
public:
    explicit UniqueArray(unsigned long size):
        data(new T[size]) 
    {}

    ~UniqueArray()
    { delete[] data; }

    T& operator[](unsigned long at)
    { return data[at]; }

public:
    T* const data;
};

void throwCError(int line, const char* file);

#ifndef NULL
#define NULL 0
#endif

#define THROW_C_ERROR() \
    throwCError(__LINE__, __FILE__)

#endif

