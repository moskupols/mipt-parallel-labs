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

OstreamLocker out() { return OstreamLocker(coutMutex); }
OstreamLocker err() { return OstreamLocker(cerrMutex); }
OstreamLocker debug() { return DebugStreamLocker(debugMutex); }

void out(const string& s) { out() << s; }
void err(const string& s) { err() << s; }

#ifdef DEBUG_OUTPUT
void debug(const string& s) { debug() << s; }
#else
void debug(const string&) {}
#endif

