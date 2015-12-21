#pragma once
#include <pebble.h>


#define TRACK_START_LINE (76)
#define TRACK_FINISH_LINE (15000)


GBitmap *kerbLeft;
GBitmap *kerbRight;

GBitmap *finishLine;

void load_kerb_bitmaps();
void destroy_kerb_bitmaps();

void draw_track(GContext *ctx, int cameraFocus);

