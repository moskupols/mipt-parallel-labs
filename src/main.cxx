#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <memory>

#include "output.hxx"
#include "threaded-manager.hxx"
#include "tile.hxx"
#include "exceptions.hxx"

using namespace game_of_life;
using namespace std;

vector<string> split(const string& s)
{
    vector<string> ret;
    istringstream iss(s);
    string w;
    while (iss >> w)
        if (!w.empty())
            ret.push_back(w);
    return ret;
}

int toInt(const string &s)
{
    int ret = 0;
    if (s.size() > 8)
        throw domain_error(string("\"") + s + "\" is too long for int");
    for (size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c >= '0' && c <= '9')
            ret = ret * 10 + c - '0';
        else
            throw domain_error(string("\"") + s + "\" is not an integer");
    }
    return ret;
}

ThreadedManager manager;
Matrix matrix;

typedef const vector<string>& Params;
typedef void (*CommandHandler)(Params);

void checkParamCount(const string& tag, Params p, size_t lo, size_t hi)
{
    if (p.size() > hi)
        throw IncorrectCommandException(tag + "too much parameters");
    if (p.size() < lo)
        throw IncorrectCommandException(tag + "too few parameters");
}

void start(Params p)
{
    static const string TAG("START: ");

    if (manager.getState() != ThreadedManager::NOT_STARTED)
        throw IncorrectCommandException(TAG + "already started");
    checkParamCount(TAG, p, 2, 3);

    try
    {
        int concurrency = toInt(p[0]);
        if (p.size() == 3)
        {
            int h = toInt(p[1]), w = toInt(p[2]);
            matrix = Matrix(h, w);
            matrix.set(0, 0, true);
            matrix.set(1, 1, true);
        }
        else
        {

        }
        // manager.start(&matrix, concurrency);
    }
    catch (exception& e)
    {
        throw IncorrectCommandException(TAG + e.what());
    }
}

void status(Params p)
{}

void run(Params p)
{}

void stop(Params p)
{}

void quit(Params p)
{}

void unknownCommand(const string& s)
{}

int main()
{
    map<string, CommandHandler> cmdMap;
    cmdMap["START"] = start;
    cmdMap["STATUS"] = status;
    cmdMap["RUN"] = run;
    cmdMap["STOP"] = stop;
    cmdMap["QUIT"] = quit;

    string line;
    while (out("game-of-life: "), getline(cin, line))
    {
        vector<string> words = split(line);

        if (words.empty())
            continue;

        string cmd = words[0];
        for (size_t i = 0; i < cmd.size(); ++i)
            cmd[i] = toupper(cmd[i]);

        words.erase(words.begin());

        map<string, CommandHandler>::iterator it = cmdMap.find(cmd);
        try
        {
            if (it != cmdMap.end())
                it->second(words);
            else
                unknownCommand(cmd);
        }
        catch (IncorrectCommandException& e)
        {
            outLn(e.what());
        }
    }
}

