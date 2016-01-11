#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <memory>

#include <cstdlib>

#include "output.hxx"
#include "tile.hxx"
#include "exceptions.hxx"

#include "mpi/mpi.hxx"
#include "mpi/mpi-manager.hxx"
#include "mpi/mpi-worker.hxx"

using namespace game_of_life;
using namespace mpi;

using std::vector;
using std::string;

using std::exception;
using std::domain_error;
using std::bad_alloc;

using std::map;

using std::cin;
using std::istringstream;
using std::ifstream;

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

MpiManager manager;
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

    if (manager.getState() != Manager::NOT_STARTED)
        throw IncorrectCommandException(TAG + "already started");
    checkParamCount(TAG, p, 2, 3);

    int concurrency;
    try
    {
        concurrency = toInt(p[0]);
        if (concurrency <= 0)
            throw IncorrectCommandException(TAG + "incorrect concurrency");
        if (p.size() == 3)
        {
            int h = toInt(p[1]), w = toInt(p[2]);
            if (h <= 0 || w <= 0)
                throw IncorrectCommandException(TAG + "incorrect matrix size");
            matrix = Matrix::random(h, w, 42);
        }
        else
        {
            try
            {
                ifstream csv(p[1].data());
                matrix = Matrix::fromCsv(csv);
            }
            catch (exception& e)
            {
                throw IncorrectCommandException(TAG + e.what());
            }
        }
    }
    catch (domain_error& e)
    {
        throw IncorrectCommandException(TAG + e.what());
    }
    catch (bad_alloc& e)
    {
        throw IncorrectCommandException(TAG + "couldn't allocate enough memory");
    }
    debug(TAG + "starting the manager");
    manager.start(matrix, Mpi::getWorldComm(), concurrency);
}

void status(Params p)
{
    static const string TAG("STATUS: ");
    checkParamCount(TAG, p, 0, 0);

    manager.updateStatus();
    Manager::State state = manager.getState();

    std::ostream& o(out());
    o << "System state: " << manager.stateStr(state) << "\n";
    if (state != Manager::NOT_STARTED && state != Manager::RUNNING)
    {
        o << "After iteration " << manager.getStop() << ":\n";
        if (matrix.getWidth() * matrix.getHeight() > 1000)
            o << "<matrix is too large to print it>\n";
        else
            matrix.output(o);
    }
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
        try
        {
            runs = toInt(p[0]);
        }
        catch (domain_error& e)
        {
            throw IncorrectCommandException(TAG + e.what());
        }
        debug(TAG + "trying to add " + p[0] + " iterations");
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

    manager.updateStatus();
    switch (manager.getState())
    {
    case Manager::RUNNING:
        debug(TAG + "stopping");
        manager.pauseAll();
        manager.wakeWhenStateIs(Manager::STOPPED);
        debug(TAG + "awake and stopped");
        out() << "Stopped at " << manager.getStop() << "\n";
        break;
    default:
        throw IncorrectCommandException(TAG + "not running");
    }
}

void quit(Params p)
{
    static const string TAG("QUIT: ");
    checkParamCount(TAG, p, 0, 0);

    debug(TAG);
    if (manager.getState() != Manager::NOT_STARTED)
    {
        debug(TAG + "manager has started, trying to shut him");
        manager.shutdown();
        // manager.join();
        debug(TAG + "joined the manager");
    }
    debug(TAG + "exiting the program gracefully ----------");
    exit(0);
}

void block(Params p)
{
    static const string TAG("BLOCK: ");
    checkParamCount(TAG, p, 0, 0);

    debug(TAG);
    if (manager.getState() == Manager::NOT_STARTED)
        throw IncorrectCommandException(TAG + "not running");
    manager.wakeWhenStateIs(Manager::STOPPED);
    debug(TAG + "awake and resuming");
}

void unknownCommand(const string& s)
{
    throw IncorrectCommandException(string("\"") + s + "\" is not supported ._.");
}

int main(int argc, char** argv)
{
    Mpi::init(argc, argv);

    MpiCommunicator globalComm = Mpi::getWorldComm();
    if (globalComm.getRank())
    {
        MpiWorker worker{};
        worker.run(globalComm);
        return 0;
    }

    map<string, CommandHandler> cmdMap;
    cmdMap["START"] = start;
    cmdMap["STATUS"] = status;
    cmdMap["RUN"] = run;
    cmdMap["STOP"] = stop;
    cmdMap["QUIT"] = quit;
    cmdMap["BLOCK"] = block;

    string line;
    debug("---------------- RESTART ----------------------");
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
            err() << e.what() << "\n";
        }
    }
    out("quit\n");
    quit(vector<string>());
}

