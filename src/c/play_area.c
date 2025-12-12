#include "play_area.h"
#include "stage1.h"
#include "stage2.h"
#include "ui_config.h"

static Window *s_window;
static Layer *s_top_icon_bar_layer;
static Layer *s_bottom_icon_bar_layer;
static void init_stage(int8_t stage, Layer *parent_layer);

static void top_icon_bar_update_proc(Layer *layer, GContext *ctx)
{
    GRect bounds = layer_get_bounds(layer);

    // TODO: Clean this up, it should be in ui config and work for different screen sizes
    const int icon_size = 25;
    const int total_width = bounds.size.w;
    const int spacing = (total_width - (4 * icon_size)) / 5;
    const int y_pos = (bounds.size.h - icon_size) / 2;

    int x_pos = spacing;

    // Eat
    GDrawCommandImage *eat_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_TOOL_ICON_EAT);
    if (eat_icon)
    {
        gdraw_command_image_draw(ctx, eat_icon, GPoint(x_pos, y_pos));
        gdraw_command_image_destroy(eat_icon);
    }

    // Sleep
    x_pos += icon_size + spacing;
    GDrawCommandImage *sleep_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_TOOL_ICON_SLEEP);
    if (sleep_icon)
    {
        gdraw_command_image_draw(ctx, sleep_icon, GPoint(x_pos, y_pos));
        gdraw_command_image_destroy(sleep_icon);
    }

    // Play
    x_pos += icon_size + spacing;
    GDrawCommandImage *play_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_TOOL_ICON_PLAY);
    if (play_icon)
    {
        gdraw_command_image_draw(ctx, play_icon, GPoint(x_pos, y_pos));
        gdraw_command_image_destroy(play_icon);
    }

    // Heal
    x_pos += icon_size + spacing;
    GDrawCommandImage *heal_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_TOOL_ICON_HEAL);
    if (heal_icon)
    {
        gdraw_command_image_draw(ctx, heal_icon, GPoint(x_pos, y_pos));
        gdraw_command_image_destroy(heal_icon);
    }
}

static void bottom_icon_bar_update_proc(Layer *layer, GContext *ctx)
{
    GRect bounds = layer_get_bounds(layer);

    // TODO: Clean this up, it should be in ui config and work for different screen sizes
    const int icon_size = 25;
    const int total_width = bounds.size.w;
    const int spacing = (total_width - (4 * icon_size)) / 5;
    const int y_pos = (bounds.size.h - icon_size) / 2;

    int x_pos = spacing;

    // Clean
    GDrawCommandImage *clean_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_TOOL_ICON_CLEAN);
    if (clean_icon)
    {
        gdraw_command_image_draw(ctx, clean_icon, GPoint(x_pos, y_pos));
        gdraw_command_image_destroy(clean_icon);
    }

    // Check Health
    x_pos += icon_size + spacing;
    GDrawCommandImage *health_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_TOOL_ICON_CHECK_HEALTH);
    if (health_icon)
    {
        gdraw_command_image_draw(ctx, health_icon, GPoint(x_pos, y_pos));
        gdraw_command_image_destroy(health_icon);
    }

    // Alert
    x_pos += icon_size + spacing;
    GDrawCommandImage *alert_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_TOOL_ICON_ALERT);
    if (alert_icon)
    {
        gdraw_command_image_draw(ctx, alert_icon, GPoint(x_pos, y_pos));
        gdraw_command_image_destroy(alert_icon);
    }

    // Discipline
    x_pos += icon_size + spacing;
    GDrawCommandImage *discipline_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_TOOL_ICON_DISCIPLINE);
    if (discipline_icon)
    {
        gdraw_command_image_draw(ctx, discipline_icon, GPoint(x_pos, y_pos));
        gdraw_command_image_destroy(discipline_icon);
    }
}

void update_stage(int8_t stage)
{
    init_stage(stage, window_get_root_layer(s_window));
}

void init_stage(int8_t stage, Layer *parent_layer)
{
    if (stage == 2)
    {
        stage2_init(parent_layer, update_stage);
    }
    else
    {
        stage1_init(parent_layer, update_stage);
    }
}

void play_area_init(void)
{
    s_window = window_create();

    window_set_background_color(s_window, FULL_WINDOW_BACKGROUND_COLOR);

    window_stack_push(s_window, false);

    Layer *root_layer = window_get_root_layer(s_window);
    GRect bounds = layer_get_bounds(root_layer);

    // Top icon bar, 50 pixels at the top
    s_top_icon_bar_layer = layer_create(GRect(0, 0, bounds.size.w, 50));
    layer_set_update_proc(s_top_icon_bar_layer, top_icon_bar_update_proc);
    layer_add_child(root_layer, s_top_icon_bar_layer);

    // Bottom icon bar, 50 pixels at the bottom
    s_bottom_icon_bar_layer = layer_create(GRect(0, bounds.size.h - 50, bounds.size.w, 50));
    layer_set_update_proc(s_bottom_icon_bar_layer, bottom_icon_bar_update_proc);
    layer_add_child(root_layer, s_bottom_icon_bar_layer);

    init_stage(1, root_layer);
}

void play_area_deinit(void)
{
    if (s_top_icon_bar_layer)
    {
        layer_destroy(s_top_icon_bar_layer);
        s_top_icon_bar_layer = NULL;
    }

    if (s_bottom_icon_bar_layer)
    {
        layer_destroy(s_bottom_icon_bar_layer);
        s_bottom_icon_bar_layer = NULL;
    }

    if (s_window)
    {
        window_destroy(s_window);
    }
}