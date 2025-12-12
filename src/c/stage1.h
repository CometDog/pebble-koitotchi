#pragma once
#include <pebble.h>

void stage1_init(Layer *parent_layer, void (*stage_completion_handler)(int8_t));
void stage1_deinit(void);
