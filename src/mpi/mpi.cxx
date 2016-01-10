#include "mpi.hxx"

#include "../exceptions.hxx"

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
    MPI_Finalize();
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

bool Mpi::instantiated = false;
bool Mpi::initialized = false;

}

