#include "mines.h"
#include "load_media.h"

void mines_update_digits(struct Mines *m);

bool mines_new(struct Mines **mines, SDL_Renderer *renderer, int mine_count,
               float scale) {
    *mines = calloc(1, sizeof(struct Mines));
    if (!*mines) {
        fprintf(stderr, "Error in calloc of new Mines.\n");
        return false;
    }
    struct Mines *m = *mines;

    m->renderer = renderer;
    m->mine_count = mine_count;
    m->scale = scale;

    if (!load_media_sheet(m->renderer, &m->back_image, "images/digitback.png",
                          DIGIT_BACK_WIDTH, DIGIT_BACK_HEIGHT,
                          &m->back_src_rects)) {
        return false;
    }

    if (!load_media_sheet(m->renderer, &m->digit_image, "images/digits.png",
                          DIGIT_WIDTH, DIGIT_HEIGHT, &m->digit_src_rects)) {
        return false;
    }

    mines_update_digits(m);
    mines_set_scale(m, m->scale);

    return true;
}

void mines_free(struct Mines **mines) {
    if (*mines) {
        struct Mines *m = *mines;

        if (m->digit_src_rects) {
            free(m->digit_src_rects);
            m->digit_src_rects = NULL;
        }
        if (m->digit_image) {
            SDL_DestroyTexture(m->digit_image);
            m->digit_image = NULL;
        }

        if (m->back_src_rects) {
            free(m->back_src_rects);
            m->back_src_rects = NULL;
        }
        if (m->back_image) {
            SDL_DestroyTexture(m->back_image);
            m->back_image = NULL;
        }

        m->renderer = NULL;

        free(m);
        m = NULL;
        *mines = NULL;

        printf("Free Mines.\n");
    }
}

void mines_update_digits(struct Mines *m) {
    unsigned mine_count = (unsigned)abs(m->mine_count);

    if (m->mine_count > 999) {
        mine_count = 999;
    }

    if (m->mine_count < -99) {
        mine_count = 99;
    }

    if (mine_count > 99) {
        m->digits[0] = mine_count / 100;
    } else if (m->mine_count < 0) {
        m->digits[0] = 10;
    } else {
        m->digits[0] = 11;
    }

    m->digits[1] = (mine_count > 9) ? (mine_count / 10) % 10 : 11;

    m->digits[2] = mine_count % 10;
}

void mines_reset(struct Mines *m, int mine_count) {
    m->mine_count = mine_count;
    mines_update_digits(m);
}

void mines_set_scale(struct Mines *m, float scale) {
    m->scale = scale;
    m->back_dest_rect.x = DIGIT_BACK_LEFT * m->scale;
    m->back_dest_rect.y = DIGIT_BACK_TOP * m->scale;
    m->back_dest_rect.w = DIGIT_BACK_WIDTH * m->scale;
    m->back_dest_rect.h = DIGIT_BACK_HEIGHT * m->scale;
    m->digit_width = DIGIT_WIDTH * m->scale;
    m->digit_height = DIGIT_HEIGHT * m->scale;
}

void mines_increment(struct Mines *m) {
    m->mine_count++;
    mines_update_digits(m);
}

void mines_decrement(struct Mines *m) {
    m->mine_count--;
    mines_update_digits(m);
}

void mines_draw(const struct Mines *m) {
    SDL_RenderTexture(m->renderer, m->back_image, &m->back_src_rects[0],
                      &m->back_dest_rect);
    SDL_FRect digit_rect = {0, m->back_dest_rect.y + 2, m->digit_width,
                            m->digit_height};
    for (int i = 0; i < 3; i++) {
        digit_rect.x = m->back_dest_rect.x + 2 + digit_rect.w * (float)i;
        SDL_RenderTexture(m->renderer, m->digit_image,
                          &m->digit_src_rects[m->digits[i]], &digit_rect);
    }
}
