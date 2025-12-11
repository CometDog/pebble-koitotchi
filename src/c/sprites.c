#include "sprites.h"
#include "ui_config.h"

static GBitmap *s_sprite_sheet;
static bool s_alt_sprites = false;
static int8_t s_sprite_x_offset = 0;
static bool PLAY_AREA;

void clear_play_area(void)
{
    memset(s_play_area_state, 0, sizeof(s_play_area_state));
    set_sprite_offset(0);
}

bool get_pixel_from_sheet(int x, int y)
{
    if (!s_sprite_sheet)
        return false;

    GBitmapFormat format = gbitmap_get_format(s_sprite_sheet);
    uint8_t *data = gbitmap_get_data(s_sprite_sheet);
    int bytes_per_row = gbitmap_get_bytes_per_row(s_sprite_sheet);

    if (format == GBitmapFormat1Bit || format == GBitmapFormat1BitPalette)
    {
        uint8_t byte = data[y * bytes_per_row + (x / 8)];
        // We return true for black pixels to know to draw them
        return !((byte >> (7 - (x % 8))) & 1);
    }
    return false;
}

void update_play_area_with_sprite(int start_x, int start_y, const Sprite *sprite)
{
    int sprite_width = sprite->column_count * 16;
    int sprite_height = sprite->row_count * 16;
    int sheet_origin_x = (sprite->column - 1) * 16;
    int sheet_origin_y = (sprite->row - 1) * 16;

    for (int y = 0; y < sprite_height; y++)
    {
        for (int x = 0; x < sprite_width; x++)
        {
            int target_x = start_x + x;
            int target_y = start_y + y;

            if (target_x >= 0 && target_x < PLAY_AREA_COLUMNS && target_y >= 0 && target_y < PLAY_AREA_ROWS)
            {

                int grid_index = target_y * PLAY_AREA_COLUMNS + target_x;

                int sheet_x = sheet_origin_x + x;
                int sheet_y = sheet_origin_y + y;

                if (get_pixel_from_sheet(sheet_x, sheet_y))
                {
                    s_play_area_state[grid_index] = true;
                }
            }
        }
    }
}

void draw_sprite(Layer *layer, GContext *ctx)
{
    graphics_context_set_fill_color(ctx, PLAY_AREA_BACKGROUND_COLOR);
    GRect bounds = layer_get_bounds(layer);
    graphics_fill_rect(ctx,
                       GRect(bounds.origin.x, bounds.origin.y + PLAY_AREA_TOP, bounds.size.w,
                             (PLAY_AREA_CELL_SIZE * PLAY_AREA_ROWS) + (PLAY_AREA_ROWS - 1)),
                       0, GCornerNone);

    for (int y = 0; y < PLAY_AREA_ROWS; y++)
    {
        for (int x = 0; x < PLAY_AREA_COLUMNS; x++)
        {
            int source_x = x - s_sprite_x_offset;
            bool should_fill = false;

            if (source_x >= 0 && source_x < PLAY_AREA_COLUMNS)
            {
                int i = y * PLAY_AREA_COLUMNS + source_x;
                should_fill = s_play_area_state[i];
            }

            if (should_fill)
            {
                graphics_context_set_fill_color(ctx, PLAY_AREA_GRID_1);
            }
            else
            {
                graphics_context_set_fill_color(ctx, PLAY_AREA_GRID_0);
            }
            graphics_fill_rect(ctx,
                               GRect(PLAY_AREA_LEFT + x * (PLAY_AREA_CELL_SIZE + 1),
                                     PLAY_AREA_TOP + y * (PLAY_AREA_CELL_SIZE + 1), PLAY_AREA_CELL_SIZE,
                                     PLAY_AREA_CELL_SIZE),
                               0, GCornerNone);
        }
    }
}

void set_sprite_offset(int x_offset)
{
    s_sprite_x_offset = x_offset;
}

void sprites_init(void)
{
    s_alt_sprites = rand() % 2 == 1;

    s_sprite_sheet =
        gbitmap_create_with_resource(s_alt_sprites ? RESOURCE_ID_SPRITE_SHEET_EGG_ALT : RESOURCE_ID_SPRITE_SHEET_EGG);
}

void sprites_deinit(void)
{
    if (s_sprite_sheet)
    {
        gbitmap_destroy(s_sprite_sheet);
    }
}
