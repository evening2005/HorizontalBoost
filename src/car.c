

#include "pge.h"
#include "car.h"
#include "track.h"
#include "pge_collision.h"
#include "statemachine.h"

/*

static carType *playerCar;
static carType *startingGrid[MAX_NPCS + 1]; 
static uint16_t howManyNPCs = 0;
static carType *sortedGrid[MAX_NPCS + 1];
static carType *finishingOrder[MAX_NPCS + 1];

// rank is numbered from 0
carType *get_finisher(int rank) {
    if(rank < 0) rank = 0;
    if(rank > MAX_NPCS) rank = MAX_NPCS;
    return finishingOrder[rank];
}

static uint64_t raceStartTime = 0;

uint64_t get_milli_time() {
    time_t seconds;
    uint16_t millis;
    time_ms(&seconds, &millis);
    return (seconds * 1000 + millis);          
}


static int cameraFocus;

void set_camera_focus() {
    if(playerCar->finished == 0) {
        cameraFocus = playerCar->worldPosition.y;    
    } else {
        cameraFocus = TRACK_FINISH_LINE - 50;
    }
}

int get_camera_focus() {
    return cameraFocus;
}


void sort_cars() {
    int c=0;
    for(c=0; c <= howManyNPCs; c++) {
        sortedGrid[c] = startingGrid[c];
    }
    int i, j;
    carType *temp;
    for(i=0; i < howManyNPCs; i++) {
        for(j=i+1; j < howManyNPCs + 1; j++) {
            if(sortedGrid[j]->worldPosition.y > sortedGrid[i]->worldPosition.y) {
                temp = sortedGrid[i]; 
                sortedGrid[i] = sortedGrid[j];
                sortedGrid[j] = temp;
            }
        }
    }
    for(c=0; c <= howManyNPCs; c++) {
        sortedGrid[c]->rank = c;
    }
}

void sort_cars_by_finishing_time() {
    int c=0;
    for(c=0; c <= howManyNPCs; c++) {
        finishingOrder[c] = startingGrid[c];
    }
    int i, j;
    carType *temp;
    for(i=0; i < howManyNPCs; i++) {
        for(j=i+1; j < howManyNPCs + 1; j++) {
            if(finishingOrder[j]->finished < finishingOrder[i]->finished) {
                temp = finishingOrder[i]; 
                finishingOrder[i] = finishingOrder[j];
                finishingOrder[j] = temp;
            }
        }
    }
    for(c=0; c <= howManyNPCs; c++) {
        finishingOrder[c]->rank = c;
    } 
}


TextLayer *positionAndGap = NULL;

void make_pos_gap_ui(Window *window) {
    if(positionAndGap == NULL) {
        GRect greyFrame = { {16, 152} , {112, 16} };
        Layer *windowLayer = window_get_root_layer(window);
        positionAndGap = text_layer_create(greyFrame);
        text_layer_set_background_color(positionAndGap, GColorLightGray);
        text_layer_set_text_alignment(positionAndGap, GTextAlignmentLeft);   
        layer_add_child(windowLayer, (Layer *)positionAndGap);      
    }
}

void destroy_pos_gap_ui() {
    text_layer_destroy(positionAndGap);
}

static char posGapText[32];
static bool updateOnceAfterFinishing = false;

void update_position_ui() {
    if(updateOnceAfterFinishing) return;
    
    int gap;
    if(sortedGrid[0] == playerCar) {
        text_layer_set_text_color(positionAndGap, GColorMintGreen);
        gap = sortedGrid[0]->worldPosition.y - sortedGrid[1]->worldPosition.y;
    } else {
        text_layer_set_text_color(positionAndGap, GColorDarkCandyAppleRed);
        gap = sortedGrid[0]->worldPosition.y - playerCar->worldPosition.y;
    }
    int gapMetres = gap / 10;
    snprintf(posGapText, 30, "%2d (%03d)", (int)playerCar->rank+1, gapMetres);        

    text_layer_set_text(positionAndGap, posGapText);

    if(playerCar->finished) {
        updateOnceAfterFinishing = true;   
    }
}

void car_add_to_grid(carType *carPtr) {
    if(carPtr == playerCar) {
        startingGrid[0] = carPtr;
        carPtr->carNumber = 1;
    } else {
        if(howManyNPCs >= MAX_NPCS) return;
        howManyNPCs++;
        startingGrid[howManyNPCs] = carPtr;
        carPtr->carNumber = howManyNPCs + 1;
    }
}



// The "player Car" is the one at the centre of the window
void set_player_car(carType *pc) {
    playerCar = pc;
    car_add_to_grid(pc);
}

void delete_cars() {
    int c;
    carType *carPtr;
    for(c=0; c <= howManyNPCs; c++) {
        carPtr = startingGrid[c];
        pge_sprite_destroy(carPtr->sprite);
    }
}

void draw_cars(GContext *ctx) {
    int c;
    carType *carPtr;

    //draw_finish_line(ctx, cameraFocus);
    for(c=0; c <= howManyNPCs; c++) {
        carPtr = startingGrid[c];
        pge_sprite_draw(carPtr->sprite, ctx);
    }
}



void make_car_rect(const carType *carPtr, GRect *tempRect) {
    tempRect->origin = carPtr->worldPosition;
    tempRect->size.w = CAR_WIDTH;
    tempRect->size.h = CAR_LENGTH;
}


// This checks that we are able to steer to the left or the right
//  (ie it checks to be sure that there is no car in the way)
int car_steer(carType *carPtr, int amount) {
    GRect rectUs, rectThem;
    make_car_rect(carPtr, &rectUs);
    rectUs.origin.x += amount;

    int c;
    for(c=0; c <= howManyNPCs; c++) {
        if(startingGrid[c] == carPtr) continue;
        make_car_rect(startingGrid[c], &rectThem);
        if(pge_collision_rectangle_rectangle(&rectUs, &rectThem)) {
            // A car is in the way; we can't steer in the direction
            //  we want to go
            carPtr->currentSpeed -= CRASH_SLOWDOWN;
            return carPtr->worldPosition.x;
        }        
    }
    // If we get here, it means no car is in the way,
    // so we can steer in the direction we want to go
    carPtr->currentSpeed -= STEER_SLOWDOWN;
    return rectUs.origin.x;
}


#define BOOST_RANKS (10)
static uint32_t permittedBoostTimes[BOOST_RANKS] = {0, 1250, 2000, 3000, 4000, 4500, 4750, 5000, 5250, 5500};

int get_permitted_boost_time(carType *carPtr) {
    uint32_t r = carPtr->rank;
    if(r >= BOOST_RANKS) r = BOOST_RANKS-1;
    
    return permittedBoostTimes[r];
}

static GRect boostUIOuter = { {38, 2}, {66, 11} };
static GRect boostUIInner = { {39, 3}, {64,  9} };

void update_boost_ui(GContext *ctx) {
    // The boost ui is just a rectangle reflecting how boost times
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_draw_rect(ctx, boostUIOuter);
    if(playerCar->finished) return;
    
    if(playerCar->boosting) {
        // The whole bar is green. The length reflects the proportion
        //  of the boostDurationMillis that remains
        int boostRemaining = playerCar->boostDurationMillis - (get_milli_time() - playerCar->boostStartMillis);
        if(boostRemaining < 0) boostRemaining = 0;
        int16_t barLength = (boostRemaining * 64) / playerCar->maxBoostDurationMillis;
        boostUIInner.size.w = barLength;
        graphics_context_set_fill_color(ctx, GColorOxfordBlue);    
        graphics_fill_rect(ctx, boostUIInner, 0, 0);
    } else {
        int sinceLastBoost = min((get_milli_time() - playerCar->lastBoostMillis), BOOST_COOLDOWN_MILLIS);
        uint32_t barLength = (sinceLastBoost * 64) / BOOST_COOLDOWN_MILLIS;
        if((sinceLastBoost < BOOST_MINIMUM_COOLDOWN) || (playerCar->rank == 0)) graphics_context_set_fill_color(ctx, GColorSunsetOrange);
        else graphics_context_set_fill_color(ctx, GColorOxfordBlue);
        boostUIInner.size.w = (int16_t)barLength;
        graphics_fill_rect(ctx, boostUIInner, 0, 0);
    }
}

void check_boost_status(carType *carPtr) {
    if(!carPtr->boosting) return;
    carPtr->lastBoostMillis = get_milli_time();
    time_t howLongBoosting = carPtr->lastBoostMillis - carPtr->boostStartMillis;
    if(howLongBoosting >= carPtr->boostDurationMillis) {
        // We are out of boost!
        carPtr->boosting = false;
    }
    
}

void switch_on_boost(carType *carPtr) {
    // We can't boost if we're in the lead..
    if(carPtr->rank == 0) return;
    // We can't boost if we are currently boosting!
    if(carPtr->boosting) return;
    // How long since we last boosted?
    uint64_t timeSinceBoost = get_milli_time() - carPtr->lastBoostMillis;
    // We must wait a little bit before boosting again!
    if(timeSinceBoost < BOOST_MINIMUM_COOLDOWN) return;
    // This is the MAXIMUM we can have
    time_t maxBoostDuration = permittedBoostTimes[carPtr->rank];
    carPtr->maxBoostDurationMillis = maxBoostDuration;
    uint64_t normalizedTSB = min(timeSinceBoost, BOOST_COOLDOWN_MILLIS);
    uint64_t boostDuration = (maxBoostDuration * normalizedTSB) / BOOST_COOLDOWN_MILLIS;
    carPtr->boostDurationMillis = boostDuration;
    if(carPtr == playerCar) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Rank: %d - boost duration: %d", (int)carPtr->rank+1, (int)boostDuration);
    }
    carPtr->boostStartMillis = get_milli_time();
    carPtr->lastBoostMillis = carPtr->boostStartMillis;
    carPtr->boosting = true;
}

void car_handle_buttons(carType *carPtr) {
    // This is only called if this is the player car
    bool left = pge_get_button_state(BUTTON_ID_UP);
    if(left) {
        if( (carPtr->worldPosition.x) > -TRACK_KERB) {
            carPtr->worldPosition.x = car_steer(carPtr, -STEER_AMOUNT);
        } else {
            // Penalise scraping the wing!
            carPtr->currentSpeed -= CRASH_SLOWDOWN;
        }
    }
    bool right = pge_get_button_state(BUTTON_ID_DOWN);
    if(right) {
        if( (carPtr->worldPosition.x) < TRACK_KERB) {
            carPtr->worldPosition.x = car_steer(carPtr, +STEER_AMOUNT);
        } else {
            // Penalise scraping the wing!
            carPtr->currentSpeed -= CRASH_SLOWDOWN;
        }
    }
    bool boost = pge_get_button_state(BUTTON_ID_SELECT);
    if(boost) {
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "Switching on boost");
        // This only switches it on if it is possible to do so
        switch_on_boost(carPtr);
        if(carPtr->boosting == false) {
            // This means we tried to switch on boost but failed
            //  So we "punish"" the driver by pushing back the next boost time
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "PUNISHING BOOST FAILURE!!!");
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d : %d : %d", (int)(get_milli_time() - carPtr->lastBoostMillis), (int)BOOST_MINIMUM_COOLDOWN, (int)BOOST_COOLDOWN_MILLIS);
            carPtr->lastBoostMillis = get_milli_time();            
        }
    }
}

void ai_steering(carType *carPtr) {
    if(carPtr->rank != 0) {
        // Try to steer so we can overtake the car in front!
        carType *carInFront = sortedGrid[carPtr->rank-1];
        if((carInFront->worldPosition.y - carPtr->worldPosition.y) < 200) {
            // We have to worry about the car in front
            int diffFront = (carInFront->sprite->position.x - carPtr->sprite->position.x);
            if(abs(diffFront) <= (CAR_WIDTH+1)) {
                if(diffFront < 0) {
                    // Car in front is to our left, so steer right (but not if too close to barrier)
                    if(carInFront->sprite->position.x >= (TRACK_CENTRE_LINE + (CAR_WIDTH/2))) {
                        carPtr->worldPosition.x = car_steer(carPtr, -STEER_AMOUNT);
                    } else {
                        carPtr->worldPosition.x = car_steer(carPtr, +STEER_AMOUNT);
                    }
                } else if(diffFront > 0) {
                    // Car in front is to our right, so steer left (but not if too close to barrier)
                    if(carInFront->sprite->position.x <= (IBL + CAR_WIDTH)) {
                        carPtr->worldPosition.x = car_steer(carPtr, +STEER_AMOUNT);
                    } else {
                        carPtr->worldPosition.x = car_steer(carPtr, -STEER_AMOUNT);
                    }
                } else {
                    // We are directly behind, so head towards the centre
                    if(carPtr->sprite->position.x <= TRACK_CENTRE_LINE) {
                        carPtr->worldPosition.x = car_steer(carPtr, +STEER_AMOUNT);                    
                    } else {
                        carPtr->worldPosition.x = car_steer(carPtr, -STEER_AMOUNT);
                    }
                }      
            }
        }
    }         
}

void car_handle_ai(carType *carPtr) {
    // This is only called if this is NOT the player car
    //  and only one in sixty-four times on average!!
    ai_steering(carPtr);
    if((rand() & 3) == 0) switch_on_boost(carPtr);      
}


bool car_check_forward_movement(const carType *carPtr, int amount) {
    GRect rectUs, rectThem;
    make_car_rect(carPtr, &rectUs);
    rectUs.origin.y += amount;

    int c;
    for(c=0; c <= howManyNPCs; c++) {
        if(sortedGrid[c] == carPtr) break; // The rest of the cars are behind us!
        make_car_rect(sortedGrid[c], &rectThem);
        
        if(pge_collision_rectangle_rectangle(&rectUs, &rectThem)) {
            // A car is in the way; we can't move forward as far as
            //  we want to go
            return false;
        }        
    }
    // If we get here, it means no car is in the way,
    // so we can steer in the direction we want to go
    return true;
}

uint32_t calculate_drag(carType *carPtr) {
    uint32_t drag = carPtr->currentSpeed * carPtr->currentSpeed;
    drag = drag >> DRAG_DIVISOR;
    return drag;
}



void car_movement(carType *carPtr) {
    if(carPtr->worldPosition.y > TRACK_FINISH_LINE + 250) {
        return;    
    }
    
    if(carPtr->boosting) {
        carPtr->lastBoostMillis = get_milli_time();
        carPtr->currentSpeed += BOOST_INC;
        // Here we make sure that moving foward at the current speed will not cause
        //  us to crash into a car in front. 
        bool canMove = car_check_forward_movement(carPtr, carPtr->currentSpeed >> 8);
        if(!canMove) {
            carPtr->currentSpeed -= BOOST_INC;
            carPtr->currentSpeed += SPEED_INC;            
        }
        int tempSpeed = carPtr->currentSpeed;
        while(!canMove) {
            tempSpeed -= REAR_END_PENALTY;
            canMove = car_check_forward_movement(carPtr, tempSpeed >> 8);
        }   
        carPtr->worldPosition.y += (tempSpeed >> 8);     
    } else {
        carPtr->currentSpeed += SPEED_INC;
        bool canMove = car_check_forward_movement(carPtr, carPtr->currentSpeed >> 8);
        if(!canMove) {
            carPtr->currentSpeed -= SPEED_INC;
            carPtr->currentSpeed += (SPEED_INC >> 1);            
        }
        int tempSpeed = carPtr->currentSpeed;
        while(!canMove) {
            tempSpeed -= REAR_END_PENALTY;
            canMove = car_check_forward_movement(carPtr, tempSpeed >> 8);
        }   
        carPtr->worldPosition.y += (tempSpeed >> 8);     
    }
    carPtr->currentSpeed -= calculate_drag(carPtr);
}


#define PLAYERFIRSTSCREENY (40)
#define PLAYERLASTSCREENY (112)
#define PLAYERMIDSCREENY (76)
static int playerScreenPosY = PLAYERMIDSCREENY;

// Where the player car "sits" on the screen depends what position it is in
//  If the player is leading, the player car sits near the top of the screen
//  If the player is in last, the player car sits near the bottom..
//  This gives us more chance to keep the other cars "in view"
int player_screen_position() {
    static uint32_t counter = 0; // used to slow down transition in screenPosY
    counter++;
    
    if((counter & 3) == 0) {
        if((get_milli_time() - raceStartTime) < 2200) {
            return PLAYERMIDSCREENY;
        }
        if(playerCar->rank == 0) { // Player is in the lead
            if(playerScreenPosY > PLAYERFIRSTSCREENY) {
                playerScreenPosY -= 1;
            } else if(playerScreenPosY < PLAYERFIRSTSCREENY) {
                playerScreenPosY += 1;
            }
        } else if(playerCar->rank == howManyNPCs) { // Oh dear! Player is last
            if(playerScreenPosY < PLAYERLASTSCREENY) {
                playerScreenPosY += 1;
            } else if(playerScreenPosY > PLAYERLASTSCREENY) {
                playerScreenPosY -= 1;
            }
        } else { // Player is somewhere in the middle of the field...
            if(playerScreenPosY < PLAYERMIDSCREENY) {
                playerScreenPosY += 1;
            } else if(playerScreenPosY > PLAYERMIDSCREENY) {
                playerScreenPosY -= 1;
            }
        }
    }

    return playerScreenPosY;
}


void drive_car(carType *carPtr) {
    if(carPtr == playerCar) {
        car_handle_buttons(carPtr);
    } else {
        car_handle_ai(carPtr);
    }
}



void check_for_finisher(carType *carPtr) {
    GRect carRect;

    carRect.origin = carPtr->sprite->position;
    carRect.size.w = CAR_WIDTH;
    carRect.size.h = CAR_LENGTH;
    if(car_crossed_line(carRect)) {
        if(carPtr->finished != 0) return;
        int ft = get_milli_time() - raceStartTime;
        carPtr->finished = ft;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "FINISHED: %d : %d - SPEED: %d - POSITION: %d", carPtr->carNumber, ft, carPtr->currentSpeed >> 8, carPtr->worldPosition.y);
   }
}


static AppTimer *endOfRaceTimer;

void show_results(void *data) {
    // Switching to this state will cause the results to appear
    layer_set_hidden((Layer *)positionAndGap, true);
    set_current_state(STATE_AFTERRACE);
}


bool all_cars_finished() {
    int c;
    for(c=0; c < MAX_NPCS; c++) {
        if(startingGrid[c]->finished == 0) return false;
    }
    // If we get here it means they've all finished
    sort_cars_by_finishing_time();
    return true;
}

void set_race_start_time() {
    raceStartTime = get_milli_time();
}



void car_frame_update() {
    set_camera_focus();
    
    sort_cars();
    update_position_ui();
    int c;
    carType *carPtr;
    for(c=0; c <= howManyNPCs; c++) {
        carPtr = sortedGrid[c];
        check_boost_status(carPtr);
        drive_car(carPtr);
        car_movement(carPtr);
        position_car(carPtr);
        check_for_finisher(carPtr);
    }
    if(all_cars_finished()) {
        // Switch to the results screen after a short delay
        endOfRaceTimer = app_timer_register(2000, (AppTimerCallback)show_results, NULL);
    }
}


#define XPOS0 (-72+IBL+INTER_GRID_GAP+INTER_GRID_GAP)
#define XPOS1 (XPOS0 + CAR_WIDTH + INTER_GRID_GAP)
#define XPOS2 (XPOS1 + CAR_WIDTH + INTER_GRID_GAP)
#define XPOS3 (XPOS2 + CAR_WIDTH + INTER_GRID_GAP)
#define XPOS4 (XPOS3 + CAR_WIDTH + INTER_GRID_GAP)

#define YPOS0 (0)
#define YPOS1 (CAR_LENGTH - INTER_GRID_GAP - INTER_GRID_GAP)

static GPoint startingPositions[GRID_WIDTH * GRID_DEPTH] = { {XPOS0,YPOS0-2}, {XPOS1,YPOS0-1}, {XPOS2,YPOS0}, {XPOS3,YPOS0+1}, {XPOS4,YPOS0+2},
                                                             {XPOS0,YPOS1-2}, {XPOS1,YPOS1-1}, {XPOS2,YPOS1}, {XPOS3,YPOS1+1}, {XPOS4,YPOS1+2} };

// Shuffle the starting positions
void shuffle_starting_positions() {
    int temp;
    for(int i=0; i < 50; i++) {
        // Pick two at random and swap them
        uint32_t r1 = rand() % (GRID_WIDTH * GRID_DEPTH);
        uint32_t r2 = rand() % (GRID_WIDTH * GRID_DEPTH);
        temp = startingPositions[r1].x;
        startingPositions[r1].x = startingPositions[r2].x;
        startingPositions[r2].x = temp;
    }
}


void set_up_starting_positions() {
    playerScreenPosY = PLAYERMIDSCREENY;
    shuffle_starting_positions();
}

GPoint get_starting_position(uint32_t num) {
    
    return startingPositions[num];
} 





// This is used to position the cars on the grid
// It can only be called after the startingGrid has been filled!
void place_cars_on_grid() {
    // This sets up the positions of the grid on the track
    set_up_starting_positions();
    playerCar->finished = 0;
    set_camera_focus();
    // Now go through each car and find its position on the starting line
    for(uint32_t c=0; c <= howManyNPCs; c++) {
        reset_car(startingGrid[c]);
        startingGrid[c]->startingPosition = get_starting_position(c);
        startingGrid[c]->worldPosition = startingGrid[c]->startingPosition;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "starting positions: %d : %d", startingGrid[c]->worldPosition.x, startingGrid[c]->worldPosition.y);
    }
    layer_set_hidden((Layer *)positionAndGap, false);
    updateOnceAfterFinishing = false;
}

*/

uint64_t get_milli_time() {
    time_t seconds;
    uint16_t millis;
    time_ms(&seconds, &millis);
    return (seconds * 1000 + millis);          
}


// Your xPos comes from your carType struct
//  But your yPos is defined by your yPos 
//  relative to playerCar 
void position_car(carType *carPtr) {
    GPoint screenPosition;
    screenPosition.x = 56; // = TRACK_CENTRE_LINE + carPtr->worldPosition.x - (CAR_WIDTH / 2);
    screenPosition.y = 80; // get_camera_focus() - carPtr->worldPosition.y + player_screen_position();
    carPtr->sprite->position = screenPosition;
}

// This is done at the start of each race
void reset_car(carType *carPtr) {
    carPtr->worldPosition = carPtr->startingPosition;
    carPtr->boosting = false;
    carPtr->boostStartMillis = get_milli_time();
    carPtr->lastBoostMillis = get_milli_time();
    carPtr->boostDurationMillis = 0;
    carPtr->rank = 1;
    carPtr->currentSpeed = 0;
    carPtr->finished = 0; 
    position_car(carPtr);
}



// This stuff gets done only once
void initialise_car(carType *carPtr, int resourceID, GColor colour, char *name) {
    GPoint pos;
    carPtr->sprite = pge_sprite_create(pos, resourceID);
    strncpy(carPtr->carName, name, 11);
    carPtr->carColour = colour;
    reset_car(carPtr);
}




