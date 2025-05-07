#include "board.h"
#include "load_media.h"

void board_free_arrays(struct Board *b);
bool board_calloc_arrays(struct Board *b);

bool board_new(struct Board **board, SDL_Renderer *renderer, unsigned rows,
               unsigned columns) {
    *board = calloc(1, sizeof(struct Board));
    if (*board == NULL) {
        fprintf(stderr, "Error in Calloc of New Board.\n");
        return false;
    }
    struct Board *b = *board;

    b->renderer = renderer;
    b->rows = rows;
    b->columns = columns;

    b->piece_size = PIECE_SIZE * 2;
    b->top_offset = BORDER_HEIGHT * 2;
    b->left_offset = (PIECE_SIZE - BORDER_LEFT) * 2;

    if (!load_media_sheet(b->renderer, &b->image, "images/board.png",
                          PIECE_SIZE, PIECE_SIZE, &b->src_rects)) {
        return false;
    }

    if (!board_reset(b)) {
        return false;
    }

    return true;
}

void board_free_arrays(struct Board *b) {
    if (b->front_array) {
        for (unsigned row = 0; row < b->rows; row++) {
            if (b->front_array[row]) {
                free(b->front_array[row]);
                b->front_array[row] = NULL;
            }
        }
        free(b->front_array);
        b->front_array = NULL;
    }
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

        printf("Free Board.\n");
    }
}

bool board_calloc_arrays(struct Board *b) {
    b->front_array = calloc(b->rows, sizeof(unsigned *));
    if (b->front_array == NULL) {
        fprintf(stderr, "Error in Calloc of front array of rows.\n");
        return false;
    }

    for (unsigned row = 0; row < b->rows; row++) {
        b->front_array[row] = calloc(b->columns, sizeof(unsigned));
        if (b->front_array[row] == NULL) {
            fprintf(stderr, "Error in Calloc of front array of columns.\n");
            return false;
        }
    }

    return true;
}

bool board_reset(struct Board *b) {
    board_free_arrays(b);

    if (!board_calloc_arrays(b)) {
        return false;
    }

    for (unsigned row = 0; row < b->rows; row++) {
        for (unsigned column = 0; column < b->columns; column++) {
            b->front_array[row][column] = 9;
        }
    }

    return true;
}

void board_draw(const struct Board *b) {
    SDL_FRect dest_rect = {0, 0, b->piece_size, b->piece_size};
    for (unsigned row = 0; row < b->rows; row++) {
        dest_rect.y = (float)row * b->piece_size + b->top_offset;
        for (unsigned column = 0; column < b->columns; column++) {
            dest_rect.x = (float)column * b->piece_size + b->left_offset;
            unsigned index = b->front_array[row][column];
            SDL_RenderTexture(b->renderer, b->image, &b->src_rects[index],
                              &dest_rect);
        }
    }
}
