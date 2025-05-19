#ifndef BOARD_H
#define BOARD_H

#include "main.h"

enum GameState { GAME_PLAY, GAME_WON, GAME_LOST };

struct Board {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_FRect *src_rects;
        unsigned **front_array;
        unsigned **back_array;
        unsigned rows;
        unsigned columns;
        float piece_size;
        int mine_count;
        SDL_FRect rect;
        bool left_pressed;
        bool right_pressed;
        struct Node *check_head;
        enum GameState game_state;
        bool first_turn;
        int mine_marked;
        float scale;
};

struct Node {
        struct Node *next;
        int row;
        int column;
};

bool board_new(struct Board **board, SDL_Renderer *renderer, unsigned rows,
               unsigned columns, int mine_count, float scale);
void board_set_scale(struct Board *b, float scale);
void board_free(struct Board **board);
bool board_reset(struct Board *b, int mine_count, bool full_reset);
enum GameState board_game_state(const struct Board *b);
int board_mine_marked(const struct Board *b);
bool board_is_pressed(const struct Board *b);
void board_mouse_down(struct Board *b, float x, float y, Uint8 button);
bool board_mouse_up(struct Board *b, float x, float y, Uint8 button);
void board_draw(const struct Board *b);

#endif
