#include <sstream>
#include <fstream>

#include "output.hxx"
#include "thread.hxx"

using namespace std;

OstreamLocker::OstreamLocker(OstreamMutex& m):
    ResourceLocker<std::ostream>(m)
{}

OstreamLocker::OstreamLocker(OstreamLocker&& temp):
    ResourceLocker<std::ostream>(std::move(temp))
{}

OstreamLocker::~OstreamLocker()
{
    if (isValid())
        get().flush();
}

#ifdef DEBUG_OUTPUT
DebugStreamLocker::DebugStreamLocker(OstreamMutex& m):
    OstreamLocker(m)
{
    get() << "Thread " << Thread::getCurrentId() << ": ";
}
DebugStreamLocker::DebugStreamLocker(DebugStreamLocker&& temp):
    OstreamLocker(std::move(temp))
{}

DebugStreamLocker::~DebugStreamLocker()
{
    if (isValid())
        get() << "\n";
}
#endif

OstreamMutex coutMutex(cout);
OstreamMutex cerrMutex(cerr);

#ifdef DEBUG_OUTPUT
static ofstream dout(DEBUG_OUTPUT, fstream::out | fstream::app);
#else
static ostringstream dout;
#endif
OstreamMutex debugMutex(dout);

OstreamLocker out() { return OstreamLocker(coutMutex); }
OstreamLocker err() { return OstreamLocker(cerrMutex); }
#ifdef DEBUG_OUTPUT
DebugStreamLocker debug() { return DebugStreamLocker(debugMutex); }
#else
DebugStreamLocker debug() { return DebugStreamLocker(); }
#endif

void out(const string& s) { out() << s; }
void err(const string& s) { err() << s; }

#ifdef DEBUG_OUTPUT
void debug(const string& s) { debug() << s; }
#else
void debug(const string&) {}
#endif

