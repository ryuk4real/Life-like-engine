#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <mpi.h>

#include <random>
#include <thread> //sleep_for
#include <chrono> //milliseconds

#include "headers/Settings.hpp"

#define ALIVE 1
#define DEAD 0

using namespace std;



Settings settings;

int rows = settings.getDisplaySize();
int columns = settings.getDisplaySize();

int squareHeight = settings.getDisplaySize() / settings.getMatrixSize();

int *currentGeneration;
int *newGeneration;

ALLEGRO_COLOR deadCellColor;
ALLEGRO_COLOR aliveCellColor;




inline void drawCell(int i, int j, ALLEGRO_COLOR cellColor);

void initializeRandomizedFirstGeneration();

void swapLastGeneration();
void generateNextGeneration();
inline void transictionCell(int i, int j);

inline int m(int i, int j);







int main(int args, char **argv)
{

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




	MPI_Init(&args, &argv);



	int numberOfCpus;
	MPI_Comm_size(MPI_COMM_WORLD, &numberOfCpus);

    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);


	if (numberOfCpus == 1)
	{
        // Sequential algorithm
		currentGeneration = new int[rows * columns];
		newGeneration = new int[rows * columns];

		if (settings.isFirstGenerationRandomized()) initializeRandomizedFirstGeneration();



		// Game Loop -----------------------------------------------------	
		for (int i = 0; i < settings.getNumberOfGenerations(); ++i)
		{

            generateNextGeneration();
            swapLastGeneration();

			if (settings.areGenerationsDisplayedOnScreen())
			{
				// Print the number of generations on screen	
				std::string generation = std::to_string(i);
				al_draw_text(fontUbuntuB, generationTextColor, 10, 10, ALLEGRO_ALIGN_LEFT, generation.c_str());
			}
            

			al_flip_display();
			al_clear_to_color(deadCellColor);

            std::this_thread::sleep_for(std::chrono::milliseconds(settings.getMillisecodsToWaitForEachGeneration()));
		}
		// ---------------------------------------------------------------
	}
	else if (numberOfCpus % 2 == 0)
    {
        
    }
	else {}








	// DESTROY -------------------------------------------------------
	al_destroy_display(display);

    delete [] currentGeneration;
    delete [] newGeneration;

	MPI_Finalize();
	// ---------------------------------------------------------------

	return 0;
}








void initializeRandomizedFirstGeneration()
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            currentGeneration[m(i,j)] = rand()%2;
            newGeneration[m(i,j)] = 0;
        }
    }
}

void swapLastGeneration()
{
    int *currentGenerationCopy = currentGeneration;
    currentGeneration = newGeneration;
    newGeneration = currentGenerationCopy;
}

void generateNextGeneration()
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {

            transictionCell(i,j);

        }
    }
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
                if (currentGeneration[m((i + r + rows) % rows, ((j + c + columns) % columns))] == 1)
                {
                    ++neighbours;
                }           
            }
        }
        
        if (currentGeneration[m(i,j)] == 1)
        {
            if (settings.getStringRule().getNeighbourFromBornSubsetAt(neighbours))
            {
                newGeneration[m(i,j)] = 1;
                drawCell(i,j, aliveCellColor);
            }
            else 
            {
                newGeneration[m(i,j)] = 0;

            }
        }
        else
        {
            if (settings.getStringRule().getNeighbourFromSurvivorSubsetAt(neighbours))
            {
                newGeneration[m(i,j)] = 1;
                drawCell(i,j, aliveCellColor);
            }
            else
            {
                newGeneration[m(i,j)] = 0;
            }
        }
    }
}

inline int m(int i, int j)
{
    return ((i * columns) + j);
}

void drawCell(int i, int j, ALLEGRO_COLOR cellColor)
{
   al_draw_filled_rectangle(i * squareHeight,
                            j * squareHeight,
                            i * squareHeight + squareHeight,
                            j * squareHeight + squareHeight,
                            cellColor);
}

