#include <pebble.h>
#include "trackHB.h"
#include "pge_sprite.h"
#include "pge_collision.h"


static GBitmap *kerbLeft;
static GBitmap *kerbRight;
static GBitmap *finishLine;

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
    finishLineRect.origin.x = (TRACK_START_LINE - diff - 80);
    finishLineRect.origin.y = 0;
    finishLineRect.size.w = 150;
    finishLineRect.size.h = 168;
    if(abs(diff) <= 500) graphics_draw_bitmap_in_rect(ctx, finishLine, finishLineRect);
}


bool car_crossed_line(GRect carRect) {
    if(pge_collision_rectangle_rectangle(&finishLineRect, &carRect)) {
        return true;
    } else {
        return false;
    }
}
// This determines how much distance remains
//  It works in 100s, so it is 100, 200, 300 etc
static GRect distanceMarkers[33];
static GFont distFont;

void set_up_distance_markers() {
    distFont = fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS);
    GRect dist = { {8, 64}, {140, 80} };
    
    for(int i=0; i < 33; i++) {
        dist.origin.x = i*100;
        distanceMarkers[i] = dist;
    }
}


static char distBuf[] = "0000000000000";    

void draw_remaining_distance(GContext *ctx, int cameraFocus) {
    // 10 pixels == 1 metre
    int howFarToGo = (TRACK_FINISH_LINE - cameraFocus) / 10;
    int diff;
    GRect distanceRect;
    howFarToGo = (howFarToGo / 100);
    int m;
    graphics_context_set_text_color(ctx, GColorBlack);
    for(m=(howFarToGo-1); m <= (howFarToGo+1); m++) {
        diff = (m * 1000) - (TRACK_FINISH_LINE - cameraFocus);
        distanceRect = distanceMarkers[m];
        distanceRect.origin.x = TRACK_START_LINE - diff;

        snprintf(distBuf, sizeof(distBuf), "%d", m*100);

        graphics_draw_text(ctx, distBuf, distFont, distanceRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
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
    draw_remaining_distance(ctx, cameraFocus);
    draw_finish_line(ctx, cameraFocus);
    draw_kerbs(ctx, cameraFocus);
}


