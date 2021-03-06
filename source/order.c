
#include "order.h"

int orders[12] = {0};

int order_check_for_order(){
    for(int i = 0; i < 12; i++)
    {
        if(orders[i]) {
			return 1;
		}
    }
    return 0; 
}

void order_place_order(elev_button_type_t button, int floor){
    int button_value = button;
    orders[N_BUTTONS*floor + button_value] = 1;
}

void order_update() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 4; j++){
            if(!((i == 0 && j == 3) || (i == 1 && j == 0))) {
                if(elev_get_button_signal(i, j)) {
                    order_place_order(i, j);
                    elev_set_button_lamp(i,j,1);
                }
            }
        }
    }
}

void order_erase_order(int floor){
    if(floor == -1) { 
		return; 
	}

    for(int i = 0; i < N_BUTTONS; i++)
    {
        if(floor == 0 && i == 1) { 
            continue; 
        }
        if(floor == 3 && i == 0) { 
            continue;  
        }
        orders[N_BUTTONS*floor + i] = 0;
        elev_set_button_lamp(i,floor,0);
    }
}

int order_same_floor_order(int floor){
    if(floor == -1) { 
		return 0; 
	}
	return (orders[N_BUTTONS*floor] || orders[N_BUTTONS*floor + 1] || orders[N_BUTTONS*floor + 2]);
}

int order_is_order_same_direction(int floor, elev_motor_direction_t dir) {
    if(orders[N_BUTTONS*floor +2]) {
        return 1; 
    }
    if((floor == 0) || (floor == 3)) {
        return 1; 
    }
    if((orders[N_BUTTONS*floor] && dir == DIRN_UP)) {
        return 1; 
    }
    if(orders[N_BUTTONS*floor + 1] && dir == DIRN_DOWN) {
        return 1;
    }
    return 0;
}

int order_only_one_order() {
    int num_orders = 0;
	for (int i = 0; i < 12; i++) {
		num_orders += orders[i];
	}
	if (num_orders == 1) {
		return 1;
	}
    return 0;
}

elev_motor_direction_t order_get_direction(int floor, elev_motor_direction_t last_direction) {
    if(last_direction == DIRN_UP){
        for(int i = 11; i >= 0; i--) {
            if(orders[i]) {          
                if(i > floor*3) {
                   return DIRN_UP; 
                }
                return DIRN_DOWN;                 
            } 
        }
    }
    for(int i = 0; i < 12; i++) {
        if(orders[i]) {
            if(i > floor*3) {
                return DIRN_UP; 
            }
        return DIRN_DOWN; 
        } 
    }
    return DIRN_STOP;
}
