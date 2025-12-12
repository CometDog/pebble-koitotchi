#pragma once

#include <pebble.h>

#ifdef PBL_COLOR
#define FULL_WINDOW_BACKGROUND_COLOR GColorPastelYellow
#define PLAY_AREA_BACKGROUND_COLOR GColorCeleste
#define PLAY_AREA_GRID_0 GColorVividCerulean
#define PLAY_AREA_GRID_1 GColorBlack
#else
#define FULL_WINDOW_BACKGROUND_COLOR GColorLightGray
#define PLAY_AREA_BACKGROUND_COLOR GColorBlack
#define PLAY_AREA_GRID_0 GColorWhite
#define PLAY_AREA_GRID_1 GColorBlack
#endif

#define PLAY_AREA_COLUMNS 32
#define PLAY_AREA_ROWS 16

#ifdef PBL_PLATFORM_EMERY
#define PLAY_AREA_TOP 66
#define PLAY_AREA_LEFT 4
#define PLAY_AREA_CELL_SIZE 5
#else
#define PLAY_AREA_TOP 52
#define PLAY_AREA_LEFT 8
#define PLAY_AREA_CELL_SIZE 3
#endif

#define PLAY_AREA s_play_area_state[PLAY_AREA_COLUMNS * PLAY_AREA_ROWS] = {0};