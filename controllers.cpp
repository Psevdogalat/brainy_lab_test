#include <controllers.h>
#include <keys.h>
#include <cstdlib>
#include <cstdio>


CONTROLLER::CONTROLLER(){};
CONTROLLER::~CONTROLLER(){};

/* class PLAYER_CONTROLLER */
//=================================================================================================
PLAYER_CONTROLLER::PLAYER_CONTROLLER(){
	control_unit = nullptr;
}

PLAYER_CONTROLLER::~PLAYER_CONTROLLER(){
	
}

void PLAYER_CONTROLLER::init(){
	
}

void PLAYER_CONTROLLER::compute(){
		
	if(control_unit != nullptr){
		if(key_state(KEY_Q))
			control_unit->rotate_left();

		if(key_state(KEY_E))
			control_unit->rotate_right();
		
		if(key_state(KEY_W))
			control_unit->move_forward(); 
		
		if(key_state(KEY_S))
			control_unit->move_backward();
		
		if(key_state(KEY_SPACE))
			control_unit->fire();
		
	}
}

void PLAYER_CONTROLLER::set_control_unit(GAME_OBJECT_PLAYER* Control_unit){
	control_unit = Control_unit;
}

/* class ARTIFICIAL_PLAYER_CONTROLLER */
//=================================================================================================

ARTIFICIAL_PLAYER_CONTROLLER::ARTIFICIAL_PLAYER_CONTROLLER(){
	control_unit 	= nullptr;
	enemy_unit		= nullptr;
	
	dummy_timer = allocate_timer();
	dummy_time 	= 2.0;
	srand(0);
}

ARTIFICIAL_PLAYER_CONTROLLER::~ARTIFICIAL_PLAYER_CONTROLLER(){
	free_timer(dummy_timer);
}

void ARTIFICIAL_PLAYER_CONTROLLER::init(){
	
}

void ARTIFICIAL_PLAYER_CONTROLLER::compute(){
	VECTOR2D	enemy_position;
	VECTOR2D	own_position;
	VECTOR2D 	direction_to_enemy;
	VECTOR2D 	own_direction;
	double 		distance;
	
	if(control_unit != nullptr){
		
		if(key_state(KEY_A))
			control_unit->fire();
		
		if(enemy_unit != nullptr){
			own_position 		= control_unit->get_position();
			enemy_position		= enemy_unit->get_position();
			own_direction 		= control_unit->get_normal();
			direction_to_enemy	= normalize_vector(enemy_position - own_position);
			
			distance = vector_length(enemy_position - own_position);
			
			if( (1.0 - scalar_product2d(own_direction, direction_to_enemy)) > 0.001)
				if(vector_product2d(own_direction, direction_to_enemy) > 0 ){
					control_unit->rotate_left();
				}else{
					control_unit->rotate_right();
				}
			
			if( distance > 4){
				if(scalar_product2d(own_direction, direction_to_enemy) > 0){
					control_unit->move_forward();
				}else
					control_unit->move_backward();
			}
			
			if( distance < 3.5){
				if(scalar_product2d(own_direction, direction_to_enemy) < 0){
					control_unit->move_forward();
				}else
					control_unit->move_backward();
			}
		}
		
		if(dummy_timer->condition()){
			dummy_time = ((double)rand()/(double)RAND_MAX) * 5 + 0.5;
			if(dummy_time < 1){
				//printf("ARTIFICAL_PLAYER: A!! aa!\n");
			
			}else
				//printf("ARTIFICAL_PLAYER: AaaAAa... uauuuUu...\n");
			
			dummy_timer->set(dummy_time);
		}
	}
	
}

void ARTIFICIAL_PLAYER_CONTROLLER::set_control_unit(GAME_OBJECT_PLAYER* Control_unit){
	control_unit = Control_unit;
}

void ARTIFICIAL_PLAYER_CONTROLLER::set_enemy_unit(GAME_OBJECT_PLAYER* Enemy_unit){
	enemy_unit = Enemy_unit;
}
