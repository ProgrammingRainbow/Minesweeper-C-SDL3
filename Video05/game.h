#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "border.h"
#include "main.h"
#include "mines.h"

struct Game {
        SDL_Window *window;
        SDL_Renderer *renderer;
        struct Border *border;
        struct Board *board;
        struct Mines *mines;
        unsigned rows;
        unsigned columns;
        SDL_Event event;
        bool is_running;
};

bool game_new(struct Game **game);
void game_free(struct Game **game);
void game_run(struct Game *g);

#endif
