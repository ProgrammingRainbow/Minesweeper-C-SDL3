#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "border.h"
#include "clock.h"
#include "face.h"
#include "main.h"
#include "mines.h"

struct Game {
        SDL_Window *window;
        SDL_Renderer *renderer;
        struct Border *border;
        struct Board *board;
        struct Mines *mines;
        struct Clock *clock;
        struct Face *face;
        unsigned rows;
        unsigned columns;
        SDL_Event event;
        bool is_running;
        bool is_playing;
        int mine_count;
        float scale;
        float difficulty;
        char *size_str;
        char *diff_str;
};

bool game_new(struct Game **game);
void game_free(struct Game **game);
bool game_run(struct Game *g);

#endif
