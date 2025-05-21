#ifndef LOAD_MEDIA_H
#define LOAD_MEDIA_H

#include "main.h"

bool load_media_sheet(SDL_Renderer *renderer, SDL_Texture **image,
                      const char *file_path, int width, int height,
                      SDL_FRect **rects);

#endif
