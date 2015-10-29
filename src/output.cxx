#include <sstream>
#include <fstream>

#include "output.hxx"
#include "thread.hxx"

using namespace std;

#ifdef DEBUG_OUTPUT
static ofstream dout(DEBUG_OUTPUT, fstream::out | fstream::app);
#else
static ostringstream dout;
#endif

#ifdef DEBUG_OUTPUT
DebugStreamFlusher::DebugStreamFlusher(ostream& m):
    out(&m)
{
    *out << "Thread " << Thread::getCurrentId() << ": ";
}
DebugStreamFlusher::DebugStreamFlusher(DebugStreamFlusher&& temp):
    out(temp.out)
{
    temp.out = nullptr;
}

DebugStreamFlusher::~DebugStreamFlusher()
{
    if (out)
        *out << "\n";
}

ostream& DebugStreamFlusher::get()
{
    return *out;
}
#else
ostream& DebugStreamFlusher::get()
{
    return dout;
}
#endif

std::ostream& out() { return cout; }
std::ostream& err() { return cerr; }
#ifdef DEBUG_OUTPUT
DebugStreamFlusher debug() { return DebugStreamFlusher(dout); }
#else
DebugStreamFlusher debug() { return DebugStreamFlusher(); }
#endif

void out(const string& s) { out() << s; }
void err(const string& s) { err() << s; }

#ifdef DEBUG_OUTPUT
void debug(const string& s) { debug() << s; }
#else
void debug(const string&) {}
#endif

