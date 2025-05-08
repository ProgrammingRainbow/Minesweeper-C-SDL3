#ifndef BOARD_H
#define BOARD_H

#include "main.h"

struct Board {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_FRect *src_rects;
        unsigned **front_array;
        unsigned **back_array;
        unsigned rows;
        unsigned columns;
        float piece_size;
        float left_offset;
        float top_offset;
        int mine_count;
};

bool board_new(struct Board **board, SDL_Renderer *renderer, unsigned rows,
               unsigned columns, int mine_count);
void board_free(struct Board **board);
bool board_reset(struct Board *b, int mine_count);
void board_draw(const struct Board *b);

#endif
