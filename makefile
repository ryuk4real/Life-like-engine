all:
	mpiCC src/main.cpp -O3 -o parallel-noGraphics-Life-like-engine.out -I/usr/include/allegro5 -L/usr/lib -lallegro -lallegro_primitives -lallegro_ttf -lallegro_font