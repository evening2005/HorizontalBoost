#include <pebble.h>
#include "pge.h"
#include "raceresult.h"
#include "car.h"
#include "trackHB.h"
#include "pge_sprite.h"

#define PODIUM_POSITIONS (4)

static GBitmap *resultBackdropBitmap;
BitmapLayer *resultBackdropLayer;
static GFont customFont;
static GFont titleFont;


void load_result_backdrop_and_font() {
    GRect resultBackdropRect = { {10,20}, {128,128} };
    resultBackdropLayer = bitmap_layer_create(resultBackdropRect);
    resultBackdropBitmap = gbitmap_create_with_resource(RESOURCE_ID_GOLD_BACKGROUND);

    bitmap_layer_set_bitmap(resultBackdropLayer, resultBackdropBitmap);
    customFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PRAGATTINARROW_BOLD_18));
    titleFont = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK );

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

static BitmapLayer *carPositionLayers[PODIUM_POSITIONS];
static TextLayer *positionLayers[PODIUM_POSITIONS];
static char positionTexts[PODIUM_POSITIONS][24];
static GRect textScreenPos = { {-350, 0}, {92, 24} };
static GRect carScreenPos =  { {-300, 0}, {30, 24} };

void race_result_create_position_layers() {
    int rank;
    int textYPos, carYPos;
    
    for(rank = 0; rank < PODIUM_POSITIONS; rank++) {
        textYPos = RESULTS_TOP + (rank * (textScreenPos.size.h + RESULTS_GAP));    
        carYPos = RESULTS_TOP + (rank * (carScreenPos.size.h + RESULTS_GAP));
        textScreenPos.origin.y = textYPos; // Somewhere off screen - they will "animate" into position
        positionLayers[rank] = text_layer_create(textScreenPos);
        text_layer_set_font(positionLayers[rank], customFont);
        text_layer_set_text_alignment(positionLayers[rank], GTextAlignmentCenter);
        text_layer_set_text_color(positionLayers[rank], GColorBlack);
        carScreenPos.origin.y = carYPos;
        carPositionLayers[rank] = bitmap_layer_create(carScreenPos);
    }
}



static bool alreadyPopulated = false;

// Two animations per rank: (1) Car bitmap; (2) Text
static PropertyAnimation *resultAnimations[PODIUM_POSITIONS*2];
static Animation *animationSequence;
static char *titleText = "RESULTS";
static GRect titleRect = { {0, 0}, {144, 48}};

void race_result_populate_position_layers(GContext *ctx) {
    graphics_draw_text(ctx, titleText, titleFont, titleRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    if(alreadyPopulated == false) {

        // draw_result_backdrop(ctx);
        Window *gameWindow = pge_get_window();
        Layer *windowLayer = window_get_root_layer(gameWindow);
        int rank;
        uint32_t animationNumber = 0;
        for(rank = 0; rank < PODIUM_POSITIONS; rank++) {
            carType *whichCar;
            whichCar = get_finisher(rank);
            // First sort out the text layers...
            snprintf(positionTexts[rank], 23, "%2d%s - %2d.%03d", rank+1, get_position_suffix(rank), (int)whichCar->finished / 1000, (int)whichCar->finished % 1000);
            text_layer_set_background_color(positionLayers[rank], whichCar->carColour);
            text_layer_set_text_color(positionLayers[rank], gcolor_legible_over(whichCar->carColour));
            text_layer_set_text(positionLayers[rank], positionTexts[rank]);  
            layer_add_child(windowLayer, (Layer *)positionLayers[rank]);
            GRect fromFrame = layer_get_frame((Layer *)positionLayers[rank]);
            GRect toFrame = GRect(RESULTS_LEFT, RESULTS_TOP + (rank * (textScreenPos.size.h + RESULTS_GAP)), textScreenPos.size.w, textScreenPos.size.h);
            resultAnimations[animationNumber+1] = property_animation_create_layer_frame((Layer *)positionLayers[rank], &fromFrame, &toFrame);
            animation_set_duration((Animation *)resultAnimations[animationNumber+1], 600); // milliseconds
            animation_set_curve((Animation *)resultAnimations[animationNumber+1], AnimationCurveLinear);
            animation_set_delay((Animation *)resultAnimations[animationNumber+1], 0);
            // Now let's do the car images
            bitmap_layer_set_bitmap(carPositionLayers[rank], pge_sprite_get_bitmap(whichCar->sprite));
            layer_add_child(windowLayer, (Layer *)carPositionLayers[rank]);
            fromFrame = layer_get_frame((Layer *)carPositionLayers[rank]);
            toFrame = GRect(RESULTS_CAR_LEFT, RESULTS_TOP + (rank * (carScreenPos.size.h + RESULTS_GAP)), carScreenPos.size.w, carScreenPos.size.h);
            resultAnimations[animationNumber] = property_animation_create_layer_frame((Layer *)carPositionLayers[rank], &fromFrame, &toFrame);
            animation_set_duration((Animation *)resultAnimations[animationNumber], 600); // milliseconds
            animation_set_curve((Animation *)resultAnimations[animationNumber], AnimationCurveLinear);
            animation_set_delay((Animation *)resultAnimations[animationNumber], rank * 50);
            animationNumber += 2;
        }
        animationSequence = animation_sequence_create_from_array((Animation **)resultAnimations, PODIUM_POSITIONS*2);
        animation_schedule(animationSequence);
        alreadyPopulated = true;
    }
}


void race_result_destroy_assets() {
    int rank;
    for(rank = 0; rank < PODIUM_POSITIONS; rank++) {
        layer_remove_from_parent((Layer *)positionLayers[rank]);
        text_layer_destroy(positionLayers[rank]);
        layer_remove_from_parent((Layer *)carPositionLayers[rank]);
        bitmap_layer_destroy(carPositionLayers[rank]);
    }
    layer_remove_from_parent((Layer *)resultBackdropLayer);
    alreadyPopulated = false;
}

void race_result_destroy_animations() {
    animation_unschedule_all();
}


