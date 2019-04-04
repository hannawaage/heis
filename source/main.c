#include "elev.h"
#include "order.h"
#include "fsm.h"
#include "timer.h"
#include <stdio.h>

typedef enum state_id { 
    idle = 0, //I ro uten bestillinger
    open_door, //I ro, åpne dør, sjekke bestilling
    moving, //Beveger seg mot prioritert bestilling
    emergency_stop
} state;


int main() {
    // Initialize hardware
    if (!elev_init()) {
        printf("Unable to initialize elevator hardware!\n");
        return 1;
    }     

    //I idle etter init
    state current_state = idle;
    elev_motor_direction_t direction = DIRN_STOP;

    while (1) {

        if(elev_get_stop_signal()) {
            current_state = emergency_stop;
        }
        
        order_update();
        
        //MINNE FOR Å HUSKE SIST ETASJE NÅR VI ER I BEVEGELSE
        int current_floor = elev_get_floor_sensor_signal();
        int last_floor; 
        if(current_floor != -1) {
            last_floor = current_floor;
            elev_set_floor_indicator(current_floor);
        }
        
        
        switch(current_state) {
            case(idle): {
                if(order_check_for_order()){
                    if(order_same_floor_order(current_floor)){
                        fsm_order_in_current_floor();
                        current_state = open_door;
                    }
                    else{
                        //HVIS NØDSTOPP TRYKKES MELLOM TO ETASJER
                        //MÅ SNU RETNING TILBAKE
                        if (order_same_floor_order(last_floor)) {
                            if(direction == DIRN_DOWN) {
                                elev_set_motor_direction(DIRN_UP);
                            }
                            else {
                                elev_set_motor_direction(DIRN_DOWN);
                            }
                        }
                        else {
                            elev_motor_direction_t prev_direction = direction;
                            direction = order_get_dir(current_floor, prev_direction);
                            elev_set_motor_direction(direction);
                            current_state = moving;
                        }
                        current_state = moving;
                    } 
                }
                //FORSIKRING 
                else {
                    current_state = idle;
                }
                break;
            }
            case(open_door) : {
                if(order_same_floor_order(current_floor)){
                    fsm_order_in_current_floor();
                    current_state = open_door;
                }
                if(timer_timeout()) {
                    fsm_timeout();
                    if(order_check_for_order()) {
                        if(order_same_floor_order(current_floor)){
                            fsm_order_in_current_floor();
                            current_state = open_door;
                        }
                        else {
                            elev_motor_direction_t prev_direction = direction;
                            direction = order_get_dir(current_floor, prev_direction);
                            elev_set_motor_direction(direction);
                            current_state = moving; 
                        }
                    }   
                    else {
                        current_state = idle;
                    }
                }
                //FORSIKRING
                else { current_state = open_door; }
                break;
            }
            case(moving) : {
                if(order_same_floor_order(current_floor) && order_is_order_same_dir(current_floor, direction)) {
                    elev_set_motor_direction(DIRN_STOP);
                    fsm_order_in_current_floor();
                    current_state = open_door; 
                }

                break;
            }
            case(emergency_stop): {
                fsm_emergency_handler();

                //HVIS I ETASJE, GÅ TIL OPEN DOOR FOR Å LUKKE DØREN
                if(elev_get_floor_sensor_signal() != -1) {
                    fsm_order_in_current_floor();
                    current_state = open_door;
                }
                else {
                    current_state = idle; 
                }
                break;
            }
            default : {
                return 0;
            }
        }

    }

    elev_set_motor_direction(DIRN_STOP);

    return 0;
}

