#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <mpi.h>

#include <random>
#include <thread> //sleep_for
#include <chrono> //milliseconds

#include "Settings.hpp"

#define USE_ALLEGRO_GRAPHICS
#define ALIVE   1
#define DEAD    0

using namespace std;



Settings settings;






int parallelLifeEngine(int &numberOfCpus)
{
    // USING 1D TOPOLOGY

    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);


    int rows;
    int columns;

    string matrixSize = string(to_string(settings.getMatrixSize()) + " x " + to_string(settings.getMatrixSize()));
    if (rows % numberOfCpus != 0)
        throw runtime_error(string("Can't divide ") + matrixSize + (" matrix equally on ") + to_string(numberOfCpus) + string(" proccesses."));

    int dimensions[2] = {1, numberOfCpus};

    int toroidality[2] = {0, 1};

    int reorder = 1;

    MPI_Comm comunicator;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, toroidality, reorder, &comunicator);

    int up, down;
    MPI_Cart_shift(comunicator, 1, 1, &up, &down);


    MPI_Datatype contiguous;
    MPI_Type_contiguous(columns, MPI_INT, &contiguous);
    MPI_Type_commit(&contiguous);





    return 0;
}