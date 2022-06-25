#ifndef PARALLEL_LIFE_ENGINE
#define PARALLEL_LIFE_ENGINE


#include "utils.cpp"


#define USE_ALLEGRO_GRAPHICS


#define ALIVE   1
#define DEAD    0


using namespace std;


inline void sendBorders(int * subCurrentGeneration, MPI_Datatype contiguous, int &up, int &down, int numberOfCpus)
{
    MPI_Request request;

    MPI_Isend(subCurrentGeneration,
              1,
              contiguous,
              up,
              99,
              MPI_COMM_WORLD,
              &request);

    MPI_Isend(subCurrentGeneration + (settings.getMatrixSize() / numberOfCpus + 2) * settings.getMatrixSize(),
              1,
              contiguous,
              down,
              99,
              MPI_COMM_WORLD,
              &request);
}

inline void reciveBorders(int * subCurrentGeneration, MPI_Datatype contiguous, int &up, int &down, int &numberOfCpus)
{
    MPI_Recv(subCurrentGeneration,
             1,
             contiguous,
             up,
             99,
             MPI_COMM_WORLD,
             nullptr);
    
    MPI_Recv(subCurrentGeneration + (settings.getMatrixSize() / numberOfCpus + 2) * settings.getMatrixSize(),
             1,
             contiguous,
             up,
             99,
             MPI_COMM_WORLD,
             nullptr);
}

inline void parallelTransictionCell(int i, int j, int * subCurrentGeneration, int * subNextGeneration)
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
                subNextGeneration[m(i, j)] = ALIVE;
            }
            else
            {
                subNextGeneration[m(i, j)] = DEAD;
            }
        }
        else
        {
            if (settings.getStringRule().getNeighbourFromSurvivorSubsetAt(neighbours))
            {
                subNextGeneration[m(i, j)] = ALIVE;
            }
            else
            {
                subNextGeneration[m(i, j)] = DEAD;
            }
        }
    }
}




void calculateNextSubInnerGeneration(int &subRows, int &columns, int * subCurrentGeneration, int * subNextGeneration)
{
    for (int i = 1; i < subRows - 1; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            parallelTransictionCell(i,j, subCurrentGeneration, subNextGeneration);
        }
    }
}

inline void calculateNexSubGenerationBorders(int &subRows, int &columns, int * subCurrentGeneration, int * subNextGeneration)
{
    for (int j = 0; j < columns; ++j)
    {
        parallelTransictionCell(0, j, subCurrentGeneration, subNextGeneration);
        parallelTransictionCell(subRows, j, subCurrentGeneration, subNextGeneration);
    }
}

inline void swapSubGeneration(int * subCurrentGeneration, int * subNextGeneration)
{
    int *subCurrentGenerationCopy = subCurrentGeneration;
    subCurrentGeneration = subNextGeneration;
    subNextGeneration = subCurrentGenerationCopy;
}

inline void displayCurrentGeneration(int * generationAfterGather, ALLEGRO_COLOR aliveCellColor)
{
    for (int i = 0; i < settings.getMatrixSize(); ++i)
    {
        for (int j = 0; j < settings.getMatrixSize(); ++j)
        {
            drawCell(i, j, aliveCellColor);
        }
    }
}



int parallelLifeEngine(int &numberOfCpus)
{
    // USING 1D TOPOLOGY

    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    int subRows = ((settings.getMatrixSize() / numberOfCpus) + 2);
    int columns = settings.getMatrixSize();

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
    int * subNextGeneration = new int[subRows * columns];


    if (settings.isFirstGenerationRandomized())
            initializeRandomizedFirstGeneration(subRows, columns, subCurrentGeneration);
    else
        initializeFirstGenerationForTesting(subRows, columns, subCurrentGeneration);
    

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
        aliveCellColor = al_map_rgb(aliveCellColor.r, aliveCellColor.g, aliveCellColor.b);

        rgb deadCellColorRGB = settings.getDeadCellColor();
        deadCellColor = al_map_rgb(deadCellColorRGB.r, deadCellColorRGB.g, deadCellColorRGB.b);

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


        int * generationAfterGather = new int[rows * columns];


        double startTime = MPI_Wtime();


        for (int i = 0; i < settings.getNumberOfGenerations(); ++i)
        {
            sendBorders(subCurrentGeneration, contiguous, up, down, numberOfCpus);
            calculateNextSubInnerGeneration(subRows, columns, subCurrentGeneration, subNextGeneration);
            reciveBorders(subCurrentGeneration, contiguous, up, down, numberOfCpus);
            calculateNexSubGenerationBorders(subRows, columns, subCurrentGeneration, subNextGeneration);

            //save sub gen 0 to after gather

            for (int process = 1; process < numberOfCpus; ++process)
                MPI_Recv(generationAfterGather + ((subRows - 2) * i),
                         subRows - 2,
                         contiguous,
                         process,
                         66,
                         MPI_COMM_WORLD,
                         nullptr);

            swapSubGeneration(subCurrentGeneration, subNextGeneration);
            
            #ifdef USE_ALLEGRO_GRAPHICS
            displayCurrentGeneration(generationAfterGather, aliveCellColor);
            #endif
            
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
            sendBorders(subCurrentGeneration, contiguous, up, down, numberOfCpus);
            calculateNextSubInnerGeneration(subRows, columns, subCurrentGeneration, subNextGeneration);
            reciveBorders(subCurrentGeneration, contiguous, up, down, numberOfCpus);
            calculateNexSubGenerationBorders(subRows, columns, subCurrentGeneration, subNextGeneration);

            MPI_Send(subCurrentGeneration + columns,
                      subRows - 2,
                      contiguous,
                      0,
                      66,
                      MPI_COMM_WORLD);
            
            MPI_Barrier(MPI_COMM_WORLD);
            swapSubGeneration(subCurrentGeneration, subNextGeneration);
            
        }
    }
    

    delete[] subCurrentGeneration;
    delete[] newGeneration;



    return 0;
}

#endif // PARALLEL_LIFE_ENGINE