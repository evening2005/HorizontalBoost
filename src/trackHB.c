#include "trackHB.h"
#include "pge_sprite.h"
#include "pge_collision.h"

void load_kerb_bitmaps() {
    kerbLeft = gbitmap_create_with_resource(RESOURCE_ID_HB_LEFT_KERB);
    kerbRight = gbitmap_create_with_resource(RESOURCE_ID_HB_RIGHT_KERB);    
}


void destroy_kerb_bitmaps() {
    gbitmap_destroy(kerbLeft);
    gbitmap_destroy(kerbRight);
}


void load_finish_line_bitmap() {
    finishLine = gbitmap_create_with_resource(RESOURCE_ID_FINISH_LINE);
}

void destroy_finish_line_bitmap() {
    gbitmap_destroy(finishLine);
}


static GRect finishLineRect;
void draw_finish_line(GContext *ctx, int cameraFocus) {
    int diff = cameraFocus - TRACK_FINISH_LINE;
    finishLineRect.origin.x = 5;
    finishLineRect.origin.y = (TRACK_START_LINE + diff);
    finishLineRect.size.w = 132;
    finishLineRect.size.h = 120;
    if(abs(diff) <= 500) graphics_draw_bitmap_in_rect(ctx, finishLine, finishLineRect);
}


bool car_crossed_line(GRect carRect) {
    if(pge_collision_rectangle_rectangle(&finishLineRect, &carRect)) {
        return true;
    } else {
        return false;
    }
}

GRect kerbLeftRect = { {0, 0}, {180, 8} };
GRect kerbRightRect = { {0, 160}, {180, 8} };
GRect WHOLE_SCREEN = { {0,0}, {144,168}};

void draw_road_surface(GContext *ctx) {
    graphics_context_set_fill_color(ctx, GColorLightGray);
    graphics_fill_rect(ctx, WHOLE_SCREEN, 0, GCornerNone);
}

void draw_kerbs(GContext *ctx, int cameraFocus) {
    int xPos = -(cameraFocus & 31);
    kerbLeftRect.origin.x = xPos;
    kerbRightRect.origin.x = xPos;
    graphics_draw_bitmap_in_rect(ctx, kerbLeft, kerbLeftRect);
    graphics_draw_bitmap_in_rect(ctx, kerbRight, kerbRightRect);
}

void draw_track(GContext *ctx, int cameraFocus) {
    draw_road_surface(ctx);
    draw_kerbs(ctx, cameraFocus);
}


