#include "clock.h"
#include "load_media.h"

void clock_update_digits(struct Clock *c);

bool clock_new(struct Clock **clock, SDL_Renderer *renderer, unsigned columns,
               float scale) {
    *clock = calloc(1, sizeof(struct Clock));
    if (!*clock) {
        fprintf(stderr, "Error in calloc of new clock.\n");
        return false;
    }
    struct Clock *c = *clock;

    c->renderer = renderer;
    c->columns = columns;
    c->scale = scale;

    if (!load_media_sheet(c->renderer, &c->back_image, "images/digitback.png",
                          DIGIT_BACK_WIDTH, DIGIT_BACK_HEIGHT,
                          &c->back_src_rects)) {
        return false;
    }

    if (!load_media_sheet(c->renderer, &c->digit_image, "images/digits.png",
                          DIGIT_WIDTH, DIGIT_HEIGHT, &c->digit_src_rects)) {
        return false;
    }

    clock_set_scale(c, c->scale);
    clock_update_digits(c);

    return true;
}

void clock_free(struct Clock **clock) {
    if (*clock) {
        struct Clock *c = *clock;

        if (c->back_src_rects) {
            free(c->back_src_rects);
            c->back_src_rects = NULL;
        }

        if (c->back_image) {
            SDL_DestroyTexture(c->back_image);
            c->back_image = NULL;
        }

        if (c->digit_src_rects) {
            free(c->digit_src_rects);
            c->digit_src_rects = NULL;
        }

        if (c->digit_image) {
            SDL_DestroyTexture(c->digit_image);
            c->digit_image = NULL;
        }

        c->renderer = NULL;

        free(*clock);
        *clock = NULL;

        printf("clock clean.\n");
    }
}

void clock_update_digits(struct Clock *c) {
    unsigned default_digit = 11;
    unsigned seconds = c->seconds;

    if (c->seconds > 999) {
        seconds = 999;
    }

    if (seconds > 99) {
        c->digits[0] = (unsigned)(seconds / 100.0);
        default_digit = 0;
    } else {
        c->digits[0] = default_digit;
    }

    if (seconds > 9) {
        c->digits[1] = (unsigned)(seconds / 10.0) % 10;
    } else {
        c->digits[1] = default_digit;
    }

    c->digits[2] = seconds % 10;
}

void clock_reset(struct Clock *c) {
    c->last_time = SDL_GetTicks();
    c->seconds = 0;
    clock_update_digits(c);
}

void clock_set_scale(struct Clock *c, float scale) {
    c->scale = scale;
    c->back_dest_rect.x = (PIECE_SIZE * ((float)c->columns + 1) - BORDER_LEFT -
                           DIGIT_BACK_WIDTH - DIGIT_BACK_RIGHT) *
                          c->scale;
    c->back_dest_rect.y = DIGIT_BACK_TOP * c->scale;
    c->back_dest_rect.w = DIGIT_BACK_WIDTH * c->scale;
    c->back_dest_rect.h = DIGIT_BACK_HEIGHT * c->scale;
    c->digit_rect.x = c->back_dest_rect.x + c->scale;
    c->digit_rect.y = DIGIT_BACK_TOP * c->scale + c->scale;
    c->digit_rect.w = DIGIT_WIDTH * c->scale;
    c->digit_rect.h = DIGIT_HEIGHT * c->scale;
}

void clock_set_theme(struct Clock *c, unsigned theme) {
    c->back_theme = theme;
    c->digit_theme = theme * 12;
}

void clock_set_size(struct Clock *c, unsigned columns) {
    c->columns = columns;
    c->back_dest_rect.x = (PIECE_SIZE * ((float)c->columns + 1) - BORDER_LEFT -
                           DIGIT_BACK_WIDTH - DIGIT_BACK_RIGHT) *
                          c->scale;
}

void clock_update(struct Clock *c) {
    Uint64 current_time = SDL_GetTicks();
    Uint64 elapsed_time = 0;

    if (current_time >= c->last_time) {
        elapsed_time = current_time - c->last_time;
    } else {
        elapsed_time = (Uint64)-1 - c->last_time + current_time;
    }

    if (elapsed_time > 1000) {
        c->last_time += 1000;
        c->seconds++;
        clock_update_digits(c);
    }
}

void clock_draw(const struct Clock *c) {
    SDL_RenderTexture(c->renderer, c->back_image,
                      &c->back_src_rects[c->back_theme], &c->back_dest_rect);
    SDL_FRect dest_rect = {c->digit_rect.x, c->digit_rect.y, c->digit_rect.w,
                           c->digit_rect.h};
    for (int i = 0; i < 3; i++) {
        dest_rect.x = c->digit_rect.x + c->digit_rect.w * (float)i;
        SDL_RenderTexture(c->renderer, c->digit_image,
                          &c->digit_src_rects[c->digits[i] + c->digit_theme],
                          &dest_rect);
    }
}
