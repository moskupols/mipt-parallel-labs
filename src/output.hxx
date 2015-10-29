#ifndef OUTPUT_HXX_INCLUDED
#define OUTPUT_HXX_INCLUDED

#include <iostream>
#include <string>

#ifdef DEBUG_OUTPUT
class DebugStreamFlusher
{
public:
    explicit DebugStreamFlusher(std::ostream& out);
    DebugStreamFlusher(DebugStreamFlusher&&);
    ~DebugStreamFlusher();

    std::ostream& get();

    template<class T>
    DebugStreamFlusher& operator << (const T& v)
    {
        if (out)
            *out << v;
        return *this;
    }

    template<class T>
    DebugStreamFlusher& operator << (T& v)
    {
        if (out)
            *out << v;
        return *this;
    }

private:
    std::ostream* out;
};
#else
class DebugStreamFlusher
{
public:
    template<class T>
    DebugStreamFlusher& operator << (const T&) { return *this; }
    template<class T>
    DebugStreamFlusher& operator << (T&) { return *this; }

    std::ostream& get();
};
#endif

std::ostream& out();
std::ostream& err();
DebugStreamFlusher debug();

void out(const std::string&);
void err(const std::string&);
void debug(const std::string&);

#endif

