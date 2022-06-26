#include <mpi.h>

#include <random>
#include <thread> //sleep_for
#include <chrono> //milliseconds


#include "headers/serialLifeEngine.cpp"
#include "headers/parallelLifeEngine.cpp"

int main(int args, char **argv)
{

    MPI_Init(&args, &argv);


    int numberOfCpus;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfCpus);


    int errorCode;


    if (numberOfCpus == 1)
    {
        errorCode = serialLifeEngine();

        MPI_Finalize();
        return errorCode;
    }
    else if (numberOfCpus >= 2)
    {

        errorCode = parallelLifeEngine();

        MPI_Finalize();
        return errorCode;
    }

}