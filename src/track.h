#pragma once

#include <pebble.h>

/* BARRIER INFORMATION */

#define BARRIER_LEFT (8)
#define BARRIER_LENGTH (32)
#define BARRIER_WIDTH (10)
#define BARRIER_RIGHT (148 - 8 - BARRIER_WIDTH)

#define LINE_LENGTH (32)
#define LINE_WIDTH (4)

#define IBL (BARRIER_LEFT + BARRIER_WIDTH)
#define TRACK_CENTRE_LINE ((IBL + BARRIER_RIGHT - LINE_WIDTH) / 2)
#define TRACK_START_LINE (76)
#define TRACK_KERB (46)

#define TRACK_FINISH_LINE (15000)



GBitmap *finishLine;
void load_finish_line_bitmap();
void draw_track(GContext *ctx, int playerPosition);
void draw_finish_line(GContext *ctx, int playerPosition);
bool car_crossed_line(GRect carRect);
void set_up_distance_markers();



