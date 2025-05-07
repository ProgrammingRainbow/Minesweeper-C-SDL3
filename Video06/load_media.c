#include "load_media.h"

bool load_media_sheet(SDL_Renderer *renderer, SDL_Texture **image,
                      const char *file_path, int width, int height,
                      SDL_FRect **src_rects) {
    if (*image) {
        SDL_DestroyTexture(*image);
        *image = NULL;
    }
    if (*src_rects) {
        free(*src_rects);
        *src_rects = NULL;
    }

    *image = IMG_LoadTexture(renderer, file_path);
    if (!*image) {
        fprintf(stderr, "Error loading Texture: %s\n", SDL_GetError());
        return false;
    }

    int rows = (*image)->h / height;
    int columns = (*image)->w / width;

    if (!SDL_SetTextureScaleMode(*image, SDL_SCALEMODE_NEAREST)) {
        fprintf(stderr, "Error setting Texture scale mode: %s\n",
                SDL_GetError());
        return false;
    }

    *src_rects = calloc((size_t)(rows * columns), sizeof(SDL_FRect));
    if (*src_rects == NULL) {
        fprintf(stderr, "Error in Calloc of src_rects.\n");
        return false;
    }

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            int index = row * columns + column;
            (*src_rects)[index].x = (float)(column * width);
            (*src_rects)[index].y = (float)(row * height);
            (*src_rects)[index].w = (float)width;
            (*src_rects)[index].h = (float)height;
        }
    }

    return true;
}
