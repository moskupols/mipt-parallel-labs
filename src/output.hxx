#ifndef OUTPUT_HXX_INCLUDED
#define OUTPUT_HXX_INCLUDED

#include <iostream>
#include "thread.hxx"

extern ResourceMutex<std::ostream> coutMutex;
extern ResourceMutex<std::ostream> cerrMutex;

void out(ResourceMutex<std::ostream>& outRes, const std::string& msg);

void out(const std::string& msg);
void outLn(const std::string& msg);

void err(const std::string& msg);
void errLn(const std::string& msg);

#endif

