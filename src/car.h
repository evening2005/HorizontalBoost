#pragma once
#include "pge_sprite.h"

#define CAR_WIDTH (17)
#define CAR_LENGTH (30)
#define STEER_AMOUNT (1)
#define STEER_SLOWDOWN (0)
#define REAR_END_PENALTY (1)

#define BOOST_COOLDOWN_MILLIS (2048)
#define BOOST_MINIMUM_COOLDOWN (BOOST_COOLDOWN_MILLIS >> 1)

#define SPEED_INC (7)
#define CRASH_SLOWDOWN (1)
#define BOOST_INC (23)
#define DRAG_DIVISOR (21)

#define MAX_NPCS (6)

#define min(a, b) ((a) <= (b) ? (a) : (b))

// currentSpeed is maintained in the upper byte of currentSpeed
struct CAR {
    uint8_t carNumber;
    PGESprite *sprite;
    GPoint startingPosition;
    GPoint worldPosition;
    uint32_t rank;
    bool boosting;
    uint16_t currentSpeed;
    uint64_t finished;
    time_t boostStartMillis;
    time_t boostDurationMillis;
    time_t maxBoostDurationMillis;
    time_t lastBoostMillis;
    GColor8 carColour;
    char carName[12];
};

typedef struct CAR carType;


int get_camera_focus();
void set_player_car(carType *pc);
void initialise_car(carType *carPtr, int resourceID, GColor colour, char *name);

void update_boost_ui(GContext *ctx);
void draw_cars(GContext *ctx);
void delete_cars();
void car_frame_update();
void position_car(carType *car);
void car_add_to_grid(carType *carPtr);
void set_up_steering_guide();
void set_race_start_time();

void make_pos_gap_ui(Window *window);
void destroy_pos_gap_ui();
// This is called from raceresult.c
carType *get_finisher(int rank);

// This must be set up specifically based on the width of the track and the 
//  width of the cars..
#define GRID_WIDTH  (5)
#define GRID_DEPTH  (2)
#define INTER_GRID_GAP (4)



void place_cars_on_grid();
void set_up_starting_positions();
GPoint get_starting_position(uint32_t num);


