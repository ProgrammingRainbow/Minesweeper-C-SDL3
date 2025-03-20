#include "game.h"
#include "init_sdl.h"

bool game_new(struct Game **game) {
    *game = calloc(1, sizeof(struct Game));
    if (*game == NULL) {
        fprintf(stderr, "Error in Calloc of New Game.\n");
        return false;
    }
    struct Game *g = *game;

    if (!game_init_sdl(g)) {
        return false;
    }

    g->is_running = true;

    return true;
}

void game_free(struct Game **game) {
    if (*game) {
        struct Game *g = *game;

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

void game_run(struct Game *g) {
    SDL_SetRenderDrawColor(g->renderer, 128, 0, 92, 255);
    SDL_RenderClear(g->renderer);

    // Draw

    SDL_RenderPresent(g->renderer);

    SDL_Delay(5000);
}
