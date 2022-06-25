#include <mpi.h>

#include "headers/serialLifeEngine.cpp"
#include "headers/parallelLifeEngine.cpp"

int main(int args, char **argv)
{
    MPI_Init(&args, &argv);

    int numberOfCpus;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfCpus);

    if (numberOfCpus == 1)
    {
        int errorCode;
        errorCode = serialLifeEngine();

        MPI_Finalize();
        return errorCode;
    }
    else if (numberOfCpus == 2 || numberOfCpus % 2 == 1)
    {
        parallelLifeEngine(numberOfCpus);
    }




    return 0;
}