#include "stage1.h"
#include "sprites.h"
#include "ui_config.h"
#include <pebble.h>

static Layer *s_egg_layer;
static AppTimer *s_frame_timer;
static bool s_is_squished = false;
static bool s_wiggle_direction_right = false;
static int s_wiggled_times = 0;
static bool s_alt_egg = false;
static int s_incubate_seconds = 0;
static int s_hatching_seconds = 0;
static void (*s_stage_completion_handler)(int8_t);

#define EGG_BOUNCE_INTERVAL_MS 500
#define EGG_HATCH_INCUBATING_CYCLES (5 * 60) * 2 // 5 minutes (2 cycles per second)
#define EGG_HATCH_WIGGLE_DELAY_SECONDS 1
#define EGG_HATCH_WIGGLE_COUNT 3
#define EGG_HATCH_DELAY_SECONDS 1
#define EGG_HATCH_DISPLAY_SECONDS 2
#define EGG_HATCH_TOTAL_SECONDS (EGG_HATCH_DELAY_SECONDS + EGG_HATCH_DISPLAY_SECONDS)

typedef enum
{
    EGG_STATE_INCUBATING,
    EGG_STATE_WIGGLING,
    EGG_STATE_HATCHING
} EggState;

static EggState s_egg_state = EGG_STATE_INCUBATING;

static void update_egg_state()
{
    clear_play_area();

    const Sprite *current_sprite;

    if (s_egg_state == EGG_STATE_HATCHING)
    {
        if (s_hatching_seconds < EGG_HATCH_DELAY_SECONDS)
        {
            current_sprite = s_alt_egg ? &EGG_SPRITE_SQUISHED_ALT : &EGG_SPRITE_SQUISHED;
        }
        else
        {
            current_sprite = s_alt_egg ? &EGG_SPRITE_HATCHING_ALT : &EGG_SPRITE_HATCHING;
        }
    }
    else if (s_egg_state == EGG_STATE_WIGGLING)
    {
        current_sprite = s_alt_egg ? &EGG_SPRITE_SQUISHED_ALT : &EGG_SPRITE_SQUISHED;
        set_sprite_offset(s_wiggle_direction_right ? 1 : -1);
    }
    else
    {
        if (s_is_squished)
        {
            current_sprite = s_alt_egg ? &EGG_SPRITE_SQUISHED_ALT : &EGG_SPRITE_SQUISHED;
        }
        else
        {
            current_sprite = s_alt_egg ? &EGG_SPRITE_NEUTRAL_ALT : &EGG_SPRITE_NEUTRAL;
        }
    }

    int sprite_width = current_sprite->column_count * 16;
    int sprite_height = current_sprite->row_count * 16;

    int sprite_x = (PLAY_AREA_COLUMNS - sprite_width) / 2;
    int sprite_y = PLAY_AREA_ROWS - sprite_height;

    update_play_area_with_sprite(sprite_x, sprite_y, current_sprite);
    layer_mark_dirty(s_egg_layer);
}

static void next_frame_handler(void *context)
{
    if (s_egg_state == EGG_STATE_INCUBATING)
    {
        s_incubate_seconds++;
        s_is_squished = !s_is_squished;

        // Only begin hatching from squished so it looks like it burst open
        if (s_incubate_seconds >= EGG_HATCH_INCUBATING_CYCLES && !s_is_squished)
        {
            s_egg_state = EGG_STATE_WIGGLING;
            s_wiggled_times = 0;
        }
    }
    else if (s_egg_state == EGG_STATE_WIGGLING)
    {
        s_wiggled_times++;
        s_wiggle_direction_right = !s_wiggle_direction_right;

        if (s_wiggled_times >= EGG_HATCH_WIGGLE_COUNT)
        {
            s_egg_state = EGG_STATE_HATCHING;
            s_hatching_seconds = 0;
        }
    }
    else if (s_egg_state == EGG_STATE_HATCHING)
    {
        s_hatching_seconds++;
        if (s_hatching_seconds >= EGG_HATCH_TOTAL_SECONDS)
        {
            if (s_stage_completion_handler)
            {
                s_stage_completion_handler(2);
                return;
            }
            else
            {
                // CRITICAL ERROR: No stage completion handler set
                APP_LOG(APP_LOG_LEVEL_ERROR, "No stage completion handler set in stage1!");
            }
        }
    }

    update_egg_state();
    s_frame_timer = app_timer_register(EGG_BOUNCE_INTERVAL_MS, next_frame_handler, NULL);
}

static void layer_update_proc(Layer *layer, GContext *ctx)
{
    draw_sprite(layer, ctx);
}

void stage1_init(Layer *parent_layer, void (*stage_completion_handler)(int8_t))
{
    sprite_sheet_stage(1);
    s_alt_egg = rand() % 2 == 1;

    GRect bounds = layer_get_bounds(parent_layer);

    s_egg_layer = layer_create(bounds);
    layer_set_update_proc(s_egg_layer, layer_update_proc);
    layer_add_child(parent_layer, s_egg_layer);

    update_egg_state();
    s_frame_timer = app_timer_register(EGG_BOUNCE_INTERVAL_MS, next_frame_handler, NULL);

    s_stage_completion_handler = stage_completion_handler;
}

void stage1_deinit(void)
{
    if (s_frame_timer)
    {
        app_timer_cancel(s_frame_timer);
    }
    layer_destroy(s_egg_layer);
}
