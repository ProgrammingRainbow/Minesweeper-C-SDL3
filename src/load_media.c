#include "load_media.h"

bool load_media_sheet(SDL_Renderer *renderer, SDL_Texture **image,
                      const char *file_path, int width, int height,
                      SDL_FRect **rects) {

    SDL_Surface *source_surf = IMG_Load(file_path);
    if (!source_surf) {
        fprintf(stderr, "Error creating the source surface: %s\n",
                SDL_GetError());
        return false;
    }

    int max_rows = source_surf->h / height;
    int max_columns = source_surf->w / width;
    size_t rects_length = (size_t)(max_rows * max_columns);

    *image = SDL_CreateTextureFromSurface(renderer, source_surf);
    SDL_DestroySurface(source_surf);
    if (!*image) {
        fprintf(stderr, "Error creating a image texture: %s\n", SDL_GetError());
        return false;
    }

    if (!SDL_SetTextureScaleMode(*image, SDL_SCALEMODE_NEAREST)) {
        fprintf(stderr, "Error setting texture scale mode: %s\n",
                SDL_GetError());
        return false;
    }

    if (*rects) {
        free(*rects);
        *rects = NULL;
    }

    *rects = calloc(rects_length, sizeof(SDL_Rect));
    if (!*rects) {
        fprintf(stderr, "Error in calloc of image array.\n");
        return false;
    }

    for (int row = 0; row < max_rows; row++) {
        for (int column = 0; column < max_columns; column++) {
            int index = row * max_columns + column;
            (*rects)[index].x = (float)(column * width);
            (*rects)[index].y = (float)(row * height);
            (*rects)[index].w = (float)width;
            (*rects)[index].h = (float)height;
        }
    }

    return true;
}
