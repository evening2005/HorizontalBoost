#include <pebble.h>
#include "pge.h"
#include "raceresult.h"
#include "car.h"
#include "trackHB.h"

#define PODIUM_POSITIONS (4)

static GBitmap *resultBackdropBitmap;
BitmapLayer *resultBackdropLayer;
static GFont customFont;


void load_result_backdrop_and_font() {
    GRect resultBackdropRect = { {10,20}, {128,128} };
    resultBackdropLayer = bitmap_layer_create(resultBackdropRect);
    resultBackdropBitmap = gbitmap_create_with_resource(RESOURCE_ID_GOLD_BACKGROUND);

    bitmap_layer_set_bitmap(resultBackdropLayer, resultBackdropBitmap);
    customFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PRAGATTINARROW_BOLD_18));
}

void destroy_result_backdrop_and_font() {
    fonts_unload_custom_font(customFont);
    layer_remove_from_parent((Layer *)resultBackdropLayer);
    bitmap_layer_destroy(resultBackdropLayer);
    gbitmap_destroy(resultBackdropBitmap);  
}

void draw_result_backdrop(GContext *ctx) {
    Window *gameWindow = pge_get_window();
    Layer *windowLayer = window_get_root_layer(gameWindow);
    layer_add_child(windowLayer, (Layer *)resultBackdropLayer);
}


static char * get_position_suffix(int position) {
    if(position == 0) {
        return "st";
    } else if(position == 1) {
        return "nd";
    } else if(position == 2) {
        return "rd";
    } else if(position < 20) {
        return "th";
    } 
    return "  ";   
}

static TextLayer *positionLayers[PODIUM_POSITIONS];
static char positionTexts[PODIUM_POSITIONS][24];
static GRect screenPos = { {8, 0}, {100, 24} };

void race_result_create_position_layers() {
    int rank;
    
    for(rank = 0; rank < PODIUM_POSITIONS; rank++) {
        screenPos.origin.y = 300; // Somewhere off screen - they will "animate" into position
        positionLayers[rank] = text_layer_create(screenPos);
        text_layer_set_font(positionLayers[rank], customFont);
        text_layer_set_text_alignment(positionLayers[rank], GTextAlignmentCenter);
        text_layer_set_text_color(positionLayers[rank], GColorBlack);
    }
}



static bool alreadyPopulated = false;

static PropertyAnimation *resultAnimations[PODIUM_POSITIONS];
static Animation *animationSequence;

void race_result_populate_position_layers(GContext *ctx) {
    if(alreadyPopulated == false) {

        draw_result_backdrop(ctx);
        Window *gameWindow = pge_get_window();
        Layer *windowLayer = window_get_root_layer(gameWindow);
        int rank;
        for(rank = 0; rank < PODIUM_POSITIONS; rank++) {
            //carType *whichCar;
            //whichCar = get_finisher(rank);
            //snprintf(positionTexts[rank], 23, "%2d%s - %2d.%03d", rank+1, get_position_suffix(rank), (int)whichCar->finished / 1000, (int)whichCar->finished % 1000);
            //text_layer_set_background_color(positionLayers[rank], whichCar->carColour);
            //text_layer_set_text_color(positionLayers[rank], gcolor_legible_over(whichCar->carColour));
            text_layer_set_text(positionLayers[rank], positionTexts[rank]);  
            layer_add_child(windowLayer, (Layer *)positionLayers[rank]);
            GRect fromFrame = layer_get_frame((Layer *)positionLayers[rank]);
            GRect toFrame = GRect(RESULTS_LEFT, RESULTS_TOP + (rank * (screenPos.size.h + RESULTS_GAP)), screenPos.size.w, screenPos.size.h);
            resultAnimations[rank] = property_animation_create_layer_frame((Layer *)positionLayers[rank], &fromFrame, &toFrame);
            animation_set_duration((Animation *)resultAnimations[rank], 800); // 500ms
            animation_set_curve((Animation *)resultAnimations[rank], AnimationCurveEaseOut);
            animation_set_delay((Animation *)resultAnimations[rank], rank * 200);
        }
        animationSequence = animation_sequence_create_from_array((Animation **)resultAnimations, PODIUM_POSITIONS);
        animation_schedule(animationSequence);
        alreadyPopulated = true;
    }
}


void race_result_destroy_assets() {
    int rank;
    for(rank = 0; rank < PODIUM_POSITIONS; rank++) {
        layer_remove_from_parent((Layer *)positionLayers[rank]);
        text_layer_destroy(positionLayers[rank]);
    }
    layer_remove_from_parent((Layer *)resultBackdropLayer);
    alreadyPopulated = false;
}

void race_result_destroy_animations() {
    animation_unschedule_all();
}


