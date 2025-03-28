#include "mines.h"
#include "load_media.h"

bool mines_new(struct Mines **mines, SDL_Renderer *renderer) {
    *mines = calloc(1, sizeof(struct Mines));
    if (!*mines) {
        fprintf(stderr, "Error in calloc of new clock.\n");
        return false;
    }
    struct Mines *m = *mines;

    m->renderer = renderer;

    m->back_dest_rect.x = DIGIT_BACK_LEFT * 2;
    m->back_dest_rect.y = DIGIT_BACK_TOP * 2;
    m->back_dest_rect.w = DIGIT_BACK_WIDTH * 2;
    m->back_dest_rect.h = DIGIT_BACK_HEIGHT * 2;
    m->digit_rect.x = DIGIT_BACK_LEFT * 2 + 2;
    m->digit_rect.y = DIGIT_BACK_TOP * 2 + 2;
    m->digit_rect.w = DIGIT_WIDTH * 2;
    m->digit_rect.h = DIGIT_HEIGHT * 2;

    if (!load_media_sheet(m->renderer, &m->back_image, "images/digitback.png",
                          DIGIT_BACK_WIDTH, DIGIT_BACK_HEIGHT,
                          &m->back_src_rects)) {
        return false;
    }

    if (!load_media_sheet(m->renderer, &m->digit_image, "images/digits.png",
                          DIGIT_WIDTH, DIGIT_HEIGHT, &m->digit_src_rects)) {
        return false;
    }

    return true;
}

void mines_free(struct Mines **mines) {
    if (*mines) {
        struct Mines *m = *mines;

        if (m->back_src_rects) {
            free(m->back_src_rects);
            m->back_src_rects = NULL;
        }

        if (m->back_image) {
            SDL_DestroyTexture(m->back_image);
            m->back_image = NULL;
        }

        if (m->digit_src_rects) {
            free(m->digit_src_rects);
            m->digit_src_rects = NULL;
        }

        if (m->digit_image) {
            SDL_DestroyTexture(m->digit_image);
            m->digit_image = NULL;
        }

        m->renderer = NULL;

        free(m);
        m = NULL;
        *mines = NULL;

        printf("mines clean.\n");
    }
}

void mines_draw(const struct Mines *m) {
    SDL_RenderTexture(m->renderer, m->back_image, &m->back_src_rects[0],
                      &m->back_dest_rect);
    SDL_FRect dest_rect = {m->digit_rect.x, m->digit_rect.y, m->digit_rect.w,
                           m->digit_rect.h};
    for (int i = 0; i < 3; i++) {
        dest_rect.x = m->digit_rect.x + m->digit_rect.w * (float)i;
        SDL_RenderTexture(m->renderer, m->digit_image, &m->digit_src_rects[0],
                          &dest_rect);
    }
}
