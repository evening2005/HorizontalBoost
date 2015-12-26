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
carType orangeCar;
carType yellowCar;
carType greenCar;
carType blackCar;
carType blackTruck;
carType orangeTruck;
carType yellowTruck;
carType greenTruck;


static void game_logic() {
    // Per-frame game logic here

    if(get_current_state() == STATE_BEFORERACE) {
        place_cars_on_grid();
        set_race_start_time();
        switch_on_light();
        set_current_state(STATE_RACING);
    } else if(get_current_state() == STATE_RACING) {
        car_frame_update();
    } else if(get_current_state() == STATE_AFTERRACE) {
        psleep(100); // Trying to save the battery!
    }

}

static void game_draw(GContext *ctx) {

    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    // Per-frame game rendering here
    if(get_current_state() == STATE_RACING) {
        draw_track(ctx, get_camera_focus());
        draw_cars(ctx);
        update_boost_ui(ctx);
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
    // Load the finishing line bitmap
    load_finish_line_bitmap();
    set_up_distance_markers();
    
    initialise_car(&blueCar, RESOURCE_ID_BLUE_CAR, GColorCadetBlue, "Player");    
    set_player_car(&blueCar);
    initialise_car(&orangeCar, RESOURCE_ID_ORANGE_CAR, GColorOrange, "Orange");
    car_add_to_grid(&orangeCar);
    initialise_car(&yellowCar, RESOURCE_ID_YELLOW_CAR, GColorYellow, "Yellow");    
    car_add_to_grid(&yellowCar);
    initialise_car(&greenCar, RESOURCE_ID_GREEN_CAR, GColorGreen, "Green");    
    car_add_to_grid(&greenCar);
    initialise_car(&orangeTruck, RESOURCE_ID_ORANGE_TRUCK, GColorOrange, "OrangeT");
    car_add_to_grid(&orangeTruck);
    initialise_car(&yellowTruck, RESOURCE_ID_YELLOW_TRUCK, GColorYellow, "YellowT");    
    car_add_to_grid(&yellowTruck);
    initialise_car(&greenTruck, RESOURCE_ID_GREEN_TRUCK, GColorGreen, "GreenT");    
    car_add_to_grid(&greenTruck);

    // Start the game
    // Keep a Window reference for adding other UI
    pge_begin(GColorDarkGray, game_logic, game_draw, game_click);
    gameWindow = pge_get_window();
    set_current_state(STATE_BEFORERACE);
}

void pge_deinit() {

    // Finish the game
    delete_cars();
    destroy_kerb_bitmaps();
    destroy_finish_line_bitmap();
    light_off(NULL);
   
    pge_finish();

}

