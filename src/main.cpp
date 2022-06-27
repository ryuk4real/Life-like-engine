#include <mpi.h>

#define PARALLEL
//#define SERIAL

// TODO: change parallel and serial to main

#define USE_ALLEGRO_GRAPHICS

#if defined(PARALLEL)

#include "headers/parallelLifeEngine.cpp"

int main(int args, char **argv)
{

    MPI_Init(&args, &argv);


    int numberOfCpus;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfCpus);


    int errorCode;

    errorCode = parallelLifeEngine();

    MPI_Finalize();
    return errorCode;
}

#endif


#ifdef SERIAL

#include "headers/serialLifeEngine.cpp"
int main(int args, char **argv)
{

    MPI_Init(&args, &argv);


    int numberOfCpus;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfCpus);

    int errorCode;

    errorCode = serialLifeEngine();

    MPI_Finalize();
    return errorCode;
}

#endif