#ifndef PARALLEL_LIFE_ENGINE
#define PARALLEL_LIFE_ENGINE


#include "utils.cpp"

MPI_Request request;
MPI_Status status;


using namespace std;

int processId;
int numberOfCpus;

int rows;
int columns;
int subRows;


inline void sendBorders(int * subCurrentGeneration, MPI_Datatype contiguous, int &up, int &down)
{
    MPI_Isend(subCurrentGeneration + settings.getMatrixSize(),
              1,
              contiguous,
              up,
              88,
              MPI_COMM_WORLD,
              &request);

    MPI_Isend(subCurrentGeneration + settings.getMatrixSize() * (settings.getMatrixSize() / numberOfCpus),
              1,
              contiguous,
              down,
              99,
              MPI_COMM_WORLD,
              &request);
}

inline void reciveBorders(int * subCurrentGeneration, MPI_Datatype contiguous, int &up, int &down)
{
    MPI_Recv(subCurrentGeneration,
             1,
             contiguous,
             up,
             99,
             MPI_COMM_WORLD,
             &status);

    MPI_Recv(subCurrentGeneration + settings.getMatrixSize() * (settings.getMatrixSize() / numberOfCpus + 1),
             1,
             contiguous,
             down,
             88,
             MPI_COMM_WORLD,
             &status);
}

inline void parallelTransictionCell(int i, int j, int * subCurrentGeneration, int * subNewGeneration)
{
    int neighbours = 0;

    for (int r = -1; r <= 1; ++r)
    {
        for (int c = -1; c <= 1; ++c)
        {

            if ((r != 0 || c != 0))
            {
                if (subCurrentGeneration[m((i + r + rows) % rows, ((j + c + columns) % columns))] == ALIVE)
                {
                    ++neighbours;
                }
            }
        }

        if (subCurrentGeneration[m(i, j)] == ALIVE)
        {
            if (settings.getStringRule().getNeighbourFromBornSubsetAt(neighbours))
            {
                subNewGeneration[m(i, j)] = ALIVE;
            }
            else
            {
                subNewGeneration[m(i, j)] = DEAD;
            }
        }
        else
        {
            if (settings.getStringRule().getNeighbourFromSurvivorSubsetAt(neighbours))
            {
                subNewGeneration[m(i, j)] = ALIVE;
            }
            else
            {
                subNewGeneration[m(i, j)] = DEAD;
            }
        }
    }
}




void calculateNextSubInnerGeneration(int * subCurrentGeneration, int * subNewGeneration)
{
    for (int i = 1; i < subRows - 2; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            parallelTransictionCell(i,j, subCurrentGeneration, subNewGeneration);
        }
    }
}

inline void calculateNexSubGenerationBorders(int * subCurrentGeneration, int * subNewGeneration)
{
    for (int j = 0; j < columns; ++j)
    {
        parallelTransictionCell(1, j, subCurrentGeneration, subNewGeneration);
        parallelTransictionCell(subRows-2, j, subCurrentGeneration, subNewGeneration);
    }
}


inline void swapSubGeneration(int * subCurrentGeneration, int * subNewGeneration)
{
    int *subCurrentGenerationCopy = subCurrentGeneration;
    subCurrentGeneration = subNewGeneration;
    subNewGeneration = subCurrentGenerationCopy;
}


inline void displayCurrentGeneration(int * generationAfterGather, ALLEGRO_COLOR aliveCellColor)
{
    for (int i = 0; i < settings.getMatrixSize(); ++i)
    {
        for (int j = 0; j < settings.getMatrixSize(); ++j)
        {
            if (generationAfterGather[m(j,i)] == 1)
                drawCell(i, j, aliveCellColor);
        }
    }
}

void printm(int * mat, int proccessId, int _subRows)
{
    printf("Process %d\n", proccessId);

    for (int i = 0; i < _subRows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            printf("%d", mat[m(i,j)]);
        }

        printf("\n");
    }

    printf("\n\n");
}



int parallelLifeEngine()
{
    // USING 1D TOPOLOGY

    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfCpus);

    rows = settings.getMatrixSize();
    subRows = ((settings.getMatrixSize() / numberOfCpus) + 2);
    columns = settings.getMatrixSize();

    int * generationAfterGather = new int[rows * columns];

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


    int * subCurrentGeneration = new int[subRows * columns];
    int * subNewGeneration = new int[subRows * columns];


    if (settings.isFirstGenerationRandomized())
        initializeRandomizedFirstGeneration(subRows, columns, subCurrentGeneration, (unsigned int) processId);
    else
        initializeFirstGenerationForTesting(subRows, columns, subCurrentGeneration, processId);

    if (processId == 0)
    {
        for (int i = 0; i < 3; ++i) subCurrentGeneration[m(0,i)] = 1;
    }
    

    if (processId == 0)
    {
        #ifdef USE_ALLEGRO_GRAPHICS
        // ALLEGRO -------------------------------------------------------
        // Initialize allegro

        ALLEGRO_DISPLAY *display = nullptr;

        if (!al_init())
        {
            fprintf(stderr, "Failed to initialize allegro.\n");
            return 1;
        }

        // Create the display
        display = al_create_display(settings.getDisplaySize(), settings.getDisplaySize());
        if (!display)
        {
            fprintf(stderr, "Failed to create display.\n");
            return 1;
        }

        al_init_primitives_addon();
        al_init_font_addon();
        al_init_ttf_addon();

        rgb aliveCellColorRGB = settings.getDeadCellColor();
        ALLEGRO_COLOR aliveCellColor = al_map_rgb(aliveCellColor.r, aliveCellColor.g, aliveCellColor.b);

        rgb deadCellColorRGB = settings.getDeadCellColor();
        ALLEGRO_COLOR deadCellColor = al_map_rgb(deadCellColorRGB.r, deadCellColorRGB.g, deadCellColorRGB.b);

        ALLEGRO_FONT *fontUbuntuB;
        rgb textRGB;
        ALLEGRO_COLOR generationTextColor;

        if (settings.areGenerationsDisplayedOnScreen())
        {
            fontUbuntuB = al_load_font("fonts/Ubuntu-B.ttf", settings.getGenerationTextTypefaceSize(), 1);
            textRGB = settings.getGenerationTextColor();
            generationTextColor = al_map_rgb(textRGB.r, textRGB.g, textRGB.b);
        }

        al_flip_display();
        al_clear_to_color(deadCellColor);
        // ---------------------------------------------------------------
        #endif // USE_ALLEGRO_GRAPHICS

        double startTime = MPI_Wtime();


        for (int i = 0; i < settings.getNumberOfGenerations(); ++i)
        {
            sendBorders(subCurrentGeneration, contiguous, up, down);
            calculateNextSubInnerGeneration(subCurrentGeneration, subNewGeneration);
            reciveBorders(subCurrentGeneration, contiguous, up, down);
            calculateNexSubGenerationBorders(subCurrentGeneration, subNewGeneration);

            MPI_Gather(subNewGeneration + columns,
                        subRows - 2,
                        contiguous,
                        generationAfterGather,
                        subRows - 2,
                        contiguous,
                        0,
                        MPI_COMM_WORLD);
            
            int *subCurrentGenerationCopy = subCurrentGeneration;
            subCurrentGeneration = subNewGeneration;
            subNewGeneration = subCurrentGenerationCopy;
            

            #ifdef USE_ALLEGRO_GRAPHICS
            displayCurrentGeneration(generationAfterGather, aliveCellColor);
            
            if (settings.areGenerationsDisplayedOnScreen())
                displayGenerationText(fontUbuntuB, generationTextColor, i);

            al_flip_display();
            al_clear_to_color(deadCellColor);
            #endif // USE_ALLEGRO_GRAPHICS

            std::this_thread::sleep_for(std::chrono::milliseconds(settings.getMillisecodsToWaitForEachGeneration()));

            MPI_Barrier(MPI_COMM_WORLD);
        }


        double endTime = MPI_Wtime();


        //implement write on file
        printf("Elapsed time: %.3f\n", endTime - startTime);


        #ifdef USE_ALLEGRO_GRAPHICS
        al_destroy_display(display);
        #endif

    }
    else
    {

        for (int i = 0; i < settings.getNumberOfGenerations(); ++i)
        {
            sendBorders(subCurrentGeneration, contiguous, up, down);
            calculateNextSubInnerGeneration(subCurrentGeneration, subNewGeneration);
            reciveBorders(subCurrentGeneration, contiguous, up, down);
            calculateNexSubGenerationBorders(subCurrentGeneration, subNewGeneration);

            MPI_Gather(subNewGeneration + columns,
                        subRows - 2,
                        contiguous,
                        generationAfterGather,
                        subRows - 2,
                        contiguous,
                        0,
                        MPI_COMM_WORLD);
            
            int *subCurrentGenerationCopy = subCurrentGeneration;
            subCurrentGeneration = subNewGeneration;
            subNewGeneration = subCurrentGenerationCopy;
            
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
    

    delete[] subCurrentGeneration;
    delete[] subNewGeneration;
    delete[] generationAfterGather;

    MPI_Type_free(&contiguous);



    return 0;
}

#endif // PARALLEL_LIFE_ENGINE