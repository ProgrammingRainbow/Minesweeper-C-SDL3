#include "board.h"
#include "load_media.h"

void board_free_arrays(struct Board *b);
bool board_calloc_arrays(struct Board *b);

bool board_new(struct Board **board, SDL_Renderer *renderer, unsigned rows,
               unsigned columns, int mine_count) {
    *board = calloc(1, sizeof(struct Board));
    if (*board == NULL) {
        fprintf(stderr, "Error in Calloc of New Board.\n");
        return false;
    }
    struct Board *b = *board;

    b->renderer = renderer;
    b->rows = rows;
    b->columns = columns;
    b->mine_count = mine_count;

    b->piece_size = PIECE_SIZE * 2;
    b->top_offset = BORDER_HEIGHT * 2;
    b->left_offset = (PIECE_SIZE - BORDER_LEFT) * 2;

    if (!load_media_sheet(b->renderer, &b->image, "images/board.png",
                          PIECE_SIZE, PIECE_SIZE, &b->src_rects)) {
        return false;
    }

    if (!board_reset(b, b->mine_count)) {
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

    if (b->back_array) {
        for (unsigned row = 0; row < b->rows; row++) {
            if (b->back_array[row]) {
                free(b->back_array[row]);
                b->back_array[row] = NULL;
            }
        }
        free(b->back_array);
        b->back_array = NULL;
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

    b->back_array = calloc(b->rows, sizeof(unsigned *));
    if (b->back_array == NULL) {
        fprintf(stderr, "Error in Calloc of back array of rows.\n");
        return false;
    }

    for (unsigned row = 0; row < b->rows; row++) {
        b->back_array[row] = calloc(b->columns, sizeof(unsigned));
        if (b->back_array[row] == NULL) {
            fprintf(stderr, "Error in Calloc of back array of columns.\n");
            return false;
        }
    }

    return true;
}

bool board_reset(struct Board *b, int mine_count) {
    board_free_arrays(b);
    b->mine_count = mine_count;

    if (!board_calloc_arrays(b)) {
        return false;
    }

    for (unsigned row = 0; row < b->rows; row++) {
        for (unsigned column = 0; column < b->columns; column++) {
            b->front_array[row][column] = 9;
        }
    }

    for (unsigned r = 0; r < b->rows; r++) {
        for (unsigned c = 0; c < b->columns; c++) {
            b->back_array[r][c] = 0;
        }
    }

    int add_mines = b->mine_count;
    while (add_mines > 0) {
        int r = rand() % (int)b->rows;
        int c = rand() % (int)b->columns;
        if (!b->back_array[r][c]) {
            b->back_array[r][c] = 13;
            add_mines--;
        }
    }

    for (int row = 0; row < (int)b->rows; row++) {
        for (int column = 0; column < (int)b->columns; column++) {
            unsigned close_mines = 0;
            if (b->back_array[row][column] == 13) {
                continue;
            }
            for (int r = row - 1; r < row + 2; r++) {
                if (r >= 0 && r < (int)b->rows) {
                    for (int c = column - 1; c < column + 2; c++) {
                        if (c >= 0 && c < (int)b->columns) {
                            if (b->back_array[r][c] == 13) {
                                close_mines++;
                            }
                        }
                    }
                }
            }
            b->back_array[row][column] = close_mines;
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
            unsigned index = b->back_array[row][column];
            SDL_RenderTexture(b->renderer, b->image, &b->src_rects[index],
                              &dest_rect);
        }
    }
}
