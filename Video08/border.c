#include "border.h"
#include "load_media.h"

bool border_new(struct Border **border, SDL_Renderer *renderer, unsigned rows,
                unsigned columns) {
    *border = calloc(1, sizeof(struct Border));
    if (*border == NULL) {
        fprintf(stderr, "Error in Calloc of New Border.\n");
        return false;
    }
    struct Border *b = *border;

    b->renderer = renderer;
    b->rows = rows;
    b->columns = columns;

    b->left_offset = BORDER_LEFT * 2;
    b->piece_width = PIECE_SIZE * 2;
    b->piece_height = BORDER_HEIGHT * 2;

    if (!load_media_sheet(b->renderer, &b->image, "images/borders.png",
                          PIECE_SIZE, BORDER_HEIGHT, &b->src_rects)) {
        return false;
    }

    return true;
}

void border_free(struct Border **border) {
    if (*border) {
        struct Border *b = *border;

        if (b->src_rects) {
            free(b->src_rects);
            b->src_rects = NULL;
        }

        if (b->image) {
            SDL_DestroyTexture(b->image);
            b->image = NULL;
        }

        b->renderer = NULL;

        free(b);
        b = NULL;
        *border = NULL;

        printf("Free Border.\n");
    }
}

void border_draw(const struct Border *b) {
    SDL_FRect dest_rect = {0, 0, b->piece_width, b->piece_height};
    dest_rect.x = -b->left_offset;
    dest_rect.y = 0;
    SDL_RenderTexture(b->renderer, b->image, &b->src_rects[0], &dest_rect);

    dest_rect.x = b->piece_width * (float)(b->columns + 1) - b->left_offset;
    dest_rect.y = 0;
    SDL_RenderTexture(b->renderer, b->image, &b->src_rects[2], &dest_rect);

    dest_rect.x = -b->left_offset;
    dest_rect.y = b->piece_width * (float)b->rows + b->piece_height;
    SDL_RenderTexture(b->renderer, b->image, &b->src_rects[5], &dest_rect);

    dest_rect.x = b->piece_width * (float)(b->columns + 1) - b->left_offset;
    dest_rect.y = b->piece_width * (float)b->rows + b->piece_height;
    SDL_RenderTexture(b->renderer, b->image, &b->src_rects[7], &dest_rect);

    for (unsigned column = 0; column < b->columns; column++) {
        dest_rect.x = b->piece_width * (float)(column + 1) - b->left_offset;
        dest_rect.y = 0;
        SDL_RenderTexture(b->renderer, b->image, &b->src_rects[1], &dest_rect);

        dest_rect.y = b->piece_width * (float)b->rows + b->piece_height;
        SDL_RenderTexture(b->renderer, b->image, &b->src_rects[6], &dest_rect);
    }

    for (unsigned row = 0; row < b->rows; row++) {
        dest_rect.x = -b->left_offset;
        dest_rect.y = b->piece_width * (float)row + b->piece_height;
        SDL_RenderTexture(b->renderer, b->image, &b->src_rects[3], &dest_rect);

        dest_rect.x = b->piece_width * (float)(b->columns + 1) - b->left_offset;
        SDL_RenderTexture(b->renderer, b->image, &b->src_rects[4], &dest_rect);
    }
}
