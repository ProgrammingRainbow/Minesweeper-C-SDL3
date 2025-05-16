#ifndef CLOCK_H
#define CLOCK_H

#include "main.h"

struct Clock {
        SDL_Renderer *renderer;
        SDL_Texture *back_image;
        SDL_Texture *digit_image;
        SDL_FRect *back_src_rects;
        SDL_FRect *digit_src_rects;
        SDL_FRect back_dest_rect;
        float digit_width;
        float digit_height;
        unsigned columns;
        unsigned digits[3];
        unsigned seconds;
        Uint64 last_time;
};

bool clock_new(struct Clock **clock, SDL_Renderer *renderer, unsigned columns);
void clock_free(struct Clock **clock);
void clock_reset(struct Clock *c);
void clock_update(struct Clock *c);
void clock_draw(const struct Clock *c);

#endif
