#pragma once
#include <pebble.h>


#define TRACK_START_LINE (76)
#define TRACK_FINISH_LINE (15000)

#define BARRIER_LEFT (0)
#define BARRIER_WIDTH (8)

#define IBL (BARRIER_LEFT + BARRIER_WIDTH)
#define TRACK_CENTRE_LINE (84)



void load_kerb_bitmaps();
void destroy_kerb_bitmaps();

void draw_track(GContext *ctx, int cameraFocus);
bool car_crossed_line(GRect carRect);
void set_up_distance_markers();
void load_finish_line_bitmap();
void draw_finish_line(GContext *ctx, int playerPosition);
void destroy_finish_line_bitmap();

