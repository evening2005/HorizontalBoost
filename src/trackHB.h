#pragma once
#include <pebble.h>


GBitmap *kerbLeft;
GBitmap *kerbRight;

GBitmap *finishLine;

void load_kerb_bitmaps();
void destroy_kerb_bitmaps();

void draw_track(GContext *ctx, GPoint pos);

