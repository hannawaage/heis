//
//  fsm.c
//  Heis
//
//  Created by Hanna Hjelmeland on 28/03/2019.
//  Copyright © 2019 Hanna Hjelmeland. All rights reserved.
//

#include <stdio.h>
#include "fsm.h"
#include "order.h"
#include "timer.h"
#include "elev.h"



void fsm_emergency_handler() {
    elev_set_stop_lamp(1);
    elev_set_motor_direction(DIRN_STOP);
    
    for(int i = 0; i < N_FLOORS; i++) {
        order_erase_order(i);
    }
    
    //Hvis i etasje og døren er lukket, åpne døren
    if((elev_get_floor_sensor_signal() != (-1)) && timer_timeout()) {
        fsm_open_door();
    }
    
    while(elev_get_stop_signal());
    elev_set_stop_lamp(0);
}

void fsm_open_door() {
    timer_start();
    elev_set_door_open_lamp(1);
}


void fsm_timeout() {
    elev_set_door_open_lamp(0);
    timer_stop(); 
}

void fsm_order_in_current_floor() {
    order_erase_order(elev_get_floor_sensor_signal());
    fsm_open_door();
}

void fsm_start_moving() {
    elev_motor_direction_t prev_direction = direction;
    direction = order_get_dir(elev_get_floor_sensor_signal(), prev_direction);
    elev_set_motor_direction(direction);
}

void fsm_order_in_last_floor() {
    if(direction == DIRN_DOWN) {
        elev_set_motor_direction(DIRN_UP);
    }
    else {
        elev_set_motor_direction(DIRN_DOWN);
    }
}
