#include <mpi.h>

//#define USE_ALLEGRO_GRAPHICS

#define PARALLEL
//#define SERIAL

#if defined (PARALLEL)
#include "headers/parallelLifeEngine.hpp"
#endif

#if defined (SERIAL)
#include "headers/serialLifeEngine.hpp"
#endif



int main(int args, char **argv)
{
    MPI_Init(&args, &argv);

    int errorCode;

    #if defined (SERIAL)
    errorCode = serialLifeEngine();
    #endif

    #if defined (PARALLEL)
    errorCode = parallelLifeEngine();
    #endif


    MPI_Finalize();
    return errorCode;
}