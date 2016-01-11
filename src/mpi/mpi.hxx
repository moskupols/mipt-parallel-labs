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

class MpiRequest
{
public:
    explicit MpiRequest(MPI_Request r);

    bool test();
    void wait();

private:
    MPI_Request r;
};

class MpiCommunicator
{
public:
    MpiCommunicator();

    int getSize() const;
    int getRank() const;

    void abort(int error=0);

    MpiCommunicator split(int color);

    template<typename T>
    void allreduce(void* sendbuf, T* recvbuf, int count, MPI_Op op)
    {
        impl::throwOnFail(
                MPI_Allreduce(
                    sendbuf, recvbuf, count,
                    impl::DatatypeMatcher<T>::value,
                    op, comm));
    }

    template<typename T>
    void broadcast(T* start, int count, int root)
    {
        impl::throwOnFail(
                MPI_Bcast(
                    start, count, impl::DatatypeMatcher<T>::value,
                    root, comm));
    }
    template<typename T>
    MpiRequest asyncBroadcast(T* start, int count, int root)
    {
        MPI_Request result;
        impl::throwOnFail(
                MPI_Ibcast(
                    start, count, impl::DatatypeMatcher<T>::value,
                    root, comm, &result));
        return MpiRequest(result);
    }

    template<typename T>
    void send(T* start, int count, int receiver, int tag)
    {
        impl::throwOnFail(
                MPI_Send(
                    start, count, impl::DatatypeMatcher<T>::value,
                    receiver, tag, comm));
    }

    template<typename T>
    MpiRequest asyncSend(T* start, int count, int receiver, int tag)
    {
        MPI_Request result;
        impl::throwOnFail(
                MPI_Isend(
                    start, count, impl::DatatypeMatcher<T>::value,
                    receiver, tag, comm, &result));
        return MpiRequest(result);
    }

    template<typename T>
    MpiStatus receive(T* start, int count, int sender, int tag)
    {
        MpiStatus ret;
        impl::throwOnFail(
                MPI_Recv(
                    start, count, impl::DatatypeMatcher<T>::value,
                    sender, tag, comm, &ret));
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

