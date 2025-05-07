#ifndef FACE_H
#define FACE_H

#include "main.h"

struct Face {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_FRect *src_rects;
        SDL_FRect dest_rect;
        unsigned columns;
        float scale;
};

bool face_new(struct Face **face, SDL_Renderer *renderer, unsigned columns);
void face_free(struct Face **face);
void face_set_scale(struct Face *f, float scale);
void face_draw(const struct Face *f);

#endif
