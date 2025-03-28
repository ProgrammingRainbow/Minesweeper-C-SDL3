#include "board.h"
#include "load_media.h"

bool board_calloc_arrays(struct Board *b);
void board_free_arrays(struct Board *b);

bool board_new(struct Board **board, SDL_Renderer *renderer, unsigned rows,
               unsigned columns) {
    *board = calloc(1, sizeof(struct Board));
    if (!*board) {
        fprintf(stderr, "Error in calloc of new board.\n");
        return false;
    }
    struct Board *b = *board;

    b->renderer = renderer;
    b->rows = rows;
    b->columns = columns;
    b->piece_size = PIECE_SIZE * 2;
    b->rect.x = (PIECE_SIZE - BORDER_LEFT) * 2;
    b->rect.y = BORDER_HEIGHT * 2;

    if (!load_media_sheet(b->renderer, &b->image, "images/board.png",
                          PIECE_SIZE, PIECE_SIZE, &b->src_rects)) {
        return false;
    }

    if (!board_reset(b)) {
        return false;
    }

    return true;
}

void board_free(struct Board **board) {
    if (*board) {
        struct Board *b = *board;

        board_free_arrays(b);

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
        *board = NULL;

        printf("board clean.\n");
    }
}

bool board_calloc_arrays(struct Board *b) {
    b->front_array = calloc(b->rows, sizeof(unsigned *));
    if (!b->front_array) {
        fprintf(stderr, "Error in calloc of front array rows!\n");
        return false;
    }

    for (unsigned r = 0; r < b->rows; r++) {
        b->front_array[r] = calloc(b->columns, sizeof(unsigned));
        if (!b->front_array[r]) {
            fprintf(stderr, "Error in calloc of front array columns!\n");
            return false;
        }
    }

    return true;
}

void board_free_arrays(struct Board *b) {
    if (b->front_array) {
        for (unsigned r = 0; r < b->rows; r++) {
            if (b->front_array[r]) {
                free(b->front_array[r]);
            }
        }
        free(b->front_array);
        b->front_array = NULL;
    }
}

bool board_reset(struct Board *b) {

    board_free_arrays(b);

    if (!board_calloc_arrays(b)) {
        return false;
    }

    for (unsigned r = 0; r < b->rows; r++) {
        for (unsigned c = 0; c < b->columns; c++) {
            b->front_array[r][c] = 9;
        }
    }

    return true;
}

void board_draw(const struct Board *b) {
    SDL_FRect dest_rect = {0, 0, b->piece_size, b->piece_size};
    for (unsigned r = 0; r < b->rows; r++) {
        dest_rect.y = (float)r * dest_rect.h + b->rect.y;
        for (unsigned c = 0; c < b->columns; c++) {
            dest_rect.x = (float)c * dest_rect.w + b->rect.x;
            unsigned rect_index = b->front_array[r][c];
            SDL_RenderTexture(b->renderer, b->image, &b->src_rects[rect_index],
                              &dest_rect);
        }
    }
}
