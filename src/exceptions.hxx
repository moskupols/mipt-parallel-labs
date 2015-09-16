#ifndef EXCEPTIONS_HXX_INCLUDED
#define EXCEPTIONS_HXX_INCLUDED

#include <stdexcept>
#include <string>

class IncorrectCommandException : public std::runtime_error
{
public:
    explicit IncorrectCommandException(const std::string& what):
        std::runtime_error(what)
    {}
};

#endif

