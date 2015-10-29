#include "mpi-manager.hxx"

#include "../tiles/abstract-tile.hxx"

namespace game_of_life
{

MpiManager::MpiManager()
{}

void MpiManager::start(AbstractTile& tile, mpi::MpiCommunicator comm)
{
}

void MpiManager::runForMore(int runs)
{
}

void MpiManager::pauseAll()
{
}

void MpiManager::shutdown()
{
}

int MpiManager::getStop() const
{
    return -1;
}

void MpiManager::run()
{}

};

