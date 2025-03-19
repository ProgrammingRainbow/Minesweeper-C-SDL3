#include "board.h"
#include "load_media.h"

bool board_calloc_arrays(struct Board *b);
void board_free_arrays(struct Board *b);
bool board_push_check(struct Board *b, int row, int column);
struct Pos board_pop_check(struct Board *b);
bool board_uncover(struct Board *b);
void board_reveal(struct Board *b);
void board_check_won(struct Board *b);

bool board_new(struct Board **board, SDL_Renderer *renderer, unsigned rows,
               unsigned columns, float scale, int mine_count) {
    *board = calloc(1, sizeof(struct Board));
    if (!*board) {
        fprintf(stderr, "Error in calloc of new board.\n");
        return false;
    }
    struct Board *b = *board;

    b->renderer = renderer;
    b->rows = rows;
    b->columns = columns;
    b->scale = scale;
    b->mine_count = mine_count;
    b->game_status = 0;
    b->first_turn = true;

    if (!load_media_sheet(b->renderer, &b->image, "images/board.png",
                          PIECE_SIZE, PIECE_SIZE, &b->src_rects)) {
        return false;
    }

    board_set_scale(b, b->scale);

    if (!board_reset(b, b->mine_count, true)) {
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

        b = NULL;

        free(*board);
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

    b->back_array = calloc(b->rows, sizeof(unsigned *));
    if (!b->back_array) {
        fprintf(stderr, "Error in calloc of back array rows!\n");
        return false;
    }

    for (unsigned r = 0; r < b->rows; r++) {
        b->back_array[r] = calloc(b->columns, sizeof(unsigned));
        if (!b->back_array[r]) {
            fprintf(stderr, "Error in calloc of back array columns!\n");
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

    if (b->back_array) {
        for (unsigned r = 0; r < b->rows; r++) {
            if (b->back_array[r]) {
                free(b->back_array[r]);
            }
        }
        free(b->back_array);
        b->back_array = NULL;
    }
}

bool board_reset(struct Board *b, int mine_count, bool full_reset) {
    b->mine_count = mine_count;

    if (full_reset) {
        board_free_arrays(b);

        if (!board_calloc_arrays(b)) {
            return false;
        }

        for (unsigned r = 0; r < b->rows; r++) {
            for (unsigned c = 0; c < b->columns; c++) {
                b->front_array[r][c] = 9;
            }
        }
    } else {
        for (unsigned r = 0; r < b->rows; r++) {
            for (unsigned c = 0; c < b->columns; c++) {
                unsigned elem = b->front_array[r][c];
                b->front_array[r][c] =
                    ((elem == 10) || (elem == 11)) ? elem : 9;
            }
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

    b->game_status = 0;
    b->first_turn = true;
    b->mines_marked = 0;

    return true;
}

int board_game_status(const struct Board *b) { return b->game_status; }

int board_mines_marked(const struct Board *b) { return b->mines_marked; }

bool board_is_pressed(const struct Board *b) { return b->pressed; }

bool board_push_check(struct Board *b, int row, int column) {
    struct Node *node = calloc(1, sizeof(struct Node));
    if (!node) {
        fprintf(stderr, "Error in calloc of check Node.\n");
        return false;
    }

    node->row = row;
    node->column = column;

    node->next = b->check_head;
    b->check_head = node;

    return true;
}

struct Pos board_pop_check(struct Board *b) {
    struct Pos pos;
    if (b->check_head) {
        pos.row = b->check_head->row;
        pos.column = b->check_head->column;
    } else {
        pos.row = 0;
        pos.column = 0;
    }

    struct Node *node = b->check_head;
    b->check_head = b->check_head->next;
    free(node);

    return pos;
}

bool board_uncover(struct Board *b) {
    while (b->check_head) {
        struct Pos pos = board_pop_check(b);

        for (int r = pos.row - 1; r < pos.row + 2; r++) {
            if (r < 0 || r >= (int)b->rows) {
                continue;
            }
            for (int c = pos.column - 1; c < pos.column + 2; c++) {
                if (c < 0 || c >= (int)b->columns) {
                    continue;
                }
                if (b->front_array[r][c] == 9) {
                    b->front_array[r][c] = b->back_array[r][c];
                    if (b->front_array[r][c] == 0) {
                        if (!board_push_check(b, r, c)) {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

void board_reveal(struct Board *b) {
    for (unsigned row = 0; row < b->rows; row++) {
        for (unsigned column = 0; column < b->columns; column++) {
            if (b->front_array[row][column] == 9 &&
                b->back_array[row][column] == 13) {
                b->front_array[row][column] = 13;
            }
            if (b->front_array[row][column] == 10 &&
                b->back_array[row][column] != 13) {
                b->front_array[row][column] = 15;
            }
        }
    }
}

void board_check_won(struct Board *b) {
    for (unsigned row = 0; row < b->rows; row++) {
        for (unsigned column = 0; column < b->columns; column++) {
            if (b->back_array[row][column] != 13) {
                if (b->front_array[row][column] != b->back_array[row][column]) {
                    return;
                }
            }
        }
    }
    b->game_status = 1;
}

void board_mouse_down(struct Board *b, float x, float y, Uint8 button) {
    b->pressed = false;

    if (x < b->rect.x || x > b->rect.x + b->rect.w) {
        return;
    }
    if (y < b->rect.y || y > b->rect.y + b->rect.h) {
        return;
    }

    int row = (int)((y - b->rect.y) / b->piece_size);
    int column = (int)((x - b->rect.x) / b->piece_size);

    if (button == SDL_BUTTON_LEFT) {
        if (b->front_array[row][column] == 9) {
            b->pressed = true;
        }
    } else if (button == SDL_BUTTON_RIGHT) {
        if (b->front_array[row][column] > 8 &&
            b->front_array[row][column] < 12) {
            b->pressed = true;
        }
    }
}

bool board_mouse_up(struct Board *b, float x, float y, Uint8 button) {
    if (!b->pressed) {
        return true;
    }
    b->pressed = false;
    b->mines_marked = 0;

    if (x < b->rect.x || x > b->rect.x + b->rect.w) {
        return true;
    }
    if (y < b->rect.y || y > b->rect.y + b->rect.h) {
        return true;
    }

    int row = (int)((y - b->rect.y) / b->piece_size);
    int column = (int)((x - b->rect.x) / b->piece_size);

    if (button == SDL_BUTTON_LEFT) {
        if (b->front_array[row][column] == 9) {
            while (true) {
                if (b->back_array[row][column] == 13) {
                    b->game_status = -1;
                    b->front_array[row][column] = 14;
                } else {
                    b->front_array[row][column] = b->back_array[row][column];
                    if (b->front_array[row][column] == 0) {
                        if (!board_push_check(b, row, column)) {
                            return false;
                        }
                        if (!board_uncover(b)) {
                            return false;
                        }
                    }
                    board_check_won(b);
                }
                if (b->first_turn && b->game_status != 0) {
                    if (!board_reset(b, b->mine_count, false)) {
                        return false;
                    }
                } else {
                    break;
                }
            }
            b->first_turn = false;

            if (b->game_status != 0) {
                board_reveal(b);
            }

            return true;
        }
    }

    if (button == SDL_BUTTON_RIGHT) {
        if (b->front_array[row][column] == 9) {
            b->front_array[row][column]++;
            b->mines_marked = -1;
        } else if (b->front_array[row][column] == 10) {
            b->front_array[row][column]++;
            b->mines_marked = 1;
        } else if (b->front_array[row][column] == 11) {
            b->front_array[row][column] = 9;
        }
    }

    return true;
}

void board_set_scale(struct Board *b, float scale) {
    b->scale = scale;
    b->piece_size = PIECE_SIZE * b->scale;
    b->rect.x = (PIECE_SIZE - BORDER_LEFT) * b->scale;
    b->rect.y = BORDER_HEIGHT * b->scale;
    b->rect.w = (float)b->columns * b->piece_size + b->rect.x;
    b->rect.h = (float)b->rows * b->piece_size + b->rect.y;
}

void board_set_theme(struct Board *b, unsigned theme) { b->theme = theme * 16; }

void board_set_size(struct Board *b, unsigned rows, unsigned columns) {
    board_free_arrays(b);
    b->rows = rows;
    b->columns = columns;
    b->rect.w = (float)b->columns * b->piece_size + b->rect.x;
    b->rect.h = (float)b->rows * b->piece_size + b->rect.y;
}

void board_draw(const struct Board *b) {
    SDL_FRect dest_rect = {0, 0, b->piece_size, b->piece_size};
    for (unsigned r = 0; r < b->rows; r++) {
        dest_rect.y = (float)r * dest_rect.h + b->rect.y;
        for (unsigned c = 0; c < b->columns; c++) {
            dest_rect.x = (float)c * dest_rect.w + b->rect.x;
            unsigned rect_index = b->front_array[r][c];
            SDL_RenderTexture(b->renderer, b->image,
                              &b->src_rects[rect_index + b->theme], &dest_rect);
        }
    }
}
