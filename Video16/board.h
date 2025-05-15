#ifndef BOARD_H
#define BOARD_H

#include "main.h"

struct Board {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_FRect *src_rects;
        unsigned **front_array;
        unsigned **back_array;
        SDL_FRect rect;
        unsigned rows;
        unsigned columns;
        float scale;
        float piece_size;
        int mine_count;
        struct Node *check_head;
        bool pressed;
        int mines_marked;
        int game_status;
        bool first_turn;
        unsigned theme;
};

struct Node {
        struct Node *next;
        int row;
        int column;
};

struct Pos {
        int row;
        int column;
};

bool board_new(struct Board **board, SDL_Renderer *renderer, unsigned rows,
               unsigned columns, float scale, int mine_count);
void board_free(struct Board **board);
bool board_reset(struct Board *b, int mine_count, bool full_reset);
int board_game_status(const struct Board *b);
int board_mines_marked(const struct Board *b);
bool board_is_pressed(const struct Board *b);
void board_mouse_down(struct Board *b, float x, float y, Uint8 button);
bool board_mouse_up(struct Board *b, float x, float y, Uint8 button);
void board_set_scale(struct Board *b, float scale);
void board_set_theme(struct Board *b, unsigned theme);
void board_set_size(struct Board *b, unsigned rows, unsigned columns);
void board_draw(const struct Board *b);

#endif
