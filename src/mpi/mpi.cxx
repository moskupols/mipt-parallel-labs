#include "mpi.hxx"

#include "../exceptions.hxx"
#include "../output.hxx"

namespace mpi
{

namespace impl
{
MpiCommunicator makeCommunicator(MPI_Comm c)
{
    return MpiCommunicator(c);
}

void throwOnFail(int resultCode)
{
    if (resultCode != MPI_SUCCESS)
        throw CLevelException("smth in mpi");
}

#define MAKE_MATCHER(type, datatype) \
template<> const MPI_Datatype DatatypeMatcher<type>::value = datatype;

MAKE_MATCHER(bool,               MPI_CHAR)
MAKE_MATCHER(char,        MPI_CHAR)
MAKE_MATCHER(signed char,        MPI_CHAR)
MAKE_MATCHER(signed short int,   MPI_SHORT)
MAKE_MATCHER(signed int,         MPI_INT)
MAKE_MATCHER(unsigned char,      MPI_UNSIGNED_CHAR)
MAKE_MATCHER(unsigned short int, MPI_UNSIGNED_SHORT)
MAKE_MATCHER(unsigned int,       MPI_UNSIGNED)
MAKE_MATCHER(unsigned long int,  MPI_UNSIGNED_LONG)
MAKE_MATCHER(float,              MPI_FLOAT)
MAKE_MATCHER(double,             MPI_DOUBLE)
MAKE_MATCHER(long double,        MPI_LONG_DOUBLE)

#undef MAKE_MATCHER
}


MpiRequest::MpiRequest():
    r(MPI_REQUEST_NULL)
{}

MpiRequest::MpiRequest(MPI_Request r):
    r(r)
{}

bool MpiRequest::test()
{
    int ret;
    impl::throwOnFail(MPI_Test(&r, &ret, MPI_STATUS_IGNORE));
    return ret;
}

void MpiRequest::wait()
{
    impl::throwOnFail(MPI_Wait(&r, MPI_STATUS_IGNORE));
}

bool MpiRequest::isPending()
{
    return r != MPI_REQUEST_NULL;
}

int MpiRequest::waitSome(int inCount, MpiRequest* in, int* out)
{
    int ret;
    MPI_Request reqs[inCount];
    for (int i = 0; i < inCount; ++i)
        reqs[i] = in[i].r;
    impl::throwOnFail(
            MPI_Waitsome(inCount, reqs, &ret, out, MPI_STATUSES_IGNORE));
    for (int i = 0; i < ret; ++i)
        in[out[i]].r = reqs[out[i]];
    return ret;
}


MpiCommunicator::MpiCommunicator()
{
}

int MpiCommunicator::getSize() const
{
    int ret;
    impl::throwOnFail(MPI_Comm_size(comm, &ret));
    return ret;
}

int MpiCommunicator::getRank() const
{
    int ret;
    impl::throwOnFail(MPI_Comm_rank(comm, &ret));
    return ret;
}

void MpiCommunicator::abort(int error)
{
    impl::throwOnFail(MPI_Abort(comm, error));
}

void MpiCommunicator::barrier()
{
    impl::throwOnFail(MPI_Barrier(comm));
}

MpiCommunicator MpiCommunicator::split(int color)
{
    MPI_Comm ret;
    impl::throwOnFail(
            MPI_Comm_split(comm, color, getRank(), &ret));
    return MpiCommunicator(ret);
}

MpiCommunicator& MpiCommunicator::operator=(const MpiCommunicator& that)
{
    comm = that.comm;
    return *this;
}

MpiCommunicator::MpiCommunicator(MPI_Comm comm):
    comm(comm)
{}

Mpi::Mpi()
{
    assert(!instantiated);
    instantiated = true;
}

Mpi::~Mpi()
{
    if (initialized)
    {
        debug("finalized MPI, anonymous debug after this line");
        finalized = true;
        MPI_Finalize();
    }
}

void Mpi::init(int argc, char** argv)
{
    assert(!initialized);
    initialized = true;

    MPI_Init(&argc, &argv);
}

MpiCommunicator Mpi::getWorldComm()
{
    assert(initialized);
    return impl::makeCommunicator(impl::COMM_WORLD);
}

bool Mpi::isFinalized()
{
    return finalized;
}

bool Mpi::instantiated = false;
bool Mpi::initialized = false;
bool Mpi::finalized = false;

Mpi mpi;

}

