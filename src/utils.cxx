#include "utils.hxx"

#include <sstream>

#include <errno.h>
#include <string.h>

void throwCError(int line, const char* file)
{
    std::ostringstream oss;
    oss << file << ':' << line << "\t" << strerror(errno);
}

