#include "threaded-worker.hxx"
#include "threaded-manager.hxx"
#include "output.hxx"

namespace game_of_life
{

namespace
{
void wakeWhenReaches(int& watched, int desired, Mutex& m, Cond& c)
{
    MutexLocker locker(m);
    while (watched < desired)
        c.wait(m);
}
void incAndWakeAll(int& x, Mutex& m, Cond& c)
{
    MutexLocker locker(m);
    ++x;
    c.wakeAll();
}
} // namespace anonymous



ThreadedWorkerShared::ThreadedWorkerShared():
    iterationCalced(0),
    iterationPublished(0)
{ }

int ThreadedWorkerShared::getIterationCalced() const { return iterationCalced; }

int ThreadedWorkerShared::getIterationPublished() const { return iterationPublished; }

void ThreadedWorkerShared::wakeWhenCalcs(int needed)
{ wakeWhenReaches(iterationCalced, needed, calcedMutex, calcedCond); }

void ThreadedWorkerShared::wakeWhenPublishes(int needed)
{ wakeWhenReaches(iterationPublished, needed, publishMutex, publishCond); }

void ThreadedWorkerShared::incIterationCalced()
{
    // debug() << "worker finished calcing iteration" << getIterationCalced() + 1;
    incAndWakeAll(iterationCalced, calcedMutex, calcedCond);
}

void ThreadedWorkerShared::incIterationPublished()
{
    // debug() << "worker publishing iteration " << getIterationPublished() + 1;
    incAndWakeAll(iterationPublished, publishMutex, publishCond);
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

    size_t nsz = neighShared.size();

    debug("worker ready to calc");
    while (manager->wakeWhenNextIterationNeeded(myShared.iterationPublished))
    {
        makeIteration(innerResult, innerTemp);

        for (size_t i = 0; i < nsz; ++i)
            neighShared[i]->wakeWhenPublishes(myShared.iterationPublished);

        for (size_t i = 0; i < SIDE_COUNT; ++i)
            makeIteration(resultBorders[i], tempBorders[i]);

        myShared.incIterationCalced();
        for (size_t i = 0; i < nsz; ++i)
            neighShared[i]->wakeWhenCalcs(myShared.iterationCalced);

        domain->copyValues(tempDomain);
        myShared.incIterationPublished();
    }

    debug("worker stopped");
}


} // namespace game_of_life

