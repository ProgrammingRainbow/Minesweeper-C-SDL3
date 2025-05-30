#include "game.h"
#include "init_sdl.h"

bool game_reset(struct Game *g);
void game_mouse_down(struct Game *g, float x, float y, Uint8 button);
bool game_mouse_up(struct Game *g, float x, float y, Uint8 button);
bool game_events(struct Game *g);
void game_update(struct Game *g);
void game_draw(const struct Game *g);

bool game_new(struct Game **game) {
    *game = calloc(1, sizeof(struct Game));
    if (*game == NULL) {
        fprintf(stderr, "Error in Calloc of New Game.\n");
        return false;
    }
    struct Game *g = *game;

    g->is_running = true;
    g->is_playing = true;
    g->rows = 9;
    g->columns = 9;
    g->mine_count = 8;

    if (!game_init_sdl(g)) {
        return false;
    }

    srand((unsigned)time(NULL));

    if (!border_new(&g->border, g->renderer, g->rows, g->columns)) {
        return false;
    }
    if (!board_new(&g->board, g->renderer, g->rows, g->columns,
                   g->mine_count)) {
        return false;
    }
    if (!mines_new(&g->mines, g->renderer, g->mine_count)) {
        return false;
    }
    if (!clock_new(&g->clock, g->renderer, g->columns)) {
        return false;
    }
    if (!face_new(&g->face, g->renderer, g->columns)) {
        return false;
    }

    return true;
}

void game_free(struct Game **game) {
    if (*game) {
        struct Game *g = *game;

        if (g->face) {
            face_free(&g->face);
        }
        if (g->clock) {
            clock_free(&g->clock);
        }
        if (g->mines) {
            mines_free(&g->mines);
        }
        if (g->board) {
            board_free(&g->board);
        }
        if (g->border) {
            border_free(&g->border);
        }

        if (g->renderer) {
            SDL_DestroyRenderer(g->renderer);
            g->renderer = NULL;
        }
        if (g->window) {
            SDL_DestroyWindow(g->window);
            g->window = NULL;
        }

        SDL_Quit();

        free(g);
        g = NULL;
        *game = NULL;

        printf("All clean!\n");
    }
}

bool game_reset(struct Game *g) {
    if (!board_reset(g->board, g->mine_count, true)) {
        return false;
    }

    face_default(g->face);
    clock_reset(g->clock);
    mines_reset(g->mines, g->mine_count);
    g->is_playing = true;

    return true;
}

void game_mouse_down(struct Game *g, float x, float y, Uint8 button) {
    if (button == SDL_BUTTON_LEFT) {
        face_mouse_click(g->face, x, y, true);
    }

    if (g->is_playing) {
        board_mouse_down(g->board, x, y, button);
        if (board_is_pressed(g->board)) {
            face_question(g->face);
        }
    }
}

bool game_mouse_up(struct Game *g, float x, float y, Uint8 button) {
    if (button == SDL_BUTTON_LEFT) {
        if (face_mouse_click(g->face, x, y, false)) {
            if (!game_reset(g)) {
                return false;
            }
        }
    }

    if (g->is_playing) {
        if (!board_mouse_up(g->board, x, y, button)) {
            return false;
        }

        if (board_mine_marked(g->board) == 1) {
            mines_increment(g->mines);
        } else if (board_mine_marked(g->board) == -1) {
            mines_decrement(g->mines);
        }
    }

    if (board_game_state(g->board) == GAME_WON) {
        face_won(g->face);
        g->is_playing = false;
    } else if (board_game_state(g->board) == GAME_LOST) {
        face_lost(g->face);
        g->is_playing = false;
    } else {
        face_default(g->face);
    }

    return true;
}

bool game_events(struct Game *g) {
    while (SDL_PollEvent(&g->event)) {
        switch (g->event.type) {
        case SDL_EVENT_QUIT:
            g->is_running = false;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            game_mouse_down(g, g->event.button.x, g->event.button.y,
                            g->event.button.button);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (!game_mouse_up(g, g->event.button.x, g->event.button.y,
                               g->event.button.button)) {
                return false;
            }
            break;
        case SDL_EVENT_KEY_DOWN:
            switch (g->event.key.scancode) {
            case SDL_SCANCODE_ESCAPE:
                g->is_running = false;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    return true;
}

void game_update(struct Game *g) {
    if (g->is_playing) {
        clock_update(g->clock);
    }
}

void game_draw(const struct Game *g) {
    SDL_RenderClear(g->renderer);

    border_draw(g->border);
    board_draw(g->board);
    mines_draw(g->mines);
    clock_draw(g->clock);
    face_draw(g->face);

    SDL_RenderPresent(g->renderer);
}

bool game_run(struct Game *g) {
    while (g->is_running) {
        if (!game_events(g)) {
            return false;
        }

        game_update(g);

        game_draw(g);

        SDL_Delay(16);
    }

    return true;
}
