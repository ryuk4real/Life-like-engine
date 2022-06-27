#ifndef UTILS
#define UTILS

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <random>
#include <thread> //sleep_for
#include <chrono> //milliseconds

#include "Settings.hpp"

#define ALIVE   1
#define DEAD    0

Settings settings;


inline int m(int i, int j)
{
    return ((i * settings.getMatrixSize()) + j);
}


void initializeFirstGenerationForTesting(int _rows, int _columns, int * subCurrentGeneration, int processId)
{
    for (int i = 0; i < _rows; ++i)
    {
        for (int j = 0; j < _columns; ++j)
        {
            if ((j + i) % 5 == 0)
                subCurrentGeneration[m(i, j)] = 1;
            else
                subCurrentGeneration[m(i,j)] = 0;
        }
    }

    #if defined(PARALLEL)
    if (processId == 0)
    {
        for (int i = 0; i < 3; ++i) subCurrentGeneration[m(1,i)] = 1;
    }
    #endif
    
    #if defined(SERIAL)
    if (processId == 0)
    {
        for (int i = 0; i < 3; ++i) subCurrentGeneration[m(0,i)] = 1;
    }
    #endif


}


void initializeRandomizedFirstGeneration(int &_rows, int &_columns, int * currentGeneration, unsigned int seed)
{

    for (int i = 0; i < _rows; ++i)
    {
        for (int j = 0; j < _columns; ++j)
        {
            currentGeneration[m(i, j)] = rand_r(&seed) % 2;
        }
    }

    //just a slider
    /* if (seed==0)
    {
        int ii = 3;
        int jj = 10;
        currentGeneration[m(ii-1,jj)]=1;
        currentGeneration[m(ii,jj+1)]=1;
        currentGeneration[m(ii+1,jj-1)]=1;
        currentGeneration[m(ii+1,jj)]=1;
        currentGeneration[m(ii+1,jj+1)]=1;
    } */
}

void drawCell(int i, int j, ALLEGRO_COLOR cellColor)
{
    double squareSize = (double )settings.getDisplaySize() / (double) settings.getMatrixSize();

    al_draw_filled_rectangle(i * squareSize,
                             j * squareSize,
                             i * squareSize + squareSize,
                             j * squareSize + squareSize,
                             cellColor);
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

#endif //UTILS