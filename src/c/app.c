#include "app.h"
#include "play_area.h"
#include "sprites.h"

static void init(void)
{
    sprites_init();
    play_area_init();
}

static void deinit(void)
{
    play_area_deinit();
    sprites_deinit();
}

void app_restart(void)
{
    window_stack_pop_all(false);
    deinit();
    init();
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
    return 0;
}
