#ifndef OUTPUT_HXX_INCLUDED
#define OUTPUT_HXX_INCLUDED

#include <iostream>
#include <string>
#include "thread.hxx"

typedef ResourceMutex<std::ostream> OstreamMutex;

class OstreamLocker : public ResourceLocker<std::ostream>
{
public:
    OstreamLocker(OstreamMutex& m);
    OstreamLocker(OstreamLocker&& temp);
    ~OstreamLocker();

    template<class T>
    OstreamLocker& operator << (const T& some)
    {
        get() << some;
        return *this;
    }
};

class DebugStreamLocker : public OstreamLocker
{
public:
    DebugStreamLocker(OstreamMutex& m);
    DebugStreamLocker(DebugStreamLocker&& temp);
    ~DebugStreamLocker();
};

extern OstreamMutex coutMutex;
extern OstreamMutex cerrMutex;
extern OstreamMutex debugMutex;

OstreamLocker out();
OstreamLocker err();
DebugStreamLocker debug();

void out(const std::string&);
void err(const std::string&);
void debug(const std::string&);

#endif

