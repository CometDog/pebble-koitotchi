#pragma once

#include <pebble.h>

typedef struct
{
    uint8_t row;
    uint8_t column;
    uint8_t row_count;
    uint8_t column_count;
} Sprite;

static const Sprite EGG_SPRITE_NEUTRAL = {.row = 1, .column = 1, .row_count = 1, .column_count = 1};
static const Sprite EGG_SPRITE_SQUISHED = {.row = 1, .column = 2, .row_count = 1, .column_count = 1};
static const Sprite EGG_SPRITE_HATCHING = {.row = 1, .column = 3, .row_count = 1, .column_count = 1};
static const Sprite EGG_SPRITE_NEUTRAL_ALT = {.row = 2, .column = 1, .row_count = 1, .column_count = 1};
static const Sprite EGG_SPRITE_SQUISHED_ALT = {.row = 2, .column = 2, .row_count = 1, .column_count = 1};
static const Sprite EGG_SPRITE_HATCHING_ALT = {.row = 2, .column = 3, .row_count = 1, .column_count = 1};

static const Sprite BABY_SPRITE_NEUTRAL = {.row = 1, .column = 1, .row_count = 1, .column_count = 1};
static const Sprite BABY_SPRITE_HIDING = {.row = 1, .column = 2, .row_count = 1, .column_count = 1};
static const Sprite BABY_SPRITE_LOOKING_LEFT = {.row = 1, .column = 3, .row_count = 1, .column_count = 1};
static const Sprite BABY_SPRITE_LOOKING_RIGHT = {.row = 1, .column = 4, .row_count = 1, .column_count = 1};
static const Sprite BABY_SPRITE_EATING = {.row = 1, .column = 5, .row_count = 1, .column_count = 1};
static const Sprite BABY_SPRITE_HAPPY = {.row = 1, .column = 6, .row_count = 1, .column_count = 1};
static const Sprite BABY_SPRITE_ANGRY = {.row = 1, .column = 7, .row_count = 1, .column_count = 1};
static const Sprite BABY_SPRITE_FLATTENED = {.row = 1, .column = 8, .row_count = 1, .column_count = 1};

void clear_play_area(void);
void update_play_area_with_sprite(int start_x, int start_y, const Sprite *sprite);
void draw_sprite(Layer *layer, GContext *ctx);
void set_sprite_offset(int x_offset);
void sprite_sheet_stage(uint8_t stage);
void sprites_init(void);
void sprites_deinit(void);