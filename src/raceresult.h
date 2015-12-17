#pragma once
#include "pebble.h"

#define RESULTS_TOP (32)
#define RESULTS_LEFT (24)
#define RESULTS_GAP  (2)

void race_result_create_position_layers();
void race_result_destroy_assets();
void race_result_destroy_animations();
void race_result_populate_position_layers(GContext *ctx);
// bool race_result_check_for_select();
void load_result_backdrop_and_font() ;
void destroy_result_backdrop_and_font();