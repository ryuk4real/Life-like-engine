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

int rows = settings.getMatrixSize();
int columns = settings.getMatrixSize();

double squareHeight = (double) settings.getDisplaySize() / (double) settings.getMatrixSize();

int *currentGeneration;
int *newGeneration;

ALLEGRO_COLOR deadCellColor;
ALLEGRO_COLOR aliveCellColor;

inline int m(int i, int j)
{
    return ((i * columns) + j);
}

void initializeRandomizedFirstGeneration(int _rows, int _columns)
{
    unsigned int seed = time(nullptr);

    for (int i = 0; i < _rows; ++i)
    {
        for (int j = 0; j < _columns; ++j)
        {
            currentGeneration[m(i, j)] = rand_r(&seed) % 2;
            newGeneration[m(i, j)] = 0;
        }
    }
}


void drawCell(int i, int j, ALLEGRO_COLOR cellColor)
{
    al_draw_filled_rectangle(i * squareHeight,
                             j * squareHeight,
                             i * squareHeight + squareHeight,
                             j * squareHeight + squareHeight,
                             cellColor);
}


void swapLastGeneration()
{
    int *currentGenerationCopy = currentGeneration;
    currentGeneration = newGeneration;
    newGeneration = currentGenerationCopy;
}


inline void transictionCell(int i, int j)
{

    int neighbours = 0;

    for (int r = -1; r <= 1; ++r)
    {
        for (int c = -1; c <= 1; ++c)
        {

            if ((r != 0 || c != 0))
            {
                if (currentGeneration[m((i + r + rows) % rows, ((j + c + columns) % columns))] == ALIVE)
                {
                    ++neighbours;
                }
            }
        }

        if (currentGeneration[m(i, j)] == ALIVE)
        {
            if (settings.getStringRule().getNeighbourFromBornSubsetAt(neighbours))
            {
                newGeneration[m(i, j)] = ALIVE;

                #ifdef USE_ALLEGRO_GRAPHICS
                drawCell(i, j, aliveCellColor);
                #endif
            }
            else
            {
                newGeneration[m(i, j)] = DEAD;
            }
        }
        else
        {
            if (settings.getStringRule().getNeighbourFromSurvivorSubsetAt(neighbours))
            {
                newGeneration[m(i, j)] = ALIVE;

#               ifdef USE_ALLEGRO_GRAPHICS
                drawCell(i, j, aliveCellColor);
                #endif
            }
            else
            {
                newGeneration[m(i, j)] = DEAD;
            }
        }
    }
}


void generateNextGeneration()
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {

            transictionCell(i, j);
        }
    }
}




void displayGenerationText(ALLEGRO_FONT *font, ALLEGRO_COLOR textColor, int &i)
{
    if (settings.areGenerationsDisplayedOnScreen())
    {
        // Print the number of generations on screen
        string generation = to_string(i);
        al_draw_text(font, textColor, 10, 10, ALLEGRO_ALIGN_LEFT, generation.c_str());
    }
}


int serialLifeEngine()
{
    // SEQUENTIAL ALGORITHM

        currentGeneration = new int[rows * columns];
        newGeneration = new int[rows * columns];

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

        if (settings.isFirstGenerationRandomized())
            initializeRandomizedFirstGeneration(rows, columns);

        double startTime = MPI_Wtime();

        // Game Loop -----------------------------------------------------
        for (int i = 0; i < settings.getNumberOfGenerations(); ++i)
        {

            generateNextGeneration();
            swapLastGeneration();

            #ifdef USE_ALLEGRO_GRAPHICS
            if (settings.areGenerationsDisplayedOnScreen())
                displayGenerationText(fontUbuntuB, generationTextColor, i);

            al_flip_display();
            al_clear_to_color(deadCellColor);
            #endif // USE_ALLEGRO_GRAPHICS

            std::this_thread::sleep_for(std::chrono::milliseconds(settings.getMillisecodsToWaitForEachGeneration()));
        }
        // ---------------------------------------------------------------

        #ifdef USE_ALLEGRO_GRAPHICS
        al_destroy_display(display);
        #endif

        double endTime = MPI_Wtime();

        delete[] currentGeneration;
        delete[] newGeneration;

        //implement write on file
        printf("Elapsed time: %.3f\n", endTime - startTime);

        return 0;
}
