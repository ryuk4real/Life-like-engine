#ifndef SETTINGS_INSTANCE
#define SETTINGS_INSTANCE

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include "Settings.hpp"

//#define USE_ALLEGRO_GRAPHICS

Settings settings;


inline int m(int i, int j)
{
    return ((i * settings.getMatrixSize()) + j);
}


void initializeFirstGenerationForTesting(int &_rows, int &_columns, int * currentGeneration)
{
    for (int i = 0; i < _rows; ++i)
    {
        for (int j = 0; j < _columns; ++j)
        {
            if ((j + i) % 5 == 0)
                currentGeneration[m(i, j)] = 1;
            else
                currentGeneration[m(i,j)] = 0;
        }
    }

    //currentGeneration[m(_rows / 2, _columns / 2)] = 1;
    //currentGeneration[m(_rows / 2 + 1, _columns / 2 + 1)] = 1;


}


void initializeRandomizedFirstGeneration(int &_rows, int &_columns, int * currentGeneration)
{
    unsigned int seed = time(nullptr);

    for (int i = 0; i < _rows; ++i)
    {
        for (int j = 0; j < _columns; ++j)
        {
            currentGeneration[m(i, j)] = rand_r(&seed) % 2;
        }
    }
}

void drawCell(int i, int j, ALLEGRO_COLOR cellColor)
{
    al_draw_filled_rectangle(i * settings.getDisplaySize() / settings.getMatrixSize(),
                             j * settings.getDisplaySize() / settings.getMatrixSize(),
                             i * settings.getDisplaySize() / settings.getMatrixSize() + settings.getDisplaySize() / settings.getMatrixSize(),
                             j * settings.getDisplaySize() / settings.getMatrixSize() + settings.getDisplaySize() / settings.getMatrixSize(),
                             cellColor);
}

#endif