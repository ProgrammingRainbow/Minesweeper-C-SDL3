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
        unsigned image_index;
        unsigned theme;
};

bool face_new(struct Face **face, SDL_Renderer *renderer, unsigned columns,
              float scale);
void face_free(struct Face **face);
void face_set_scale(struct Face *f, float scale);
bool face_mouse_click(struct Face *f, float x, float y, bool down);
void face_default(struct Face *f);
void face_won(struct Face *f);
void face_lost(struct Face *f);
void face_question(struct Face *f);
void face_set_theme(struct Face *f, unsigned theme);
void face_set_size(struct Face *f, unsigned columns);
void face_draw(const struct Face *f);

#endif
