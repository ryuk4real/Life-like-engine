#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <mpi.h>

#include "headers/Settings.hpp"



Settings settings;


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

	ALLEGRO_FONT *fontUbuntuB;
	rgb textRGB;
	ALLEGRO_COLOR generationTextColor;

	if (settings.areGenerationsDisplayedOnScreen())
	{
		fontUbuntuB = al_load_font("fonts/Ubuntu-B.ttf", settings.getGenerationTextTypefaceSize(), 1);
		textRGB = settings.getGenerationTextColor();
		generationTextColor = al_map_rgb(textRGB.r, textRGB.g, textRGB.b);
	}


	
	rgb deadCellColorRGB = settings.getDeadCellColor();
	ALLEGRO_COLOR deadCellColor = al_map_rgb(deadCellColorRGB.r, deadCellColorRGB.g, deadCellColorRGB.b);

	al_flip_display();
	al_clear_to_color(deadCellColor);
    // ---------------------------------------------------------------




	MPI_Init(&args, &argv);





	// Game Loop -----------------------------------------------------
	
	for (int i = 0; i < settings.getNumberOfGenerations(); ++i)
	{

		if (settings.areGenerationsDisplayedOnScreen())
		{
			// Print the number of generations on screen	
			std::string generation = std::to_string(i);
			al_draw_text(fontUbuntuB, generationTextColor, 10, 10, ALLEGRO_ALIGN_LEFT, generation.c_str());
		}

		al_flip_display();
        al_clear_to_color(deadCellColor);
	}

	// ---------------------------------------------------------------






	// DESTROY -------------------------------------------------------
	al_destroy_display(display);

	MPI_Finalize();
	// ---------------------------------------------------------------

	return 0;
}