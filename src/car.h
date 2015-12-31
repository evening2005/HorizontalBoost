#pragma once

#include "pge_sprite.h"

// STEER_AMOUNT must remain 1 for the AI steering to work properly
#define STEER_AMOUNT (1)
#define STEER_SLOWDOWN (0)
#define REAR_END_PENALTY (1)

#define BOOST_COOLDOWN_MILLIS (2048)
#define BOOST_MINIMUM_COOLDOWN (BOOST_COOLDOWN_MILLIS >> 1)

#define SPEED_INC (6)
#define CRASH_SLOWDOWN (1)
#define BOOST_INC (19)
#define DRAG_DIVISOR (21)

#define MAX_NPCS (8)

#define min(a, b) ((a) <= (b) ? (a) : (b))


#define CAR_LENGTH (30)
#define CAR_WIDTH (17)

#define PASSING_CLEARANCE (2)
#define NO_STEERING_PLAN    (-9999)

struct CAR {
    uint8_t carNumber;
    PGESprite *sprite;
    GPoint startingPosition;
    GPoint worldPosition;
    uint32_t rank;
    bool boosting;
    uint16_t currentSpeed; 
    uint64_t finished;
    int steeringPlan; // This is a target for worldPosition.y
    time_t boostStartMillis;
    time_t boostDurationMillis;
    time_t maxBoostDurationMillis;
    time_t lastBoostMillis;
    GColor8 carColour;
    char carName[12];
};

typedef struct CAR carType;

void position_car(carType *car);
void initialise_car(carType *carPtr, int resourceID, GColor colour, char *name);
void set_player_car(carType *pc);
int get_camera_focus();
void draw_cars(GContext *ctx);
void car_frame_update();

void place_cars_on_grid();
void set_up_starting_positions();
GPoint get_starting_position(uint32_t num);
void delete_cars();
void car_add_to_grid(carType *carPtr);
void set_race_start_time();
void update_boost_ui(GContext *ctx);
uint64_t get_milli_time();
// This is called from raceresult.c
carType *get_finisher(int rank);

/*

void set_up_steering_guide();

void make_pos_gap_ui(Window *window);
void destroy_pos_gap_ui();

*/
