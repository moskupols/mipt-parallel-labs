#ifndef MANAGER_HXX_INCLUDED
#define MANAGER_HXX_INCLUDED

#include <vector>

namespace game_of_life
{

class CoordRect;
class AbstractTile;
class TorusView;

class Manager
{
public:
    static std::vector<CoordRect> chooseDomains(AbstractTile* t, int parts);
    static std::vector<std::vector<int> > makeNeighbors(
            TorusView* t, const std::vector<CoordRect>& r);
};

}

#endif

