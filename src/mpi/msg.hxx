#ifndef MPI_MSG_HXX
#define MPI_MSG_HXX

namespace game_of_life
{

enum class MsgType : int
{
    NO_MSG,
    UPDATE_STOPPER,
    STOP,
    SHUTDOWN,
    UPDATE_STATUS
};

};

#endif

