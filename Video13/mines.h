#ifndef MINES_H
#define MINES_H

#include "main.h"

struct Mines {
        SDL_Renderer *renderer;
        SDL_Texture *back_image;
        SDL_Texture *digit_image;
        SDL_FRect *back_src_rects;
        SDL_FRect *digit_src_rects;
        SDL_FRect back_dest_rect;
        float digit_width;
        float digit_height;
        int mine_count;
        unsigned digits[3];
};

bool mines_new(struct Mines **mines, SDL_Renderer *renderer, int mine_count);
void mines_free(struct Mines **mines);
void mines_reset(struct Mines *m, int mine_count);
void mines_increment(struct Mines *m);
void mines_decrement(struct Mines *m);
void mines_draw(const struct Mines *m);

#endif
