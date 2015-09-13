#include "threaded-worker.hxx"
#include "threaded-manager.hxx"

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
{ incAndWakeAll(iterationCalced, calcedMutex, calcedCond); }

void ThreadedWorkerShared::incIterationPublished()
{ incAndWakeAll(iterationPublished, publishMutex, publishCond); }



void ThreadedWorker::start(
        ThreadedManager* manager,
        AbstractTile* domain,
        const std::vector<ThreadedWorkerShared*>& neighShared)
{
    this->manager = manager;
    this->neighShared = neighShared;
    this->domain = domain;

    Thread::start();
}

ThreadedWorkerShared* ThreadedWorker::getShared() { return &myShared; }

void ThreadedWorker::run()
{
    coord_t h = domain->getHeight();
    coord_t w = domain->getWidth();

    CoordRect innerRect(1, 1, h - 1, w - 1);

    TileView innerResult(domain, innerRect);
    Borders resultBorders = TileView(domain).makeBorders();

    Matrix tempDomain(h, w);
    TileView innerTemp(&tempDomain, innerRect);
    Borders tempBorders = TileView(&tempDomain).makeBorders();

    while (manager->wakeWhenNextIterationNeeded(myShared.iterationPublished))
    {
        makeIteration(&innerResult, &innerTemp);

        for (size_t i = 0; i < SIDE_COUNT; ++i)
            neighShared[i]->wakeWhenPublishes(myShared.iterationPublished);

        for (size_t i = 0; i < SIDE_COUNT; ++i)
            makeIteration(resultBorders[i], tempBorders[i]);

        myShared.incIterationCalced();
        for (size_t i = 0; i < SIDE_COUNT; ++i)
            neighShared[i]->wakeWhenCalcs(myShared.iterationCalced);

        domain->assign(&tempDomain);
        myShared.incIterationPublished();
    }

    for (size_t i = 0; i < SIDE_COUNT; ++i)
    {
        delete resultBorders[i];
        delete tempBorders[i];
    }
}


} // namespace game_of_life

