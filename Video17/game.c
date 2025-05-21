#include "game.h"
#include "init_sdl.h"

bool game_create_string(char **game_str, const char *new_str);
void game_set_title(struct Game *g);
bool game_reset(struct Game *g);
void game_set_scale(struct Game *g);
void game_toggel_scale(struct Game *g);
void game_set_theme(struct Game *g, unsigned theme);
bool game_set_difficulty(struct Game *g, double difficulty,
                         const char *diff_str);
bool game_set_size(struct Game *g, unsigned rows, unsigned columns, float scale,
                   const char *size_str);
void game_mouse_down(struct Game *g, float x, float y, Uint8 button);
bool game_mouse_up(struct Game *g, float x, float y, Uint8 button);
bool game_events(struct Game *g);
void game_update(struct Game *g);
void game_draw(const struct Game *g);

bool game_new(struct Game **game) {
    *game = calloc(1, sizeof(struct Game));
    if (*game == NULL) {
        fprintf(stderr, "Error in calloc of new game.\n");
        return false;
    }
    struct Game *g = *game;

    g->is_running = true;
    g->is_playing = true;
    g->rows = 9;
    g->columns = 9;
    g->scale = 2;
    g->mine_count = 8;
    g->difficulty = 0.1;

    if (!game_init_sdl(g)) {
        return false;
    }

    if (!border_new(&g->border, g->renderer, g->rows, g->columns, g->scale)) {
        return false;
    }

    if (!board_new(&g->board, g->renderer, g->rows, g->columns, g->scale,
                   g->mine_count)) {
        return false;
    }

    if (!mines_new(&g->mines, g->renderer, g->scale, g->mine_count)) {
        return false;
    }

    if (!clock_new(&g->clock, g->renderer, g->columns, g->scale)) {
        return false;
    }

    if (!face_new(&g->face, g->renderer, g->columns, g->scale)) {
        return false;
    }

    if (!game_create_string(&g->diff_str, "Easy")) {
        return false;
    }

    if (!game_create_string(&g->size_str, "Tiny")) {
        return false;
    }

    game_set_title(g);

    return true;
}

void game_free(struct Game **game) {
    if (*game) {
        struct Game *g = *game;

        border_free(&g->border);
        board_free(&g->board);
        mines_free(&g->mines);
        clock_free(&g->clock);
        face_free(&g->face);

        if (g->diff_str) {
            free(g->diff_str);
            g->diff_str = NULL;
        }

        if (g->size_str) {
            free(g->size_str);
            g->size_str = NULL;
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

        g = NULL;

        free(*game);
        *game = NULL;

        printf("all clean!\n");
    }
}

bool game_create_string(char **game_str, const char *new_str) {
    if (*game_str) {
        free(*game_str);
        *game_str = NULL;
    }

    size_t length = (size_t)(snprintf(NULL, 0, "%s", new_str) + 1);

    *game_str = calloc(1, sizeof(char) * length);
    if (!*game_str) {
        fprintf(stderr, "Error in calloc of difficulty/size string.\n");
        return false;
    }

    snprintf(*game_str, length, "%s", new_str);

    return true;
}

void game_set_title(struct Game *g) {
    int length = snprintf(NULL, 0, "%s - %s - %s", WINDOW_TITLE, g->size_str,
                          g->diff_str) +
                 1;
    char title_str[length];

    snprintf(title_str, (size_t)length, "%s - %s - %s", WINDOW_TITLE,
             g->size_str, g->diff_str);

    SDL_SetWindowTitle(g->window, title_str);
}

bool game_reset(struct Game *g) {
    g->mine_count = (int)((double)(g->rows * g->columns) * g->difficulty);

    if (!board_reset(g->board, g->mine_count, true)) {
        return false;
    }

    mines_reset(g->mines, g->mine_count);
    clock_reset(g->clock);
    face_default(g->face);

    game_set_title(g);

    g->is_playing = true;

    return true;
}

void game_set_scale(struct Game *g) {
    border_set_scale(g->border, g->scale);
    board_set_scale(g->board, g->scale);
    mines_set_scale(g->mines, g->scale);
    clock_set_scale(g->clock, g->scale);
    face_set_scale(g->face, g->scale);

    int window_width = (int)((PIECE_SIZE * ((float)g->columns + 1) -
                              BORDER_LEFT + BORDER_RIGHT) *
                             g->scale);
    int window_height =
        (int)((PIECE_SIZE * ((float)g->rows) + BORDER_HEIGHT + BORDER_BOTTOM) *
              g->scale);
    SDL_SetWindowSize(g->window, window_width, window_height);
    SDL_SetWindowPosition(g->window, SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);
}

void game_toggel_scale(struct Game *g) {
    g->scale = (g->scale < 3) ? g->scale + 1 : 1;
    game_set_scale(g);
}

void game_set_theme(struct Game *g, unsigned theme) {
    unsigned binary_theme = (theme < 6) ? 0 : 1;
    unsigned face_theme = (theme < 3) ? 0 : (theme < 6) ? 1 : 2;
    board_set_theme(g->board, theme);
    border_set_theme(g->border, binary_theme);
    mines_set_theme(g->mines, binary_theme);
    clock_set_theme(g->clock, binary_theme);
    face_set_theme(g->face, face_theme);
}

bool game_set_size(struct Game *g, unsigned rows, unsigned columns, float scale,
                   const char *size_str) {
    g->rows = rows;
    g->columns = columns;
    g->scale = scale;

    if (!game_create_string(&g->size_str, size_str)) {
        return false;
    }

    border_set_size(g->border, g->rows, g->columns);
    board_set_size(g->board, g->rows, g->columns);
    clock_set_size(g->clock, g->columns);
    face_set_size(g->face, g->columns);

    game_set_scale(g);

    if (!game_reset(g)) {
        return false;
    }

    return true;
}

bool game_set_difficulty(struct Game *g, double difficulty,
                         const char *diff_str) {
    g->difficulty = difficulty;

    if (!game_create_string(&g->diff_str, diff_str)) {
        return false;
    }

    if (!game_reset(g)) {
        return false;
    }

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

        if (board_mines_marked(g->board) == 1) {
            mines_increment(g->mines);
        } else if (board_mines_marked(g->board) == -1) {
            mines_decrement(g->mines);
        }

        if (board_game_status(g->board) == 1) {
            face_won(g->face);
            g->is_playing = false;
        } else if (board_game_status(g->board) == -1) {
            face_lost(g->face);
            g->is_playing = false;
        } else {
            face_default(g->face);
        }
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
            case SDL_SCANCODE_B:
                game_toggel_scale(g);
                break;
            case SDL_SCANCODE_N:
                if (!game_reset(g)) {
                    return false;
                }
                break;
            case SDL_SCANCODE_1:
                game_set_theme(g, 0);
                break;
            case SDL_SCANCODE_2:
                game_set_theme(g, 1);
                break;
            case SDL_SCANCODE_3:
                game_set_theme(g, 2);
                break;
            case SDL_SCANCODE_4:
                game_set_theme(g, 3);
                break;
            case SDL_SCANCODE_5:
                game_set_theme(g, 4);
                break;
            case SDL_SCANCODE_6:
                game_set_theme(g, 5);
                break;
            case SDL_SCANCODE_7:
                game_set_theme(g, 6);
                break;
            case SDL_SCANCODE_8:
                game_set_theme(g, 7);
                break;
            case SDL_SCANCODE_A:
                if (!game_set_difficulty(g, 0.1, "Easy"))
                    return false;
                break;
            case SDL_SCANCODE_S:
                if (!game_set_difficulty(g, 0.133, "Medium"))
                    return false;
                break;
            case SDL_SCANCODE_D:
                if (!game_set_difficulty(g, 0.166, "Hard"))
                    return false;
                break;
            case SDL_SCANCODE_F:
                if (!game_set_difficulty(g, 0.2, "Very Hard"))
                    return false;
                break;
            case SDL_SCANCODE_Q:
                if (!game_set_size(g, 9, 9, 2, "Tiny"))
                    return false;
                break;
            case SDL_SCANCODE_W:
                if (!game_set_size(g, 16, 16, 2, "Small"))
                    return false;
                break;
            case SDL_SCANCODE_E:
                if (!game_set_size(g, 16, 30, 2, "Medium"))
                    return false;
                break;
            case SDL_SCANCODE_R:
                if (!game_set_size(g, 20, 40, 2, "Large"))
                    return false;
                break;
            case SDL_SCANCODE_T:
                if (!game_set_size(g, 40, 80, 1, "Huge"))
                    return false;
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

        game_draw(g);

        game_update(g);

        SDL_Delay(16);
    }

    return true;
}
