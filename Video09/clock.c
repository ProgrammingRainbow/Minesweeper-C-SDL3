#include "clock.h"
#include "load_media.h"

bool clock_new(struct Clock **clock, SDL_Renderer *renderer, unsigned columns) {
    *clock = calloc(1, sizeof(struct Clock));
    if (!*clock) {
        fprintf(stderr, "Error in calloc of new Clock.\n");
        return false;
    }
    struct Clock *c = *clock;

    c->renderer = renderer;
    c->columns = columns;

    c->back_dest_rect.x = (PIECE_SIZE * ((float)c->columns + 1) - BORDER_LEFT -
                           DIGIT_BACK_WIDTH - DIGIT_BACK_RIGHT) *
                          2;
    c->back_dest_rect.y = DIGIT_BACK_TOP * 2;
    c->back_dest_rect.w = DIGIT_BACK_WIDTH * 2;
    c->back_dest_rect.h = DIGIT_BACK_HEIGHT * 2;
    c->digit_width = DIGIT_WIDTH * 2;
    c->digit_height = DIGIT_HEIGHT * 2;

    if (!load_media_sheet(c->renderer, &c->back_image, "images/digitback.png",
                          DIGIT_BACK_WIDTH, DIGIT_BACK_HEIGHT,
                          &c->back_src_rects)) {
        return false;
    }

    if (!load_media_sheet(c->renderer, &c->digit_image, "images/digits.png",
                          DIGIT_WIDTH, DIGIT_HEIGHT, &c->digit_src_rects)) {
        return false;
    }

    return true;
}

void clock_free(struct Clock **clock) {
    if (*clock) {
        struct Clock *c = *clock;

        if (c->digit_src_rects) {
            free(c->digit_src_rects);
            c->digit_src_rects = NULL;
        }
        if (c->digit_image) {
            SDL_DestroyTexture(c->digit_image);
            c->digit_image = NULL;
        }

        if (c->back_src_rects) {
            free(c->back_src_rects);
            c->back_src_rects = NULL;
        }
        if (c->back_image) {
            SDL_DestroyTexture(c->back_image);
            c->back_image = NULL;
        }

        c->renderer = NULL;

        free(c);
        c = NULL;
        *clock = NULL;

        printf("Free Clock.\n");
    }
}

void clock_draw(const struct Clock *c) {
    SDL_RenderTexture(c->renderer, c->back_image, &c->back_src_rects[0],
                      &c->back_dest_rect);
    SDL_FRect digit_rect = {0, c->back_dest_rect.y + 2, c->digit_width,
                            c->digit_height};
    for (int i = 0; i < 3; i++) {
        digit_rect.x = c->back_dest_rect.x + 2 + digit_rect.w * (float)i;
        SDL_RenderTexture(c->renderer, c->digit_image, &c->digit_src_rects[0],
                          &digit_rect);
    }
}
