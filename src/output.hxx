#ifndef OUTPUT_HXX_INCLUDED
#define OUTPUT_HXX_INCLUDED

#include <iostream>
#include "thread.hxx"

typedef ResourceMutex<std::ostream> OstreamMutex;

class OstreamLocker : public ResourceLocker<std::ostream>
{
public:
    OstreamLocker(OstreamMutex& m):
        ResourceLocker<std::ostream>(m)
    {}

    ~OstreamLocker()
    {
        get().flush();
    }
};

extern OstreamMutex coutMutex;
extern OstreamMutex cerrMutex;
extern OstreamMutex debugMutex;

void out(OstreamMutex& mut, const std::string& msg);

void out(const std::string& msg);
void outLn(const std::string& msg);

void err(const std::string& msg);
void errLn(const std::string& msg);

void debug(const std::string& msg);
void debugLn(const std::string& msg);

#endif

