#ifndef BOARD_H
#define BOARD_H

#include "main.h"

struct Board {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_FRect *src_rects;
        unsigned **front_array;
        SDL_FRect rect;
        unsigned rows;
        unsigned columns;
        float piece_size;
};

bool board_new(struct Board **board, SDL_Renderer *renderer, unsigned rows,
               unsigned columns);
void board_free(struct Board **board);
bool board_reset(struct Board *b);
void board_draw(const struct Board *b);

#endif
