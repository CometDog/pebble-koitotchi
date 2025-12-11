#include "app.h"
#include "sprites.h"
#include "stage1.h"

static void init(void)
{
    sprites_init();
    stage1_init();
}

static void deinit(void)
{
    stage1_deinit();
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
