#ifndef OUTPUT_HXX_INCLUDED
#define OUTPUT_HXX_INCLUDED

#include <iostream>
#include <string>
#include "thread.hxx"

typedef ResourceMutex<std::ostream> OstreamMutex;

class OstreamLocker : public ResourceLocker<std::ostream>
{
public:
    OstreamLocker(OstreamMutex& m):
        ResourceLocker<std::ostream>(m)
    {}
    OstreamLocker(OstreamLocker&& temp):
        ResourceLocker<std::ostream>(std::move(temp))
    {}

    ~OstreamLocker()
    {
        get().flush();
    }

    template<class T>
    OstreamLocker& operator << (T some)
    {
        get() << some;
        return *this;
    }
};

extern OstreamMutex coutMutex;
extern OstreamMutex cerrMutex;
extern OstreamMutex debugMutex;

OstreamLocker out();
OstreamLocker err();
OstreamLocker debug();

void out(const std::string&);
void err(const std::string&);
void debug(const std::string&);

#endif

