#ifndef MPI_HXX_INCLUDED
#define MPI_HXX_INCLUDED

#include "../utils.hxx"

#include <mpi.h>

namespace mpi
{

typedef MPI_Status MpiStatus;

class MpiCommunicator;

namespace impl
{
static const MPI_Comm COMM_WORLD = MPI_COMM_WORLD;
MpiCommunicator makeCommunicator(MPI_Comm);
void throwOnFail(int resultCode);

template<typename T>
struct DatatypeMatcher
{
static const MPI_Datatype value;
};

}

class MpiCommunicator
{
public:
    int getSize() const;
    int getRank() const;

    void abort(int error=0);

    template<typename T>
    void send(T* start, int count, int receiver, int tag)
    {
        throwOnFail(
                MPI_Send(
                    start, count, impl::DatatypeMatcher<T>::value,
                    receiver, tag,
                    comm));
    }

    template<typename T>
    MpiStatus receive(T* start, int count, int sender, int tag)
    {
        MpiStatus ret;
        throwOnFail(
                MPI_Recv(
                    start, count, impl::DatatypeMatcher<T>::value,
                    sender, tag, comm,
                    &ret));
        return ret;
    }

    MpiCommunicator& operator=(const MpiCommunicator&that);

protected:
    friend MpiCommunicator impl::makeCommunicator(MPI_Comm);

    explicit MpiCommunicator(MPI_Comm comm);

private:
    MPI_Comm comm;
};

class Mpi : Noncopyable
{
public:
    Mpi();

    static void init(int argc, char** argv);

    static MpiCommunicator getWorldComm();

private:
    ~Mpi();
    static bool instantiated;
    static bool initialized;
};

extern Mpi mpi;

}

#endif

