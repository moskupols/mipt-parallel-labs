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
DebugStreamLocker debug() { return DebugStreamLocker(debugMutex); }

void out(const string& s) { out() << s; }
void err(const string& s) { err() << s; }

#ifdef DEBUG_OUTPUT
void debug(const string& s) { debug() << s; }
#else
void debug(const string&) {}
#endif

