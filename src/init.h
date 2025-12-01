#ifndef INIT_H
#define INIT_H
#include "SDL2/SDL.h"
void initSDL(SDL_Window **window, SDL_Renderer **renderer);
void cleanup(SDL_Window *window, SDL_Renderer *renderer);
#endif
