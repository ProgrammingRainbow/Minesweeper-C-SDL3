#include "board.h"
#include "load_media.h"

void board_free_arrays(struct Board *b);
bool board_calloc_arrays(struct Board *b);
bool board_uncover(struct Board *b);
bool board_push_check(struct Board *b, int row, int column);
struct Node board_pop_check(struct Board *b);
void board_reveal(struct Board *b);
void board_check_won(struct Board *b);

bool board_new(struct Board **board, SDL_Renderer *renderer, unsigned rows,
               unsigned columns, int mine_count, float scale) {
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
    b->scale = scale;

    if (!load_media_sheet(b->renderer, &b->image, "images/board.png",
                          PIECE_SIZE, PIECE_SIZE, &b->src_rects)) {
        return false;
    }

    if (!board_reset(b, b->mine_count, true)) {
        return false;
    }

    board_set_scale(b, b->scale);

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
        fprintf(stderr, "Error in Calloc of front array of rows.\n");
        return false;
    }

    for (unsigned row = 0; row < b->rows; row++) {
        b->back_array[row] = calloc(b->columns, sizeof(unsigned));
        if (b->back_array[row] == NULL) {
            fprintf(stderr, "Error in Calloc of front array of columns.\n");
            return false;
        }
    }

    return true;
}

bool board_reset(struct Board *b, int mine_count, bool full_reset) {
    b->mine_count = mine_count;

    if (full_reset) {
        board_free_arrays(b);

        if (!board_calloc_arrays(b)) {
            return false;
        }

        for (unsigned row = 0; row < b->rows; row++) {
            for (unsigned column = 0; column < b->columns; column++) {
                b->front_array[row][column] = 9;
            }
        }
    } else {
        for (unsigned row = 0; row < b->rows; row++) {
            for (unsigned column = 0; column < b->columns; column++) {
                b->back_array[row][column] = 0;
                unsigned elem = b->front_array[row][column];
                b->front_array[row][column] =
                    (elem == 10 || elem == 11) ? elem : 9;
            }
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

    b->first_turn = true;
    b->game_state = GAME_PLAY;

    return true;
}

void board_set_scale(struct Board *b, float scale) {
    b->scale = scale;
    b->piece_size = PIECE_SIZE * b->scale;
    b->rect.x = (PIECE_SIZE - BORDER_LEFT) * b->scale;
    b->rect.y = BORDER_HEIGHT * b->scale;
    b->rect.w = (float)b->columns * b->piece_size;
    b->rect.h = (float)b->rows * b->piece_size;
}

void board_set_size(struct Board *b, unsigned rows, unsigned columns) {
    board_free_arrays(b);
    b->rows = rows;
    b->columns = columns;
    b->rect.w = (float)b->columns * b->piece_size;
    b->rect.h = (float)b->rows * b->piece_size;
}

void board_set_theme(struct Board *b, unsigned theme) { b->theme = theme * 16; }

enum GameState board_game_state(const struct Board *b) { return b->game_state; }

int board_mine_marked(const struct Board *b) { return b->mine_marked; }

bool board_is_pressed(const struct Board *b) {
    return b->left_pressed || b->right_pressed;
}

bool board_uncover(struct Board *b) {
    while (b->check_head) {
        struct Node pos = board_pop_check(b);

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

bool board_push_check(struct Board *b, int row, int column) {
    struct Node *node = calloc(1, sizeof(struct Node));
    if (!node) {
        fprintf(stderr, "Error in Calloc of Check Node.\n");
        return false;
    }

    node->row = row;
    node->column = column;

    node->next = b->check_head;
    b->check_head = node;

    return true;
}

struct Node board_pop_check(struct Board *b) {
    struct Node pos = {0};
    if (b->check_head) {
        pos.row = b->check_head->row;
        pos.column = b->check_head->column;

        struct Node *node = b->check_head;
        b->check_head = b->check_head->next;
        free(node);
    }

    return pos;
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
                if (b->back_array[row][column] != b->front_array[row][column]) {
                    return;
                }
            }
        }
    }

    b->game_state = GAME_WON;
}

void board_mouse_down(struct Board *b, float x, float y, Uint8 button) {
    if (button == SDL_BUTTON_LEFT) {
        b->left_pressed = false;
    } else if (button == SDL_BUTTON_RIGHT) {
        b->right_pressed = false;
    }

    if (x >= b->rect.x && x < b->rect.x + b->rect.w) {
        if (y >= b->rect.y && y < b->rect.y + b->rect.h) {
            int row = (int)((y - b->rect.y) / b->piece_size);
            int column = (int)((x - b->rect.x) / b->piece_size);
            if (button == SDL_BUTTON_LEFT) {
                if (b->front_array[row][column] == 9) {
                    b->left_pressed = true;
                }
            } else if (button == SDL_BUTTON_RIGHT) {
                if (b->front_array[row][column] > 8 &&
                    b->front_array[row][column] < 12) {
                    b->right_pressed = true;
                }
            }
        }
    }
}

bool board_mouse_up(struct Board *b, float x, float y, Uint8 button) {
    b->mine_marked = 0;

    if (button == SDL_BUTTON_LEFT) {
        if (!b->left_pressed) {
            return true;
        } else {
            b->left_pressed = false;
        }
    } else if (button == SDL_BUTTON_RIGHT) {
        if (!b->right_pressed) {
            return true;
        } else {
            b->right_pressed = false;
        }
    }

    if (x < b->rect.x || x >= b->rect.x + b->rect.w) {
        return true;
    }
    if (y < b->rect.y || y >= b->rect.y + b->rect.h) {
        return true;
    }

    int row = (int)((y - b->rect.y) / b->piece_size);
    int column = (int)((x - b->rect.x) / b->piece_size);

    if (button == SDL_BUTTON_LEFT) {
        if (b->front_array[row][column] == 9) {
            while (true) {
                if (b->back_array[row][column] == 13) {
                    b->front_array[row][column] = 14;
                    b->game_state = GAME_LOST;
                } else {
                    b->front_array[row][column] = b->back_array[row][column];
                    if (b->back_array[row][column] == 0) {
                        if (!board_push_check(b, row, column)) {
                            return false;
                        }
                        if (!board_uncover(b)) {
                            return false;
                        }
                    }
                    board_check_won(b);
                }
                if (b->first_turn && b->game_state != GAME_PLAY) {
                    if (!board_reset(b, b->mine_count, false)) {
                        return false;
                    }
                } else {
                    break;
                }
            }
            b->first_turn = false;

            if (b->game_state != GAME_PLAY) {
                board_reveal(b);
            }
        }
    }

    if (button == SDL_BUTTON_RIGHT) {
        if (b->front_array[row][column] == 9) {
            b->front_array[row][column]++;
            b->mine_marked = -1;
        } else if (b->front_array[row][column] == 10) {
            b->front_array[row][column]++;
            b->mine_marked = 1;
        } else if (b->front_array[row][column] == 11) {
            b->front_array[row][column] = 9;
        }
    }

    return true;
}

void board_draw(const struct Board *b) {
    SDL_FRect dest_rect = {0, 0, b->piece_size, b->piece_size};
    for (unsigned row = 0; row < b->rows; row++) {
        dest_rect.y = (float)row * b->piece_size + b->rect.y;
        for (unsigned column = 0; column < b->columns; column++) {
            dest_rect.x = (float)column * b->piece_size + b->rect.x;
            unsigned index = b->front_array[row][column];
            SDL_RenderTexture(b->renderer, b->image,
                              &b->src_rects[index + b->theme], &dest_rect);
        }
    }
}
