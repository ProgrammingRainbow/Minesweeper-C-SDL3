#ifndef FACE_H
#define FACE_H

#include "main.h"

struct Face {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_FRect *src_rects;
        SDL_FRect dest_rect;
        unsigned columns;
        unsigned image_index;
        float scale;
};

bool face_new(struct Face **face, SDL_Renderer *renderer, unsigned columns,
              float scale);
void face_free(struct Face **face);
void face_set_scale(struct Face *b, float scale);
bool face_mouse_click(struct Face *f, float x, float y, bool down);
void face_default(struct Face *f);
void face_won(struct Face *f);
void face_lost(struct Face *f);
void face_question(struct Face *f);
void face_draw(const struct Face *f);

#endif
