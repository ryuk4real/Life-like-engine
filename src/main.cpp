#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

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

	
	rgb deadCellColorRGB = settings.getDeadCellColor();
	ALLEGRO_COLOR deadCellColor = al_map_rgb(deadCellColorRGB.r, deadCellColorRGB.g, deadCellColorRGB.b);
	al_clear_to_color(deadCellColor);
	al_flip_display();
    // ---------------------------------------------------------------

	while(true) {}

	return 0;
}