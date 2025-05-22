#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SDL_FLAGS SDL_INIT_VIDEO
#define IMG_FLAGS IMG_INIT_PNG

#define WINDOW_TITLE "Minesweeper"
#define WINDOW_WIDTH 328
#define WINDOW_HEIGHT 414

#define PIECE_SIZE 16
#define BORDER_HEIGHT 55
#define BORDER_LEFT 4
#define BORDER_RIGHT 8
#define BORDER_BOTTOM 8

#define DIGIT_BACK_WIDTH 41
#define DIGIT_BACK_HEIGHT 25
#define DIGIT_BACK_TOP 15
#define DIGIT_BACK_LEFT 15
#define DIGIT_BACK_RIGHT 3

#define DIGIT_WIDTH 13
#define DIGIT_HEIGHT 23

#define FACE_SIZE 26
#define FACE_TOP 15

#endif
