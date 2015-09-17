#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <memory>
#include <cstdlib>

#include "output.hxx"
#include "threaded-manager.hxx"
#include "tile.hxx"
#include "exceptions.hxx"

using namespace game_of_life;

using std::vector;
using std::string;

using std::domain_error;

using std::map;

using std::cin;
using std::endl;
using std::istringstream;

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
        // if (p.size() == 3)
        // {
            int h = toInt(p[1]), w = toInt(p[2]);
            matrix = Matrix(h, w);
            matrix.set(0, 0, true);
            matrix.set(1, 1, true);
        // }
        // else
        // {

        // }
        manager.start(matrix, concurrency);
    }
    catch (domain_error& e)
    {
        throw IncorrectCommandException(TAG + e.what());
    }
}

void status(Params p)
{
    static const string TAG("STATUS: ");
    checkParamCount(TAG, p, 0, 0);

    Manager::State state = manager.getState();

    OstreamLocker lock(coutMutex);
    lock.get() << "Status is " << state << endl;
    if (state == Manager::NOT_STARTED)
        return;

    matrix.output(lock.get());
}

void run(Params p)
{
    static const string TAG("RUN: ");
    checkParamCount(TAG, p, 1, 1);

    int runs;
    switch (manager.getState())
    {
    case Manager::NOT_STARTED:
        throw IncorrectCommandException(
                TAG + "task unknown, use START to initialize.");
    case Manager::STOPPED:
        runs = toInt(p[0]);
        debugLn(TAG + "trying to add " + p[0] + "iterations");
        manager.runForMore(runs);
        break;
    default:
        throw IncorrectCommandException(TAG + "system is busy");
    }
}

void stop(Params p)
{
    static const string TAG("STOP: ");
    checkParamCount(TAG, p, 0, 0);

    switch (manager.getState())
    {
    case Manager::RUNNING:
        debugLn(TAG + "stopping");
        manager.pauseAll();
        manager.wakeWhenStateIs(Manager::STOPPED);
        debugLn(TAG + "awake and stopped");
        break;
    default:
        throw IncorrectCommandException(TAG + "not running");
    }
}

void quit(Params p)
{
    static const string TAG("QUIT: ");
    checkParamCount(TAG, p, 0, 0);

    debugLn(TAG);
    if (manager.getState() != Manager::NOT_STARTED)
    {
        debugLn(TAG + "manager has started, trying to shut him");
        manager.shutdown();
        manager.wakeWhenStateIs(Manager::FINISHED);
        debugLn(TAG + "awake and shut");
    }
    exit(0);
}

void unknownCommand(const string& s)
{
    throw IncorrectCommandException(string("\"") + s + "\" is not supported ._.");
}

int main()
{
    map<string, CommandHandler> cmdMap;
    cmdMap["START"] = start;
    cmdMap["STATUS"] = status;
    cmdMap["RUN"] = run;
    cmdMap["STOP"] = stop;
    cmdMap["QUIT"] = quit;

    string line;
    debugLn("---------------- RESTART ----------------------");
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
    outLn("quit");
    quit(vector<string>());
}

