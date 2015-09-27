#include "threaded-worker.hxx"
#include "threaded-manager.hxx"
#include "output.hxx"

using namespace std;

namespace game_of_life
{
ThreadedWorkerShared::ThreadedWorkerShared()
{
    iterationCalced[0] = iterationCalced[1] = 0;
    iterationPublished[0] = iterationPublished[1] = 0;
}

int ThreadedWorkerShared::getIterationCalced(bool south) const
{
    return iterationCalced[south];
}

int ThreadedWorkerShared::getIterationPublished(bool south) const
{
    return iterationPublished[south];
}

int ThreadedWorkerShared::getIterationCompleted() const
{
    return max(getIterationPublished(0), getIterationPublished(1));
}

void ThreadedWorkerShared::wake()
{
    sem.post();
}

void ThreadedWorkerShared::incIterationCalced(bool south)
{
    ++iterationCalced[south];
}

void ThreadedWorkerShared::incIterationPublished(bool south)
{
    ++iterationPublished[south];
}

void ThreadedWorkerShared::sleep()
{
    sem.wait();
}


void ThreadedWorker::start(
        ThreadedManagerShared& manager,
        AbstractTile& domain,
        const std::vector<ThreadedWorkerShared*>& neighShared)
{
    this->manager = &manager;
    this->neighShared = neighShared;
    this->domain = &domain;

    Thread::start();
}

ThreadedWorkerShared& ThreadedWorker::getShared() { return myShared; }

void ThreadedWorker::run()
{
    debug("worker started");

    coord_t h = domain->getHeight();
    coord_t w = domain->getWidth();

    TileView innerResult = domain->getInner();
    Borders resultBorders = domain->getBorders();

    Matrix tempDomain(h, w);
    TileView innerTemp = tempDomain.getInner();
    Borders tempBorders = tempDomain.getBorders();

    debug() << "worker ready to calc";
    while (manager->wakeWhenNextIterationNeeded(myShared.getIterationCompleted()))
    {
        int myIter = myShared.getIterationCompleted();

        makeIteration(innerResult, innerTemp);
        makeIteration(resultBorders[SIDE_W], tempBorders[SIDE_W]);
        makeIteration(resultBorders[SIDE_E], tempBorders[SIDE_E]);

        debug() << "calced inners for iter " << myIter << "->" << myIter+1
            << ": " << innerResult.getWindow();

        bool has[2] = {false, false};
        static const int neighSides[2] = {SIDE_N, SIDE_S};
        for (bool first = true; !has[0] || !has[1]; first = false)
        {
            if (!first)
                myShared.sleep();
            for (int i = 0; i < 2; ++i)
                if (!has[i] && neighShared[i]->getIterationPublished(1-i) == myIter
                    && (h > 1 || neighShared[1-i]->getIterationPublished(i) == myIter))
                {
                    has[i] = true;
                    makeIteration(
                            resultBorders[neighSides[i]],
                            tempBorders[neighSides[i]]);
                    debug() << "calced side " << i
                        << " for iter " << myIter << "->" << myIter+1
                        << ": " << resultBorders[neighSides[i]].getWindow();
                    myShared.incIterationCalced(i);
                    neighShared[i]->wake();
                }
        }

        innerResult.copyValues(innerTemp);
        resultBorders[SIDE_W].copyValues(tempBorders[SIDE_W]);
        resultBorders[SIDE_E].copyValues(tempBorders[SIDE_E]);

        debug() << "published inners " << myIter << "->" << myIter+1;

        bool copied[2] = {false, false};
        for (bool first = true; !copied[0] || !copied[1]; first = false)
        {
            if (!first)
                myShared.sleep();
            for (int i = 0; i < 2; ++i)
                if (!copied[i] && neighShared[i]->getIterationCalced(1-i) == myIter+1
                    && (h > 1 || neighShared[1-i]->getIterationCalced(i) == myIter+1))
                {
                    copied[i] = true;
                    resultBorders[neighSides[i]]
                        .copyValues(tempBorders[neighSides[i]]);
                    debug() << "published side " << i << myIter << "->" << myIter+1;
                    myShared.incIterationPublished(i);
                    neighShared[i]->wake();
                }
        }
    }

    debug("worker stopped");
}


} // namespace game_of_life

