#pragma once
#include <pebble.h>

void stage2_init(Layer *parent_layer, void (*stage_completion_handler)(int8_t));
void stage2_deinit(void);
