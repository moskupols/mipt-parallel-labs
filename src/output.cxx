#include <sstream>
#include <fstream>

#include "output.hxx"
#include "thread.hxx"

using namespace std;

OstreamMutex coutMutex(cout);
OstreamMutex cerrMutex(cerr);

#ifdef DEBUG_OUTPUT
static ofstream dout(DEBUG_OUTPUT, fstream::out | fstream::app);
#else
static ostringstream dout;
#endif
OstreamMutex debugMutex(dout);

void out(OstreamMutex& mut, const string& msg)
{
    OstreamLocker locker(mut);
    locker.get() << msg;
}

void out(const string& msg) { out(coutMutex, msg); }
void outLn(const string& msg) { out(coutMutex, msg + "\n"); }

void err(const string& msg) { out(cerrMutex, msg); }
void errLn(const string& msg) { out(cerrMutex, msg + "\n"); }

void debug(const string& msg) { out(debugMutex, msg); }
void debugLn(const string& msg) { out(debugMutex, msg + "\n"); }

