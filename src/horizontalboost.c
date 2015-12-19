/**
 * This is the bare minimum to make a looping game with PGE!
 */

#include <pebble.h>
 
#include "pge.h"
#include "trackHB.h"
#include "car.h"
#include "statemachine.h"
#include "gamelight.h"

static Window *gameWindow;
carType blueCar;



static void game_logic() {
    // Per-frame game logic here
    if(get_current_state() == STATE_BEFORERACE) {
        switch_on_light();
        set_current_state(STATE_RACING);        
    } else if(get_current_state() == STATE_RACING) {

    } else if(get_current_state() == STATE_AFTERRACE) {
        psleep(100); // Trying to save the battery!
    }
}

static GPoint pos = {0, 0};

static void game_draw(GContext *ctx) {
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    // Per-frame game rendering here
    if(get_current_state() == STATE_RACING) {
        pos.x = pos.x % 32;
        pos.x += 17;
        draw_track(ctx, pos);
        position_car(&blueCar);
        pge_sprite_draw(blueCar.sprite, ctx);

    } else if(get_current_state() == STATE_AFTERRACE) {
        // This actually only gets done once

    }
}

static void game_click(int buttonID, bool longClick) {
    if(buttonID == BUTTON_ID_SELECT) {
        if(get_current_state() == STATE_AFTERRACE) {
            set_current_state(STATE_BEFORERACE);
        }
    }
}



void pge_init() {
    srand(time(NULL));
    load_kerb_bitmaps();
    initialise_car(&blueCar, RESOURCE_ID_BLUE_CAR, GColorCadetBlue, "Player");    
    
    // NB !!!!!  This is temporary !!!! It should be BEFORERACE  !!!!!
    set_current_state(STATE_RACING);
    // Start the game
    pge_begin(GColorDarkGray, game_logic, game_draw, game_click);
    // Keep a Window reference for adding other UI
    gameWindow = pge_get_window();
}

void pge_deinit() {
    // Finish the game
    pge_sprite_destroy(blueCar.sprite);
    destroy_kerb_bitmaps();
    light_off(NULL);
    pge_finish();
}

