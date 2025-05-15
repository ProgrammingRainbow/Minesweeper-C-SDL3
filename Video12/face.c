#include "face.h"
#include "load_media.h"

bool face_new(struct Face **face, SDL_Renderer *renderer, unsigned columns) {
    *face = calloc(1, sizeof(struct Face));
    if (!*face) {
        fprintf(stderr, "Error in calloc of new Face.\n");
        return false;
    }
    struct Face *f = *face;

    f->renderer = renderer;
    f->columns = columns;

    f->dest_rect.x = ((PIECE_SIZE * (float)f->columns - FACE_SIZE) / 2 +
                      PIECE_SIZE - BORDER_LEFT) *
                     2;
    f->dest_rect.y = FACE_TOP * 2;
    f->dest_rect.w = FACE_SIZE * 2;
    f->dest_rect.h = FACE_SIZE * 2;

    if (!load_media_sheet(f->renderer, &f->image, "images/faces.png", FACE_SIZE,
                          FACE_SIZE, &f->src_rects)) {
        return false;
    }

    return true;
}

void face_free(struct Face **face) {
    if (*face) {
        struct Face *f = *face;

        if (f->src_rects) {
            free(f->src_rects);
            f->src_rects = NULL;
        }
        if (f->image) {
            SDL_DestroyTexture(f->image);
            f->image = NULL;
        }

        f->renderer = NULL;

        free(f);
        f = NULL;
        *face = NULL;

        printf("Free Face.\n");
    }
}

bool face_mouse_click(struct Face *f, float x, float y, bool down) {
    if (x >= f->dest_rect.x && x < f->dest_rect.x + f->dest_rect.w) {
        if (y >= f->dest_rect.y && y < f->dest_rect.y + f->dest_rect.h) {
            if (down) {
                f->image_index = 1;
            } else if (f->image_index == 1) {
                f->image_index = 0;
                return true;
            }
        }
    } else if (!down) {
        f->image_index = 0;
    }

    return false;
}

void face_default(struct Face *f) { f->image_index = 0; }

void face_won(struct Face *f) { f->image_index = 3; }

void face_lost(struct Face *f) { f->image_index = 4; }

void face_question(struct Face *f) { f->image_index = 2; }

void face_draw(const struct Face *f) {
    SDL_RenderTexture(f->renderer, f->image, &f->src_rects[f->image_index],
                      &f->dest_rect);
}
