#include "output.hxx"
#include "thread.hxx"

using namespace std;

ResourceMutex<ostream> coutMutex(cout);
ResourceMutex<ostream> cerrMutex(cerr);

void out(ResourceMutex<ostream>& outRes, const string& msg)
{
    ResourceLocker<ostream> locker(outRes);
    locker.get() << msg;
}

void out(const string& msg) { out(coutMutex, msg); }
void outLn(const string& msg) { out(coutMutex, msg + "\n"); }

void err(const string& msg) { out(cerrMutex, msg); }
void errLn(const string& msg) { out(cerrMutex, msg + "\n"); }

