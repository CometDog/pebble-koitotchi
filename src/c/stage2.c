#include "sprites.h"
#include "stage1.h"
#include "ui_config.h"
#include <pebble.h>

static Layer *s_baby_layer;
static Window *s_window;
static AppTimer *s_frame_timer;
static AppTimer *s_hunger_timer;
static AppTimer *s_happiness_timer;
static bool s_is_hiding = false;
static bool s_movement_direction_right = false;
static uint8_t s_movement_type_cycle_count = 0; // 0, 1, 2
static int8_t s_movement_position = 0;          // -2, -1, 0, 1, 2
static void (*s_stage_completion_handler)(int8_t);

// Care stats (0-4, 4 being full)
static uint8_t s_hunger = 0;    // 0 = starving, 4 = full
static uint8_t s_happiness = 0; // 0 = unhappy, 4 = very happy
static bool s_calling_attention = false;
static bool s_is_eating = false;
static bool s_is_playing = false;

#define BABY_ANIMATION_INTERVAL_MS 500
#define BABY_MOVE_DISTANCE 5
#define BABY_MOVE_TYPE_CYCLES 2                       // Be neutral or hiding for 2 cycles max
#define BABY_MOVE_POSITIONS 5                         // 5 positions to move between
#define HUNGER_DEGRADE_INTERVAL_MS (3 * 60 * 1000)    // Lose 1 hunger every 3 minutes
#define HAPPINESS_DEGRADE_INTERVAL_MS (4 * 60 * 1000) // Lose 1 happiness every 4 minutes
#define FEED_ANIMATION_DURATION_MS 2000
#define PLAY_ANIMATION_DURATION_MS 2000

typedef enum
{
    BABY_STATE_IDLE,
    BABY_STATE_MOVING,
    BABY_STATE_EATING,
    BABY_STATE_PLAYING,
    BABY_STATE_CALLING
} BabyState;

static BabyState s_baby_state = BABY_STATE_MOVING;

static void update_baby_state()
{
    clear_play_area();

    const Sprite *current_sprite;

    if (s_baby_state == BABY_STATE_EATING)
    {
        current_sprite = &BABY_SPRITE_EATING;
    }
    else if (s_baby_state == BABY_STATE_PLAYING)
    {
        current_sprite = &BABY_SPRITE_HAPPY;
    }
    else if (s_baby_state == BABY_STATE_CALLING)
    {
        vibes_short_pulse();
        current_sprite = &BABY_SPRITE_ANGRY;
    }
    else
    {
        current_sprite = s_is_hiding ? &BABY_SPRITE_HIDING : &BABY_SPRITE_NEUTRAL;
    }

    int sprite_width = current_sprite->column_count * 16;
    int sprite_height = current_sprite->row_count * 16;

    int sprite_x = (PLAY_AREA_COLUMNS - sprite_width) / 2;
    int sprite_y = PLAY_AREA_ROWS - sprite_height;

    update_play_area_with_sprite(sprite_x, sprite_y, current_sprite);

    if (s_baby_state == BABY_STATE_MOVING)
    {
        set_sprite_offset(s_movement_position * BABY_MOVE_DISTANCE);
    }

    layer_mark_dirty(s_baby_layer);
}

static void next_frame_handler(void *context)
{
    if (s_movement_type_cycle_count == BABY_MOVE_TYPE_CYCLES)
    {
        s_movement_type_cycle_count = 1;
        s_is_hiding = !s_is_hiding;
    }
    else
    {
        s_movement_type_cycle_count++;
    }

    if (s_baby_state == BABY_STATE_MOVING)
    {
        if (s_movement_direction_right)
        {
            if (s_movement_position == 2)
            {
                s_movement_direction_right = false;
                s_movement_position--;
            }
            else
            {
                s_movement_position++;
            }
        }
        else
        {
            if (s_movement_position == -2)
            {
                s_movement_direction_right = true;
                s_movement_position++;
            }
            else
            {
                s_movement_position--;
            }
        }
    }

    update_baby_state();
    s_frame_timer = app_timer_register(BABY_ANIMATION_INTERVAL_MS, next_frame_handler, NULL);
}

static void hunger_degrade_callback(void *context)
{
    if (s_hunger > 0)
    {
        s_hunger--;

        if (s_hunger == 0 && !s_calling_attention)
        {
            s_calling_attention = true;
            s_baby_state = BABY_STATE_CALLING;
            update_baby_state();
            vibes_short_pulse();
        }
    }

    s_hunger_timer = app_timer_register(HUNGER_DEGRADE_INTERVAL_MS, hunger_degrade_callback, NULL);
}

static void happiness_degrade_callback(void *context)
{
    if (s_happiness > 0)
    {
        s_happiness--;

        if (s_happiness == 0 && !s_calling_attention)
        {
            s_calling_attention = true;
            s_baby_state = BABY_STATE_CALLING;
            update_baby_state();
            vibes_short_pulse();
        }
    }

    s_happiness_timer = app_timer_register(HAPPINESS_DEGRADE_INTERVAL_MS, happiness_degrade_callback, NULL);
}

static void feed_animation_end(void *context)
{
    s_is_eating = false;
    s_baby_state = BABY_STATE_MOVING;
    update_baby_state();
}

static void play_animation_end(void *context)
{
    s_is_playing = false;
    s_baby_state = BABY_STATE_MOVING;
    update_baby_state();
}

static void layer_update_proc(Layer *layer, GContext *ctx)
{
    draw_sprite(layer, ctx);
}

void stage2_init(Layer *parent_layer, void (*stage_completion_handler)(int8_t))
{
    sprite_sheet_stage(2);

    s_window = layer_get_window(parent_layer);

    GRect bounds = layer_get_bounds(parent_layer);

    s_baby_layer = layer_create(bounds);
    layer_set_update_proc(s_baby_layer, layer_update_proc);
    layer_add_child(parent_layer, s_baby_layer);

    s_hunger = 0;
    s_happiness = 0;
    s_calling_attention = true;
    s_baby_state = BABY_STATE_CALLING;
    s_is_eating = false;
    s_is_playing = false;

    update_baby_state();

    s_frame_timer = app_timer_register(BABY_ANIMATION_INTERVAL_MS, next_frame_handler, NULL);
    s_hunger_timer = app_timer_register(HUNGER_DEGRADE_INTERVAL_MS, hunger_degrade_callback, NULL);
    s_happiness_timer = app_timer_register(HAPPINESS_DEGRADE_INTERVAL_MS, happiness_degrade_callback, NULL);

    s_stage_completion_handler = stage_completion_handler;
}

void stage2_deinit(void)
{
    if (s_frame_timer)
    {
        app_timer_cancel(s_frame_timer);
        s_frame_timer = NULL;
    }
    if (s_hunger_timer)
    {
        app_timer_cancel(s_hunger_timer);
        s_hunger_timer = NULL;
    }
    if (s_happiness_timer)
    {
        app_timer_cancel(s_happiness_timer);
        s_happiness_timer = NULL;
    }

    if (s_baby_layer)
    {
        layer_destroy(s_baby_layer);
        s_baby_layer = NULL;
    }
}
