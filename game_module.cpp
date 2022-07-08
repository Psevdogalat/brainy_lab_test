#include <engine.h>

#include <objects.h>
#include <utilits.h>
#include <keys.h>
#include <controllers.h>

#include <vector_utilits.h>

#include <cstdlib>
#include <cstdio>
#include <list>
#include <cmath>

using namespace GAME_ENGINE;

static const double rot_angle_quant = M_PI/32;
double 			camera_scale		= 0.05;
VECTOR2D 		camera_proportion	= {1.5, 1.0};
VECTOR2D 		mouse_last	 		= {0.0, 0.0};
VECTOR2D 		mouse_coord 		= {0.0, 0.0};

bool 			camera_focus 		= false;
bool			draw_collisions		= true;

GAME_OBJECT* grid;

PLAYER_CONTROLLER* 				player_controller;
ARTIFICIAL_PLAYER_CONTROLLER*	artificial_player_controller;

GAME_OBJECT* 		player_spawn;
GAME_OBJECT* 		enemy_spawn;

GAME_OBJECT_PLAYER* player;
RGB_COLOR			player_color(0.0f, 1.0f, 0.0f);

GAME_OBJECT_PLAYER* enemy;
RGB_COLOR			enemy_color(1.0f, 0.0f, 0.0f);

TIMER* start_timer;
double start_time	=	0.5;
bool   round_begin;

UINT   counter_player;
UINT   counter_enemy;

void build_walls(){
	VECTOR2D 		p_coord; 	
	VECTOR2D 		e_coord;	
	
	p_coord = player_spawn->get_position();
	e_coord = enemy_spawn->get_position();
	
	struct wall_info{
		VECTOR2D	position;
		VECTOR2D	normal;
		double		width;
		double 		height;
	}walls_info[] = {
		{p_coord + vector2d( 0.00, 1.25)	,create_normal() 	, 3.0, 0.5},
		{p_coord + vector2d( 0.00,-1.25)	,create_normal() 	, 3.0, 0.5},
		{p_coord + vector2d(-1.25, 0.00)	,create_normal() 	, 0.5, 2.0},
		{e_coord + vector2d( 0.00, 1.25)	,create_normal() 	, 3.0, 0.5},
		{e_coord + vector2d( 0.00,-1.25)	,create_normal() 	, 3.0, 0.5},
		{e_coord + vector2d( 1.25, 0.00)	,create_normal() 	, 0.5, 2.0},
		
		{vector2d( 0.0, 0.0)				, vector2d(1.0, 0.0), 1.0, 3.0},
		{vector2d( 3.0, 5.0)				, vector2d(1.0, 0.0), 2.0, 2.0},
		{vector2d(-3.0,-5.0)				, vector2d(1.0, 0.0), 2.0, 2.0},
		{vector2d(-5.0, 5.0)				, vector2d(1.0, 1.0), 4.0, 2.0},
		{vector2d( 5.0,-5.0)				, vector2d(1.0, 1.0), 4.0, 2.0},
		
		{vector2d( 0.0  , 8.75)				, create_normal(), 30.0, 0.5},
		{vector2d( 0.0  ,-8.75)				, create_normal(), 30.0, 0.5},
		{vector2d(-14.75, 0.0)				, create_normal(), 0.5 , 17.0},
		{vector2d( 14.75, 0.0)				, create_normal(), 0.5 , 17.0}
	};
	
	for(UINT i = 0; i < (sizeof(walls_info)/sizeof(wall_info)); i++)
		spawn(
			new GAME_OBJECT_WALL(walls_info[i].width, walls_info[i].height, RGB_COLOR(0.0f, 0.5f, 0.0f)), 
			walls_info[i].position, 
			walls_info[i].normal
		);
	
}

void init_game(){
	start_timer = allocate_timer();
	start_timer->set(start_time);
	
	round_begin 	= true;
	counter_enemy 	= 0;
	counter_player	= 0;
	
	player 	= nullptr;
	enemy	= nullptr;
}	

void on_player_dead	(GAME_OBJECT* );
void on_enemy_dead	(GAME_OBJECT* );

void spawn_player(){
	player 	= (GAME_OBJECT_PLAYER*) spawn(new GAME_OBJECT_PLAYER(), player_spawn->get_position(), player_spawn->get_normal());
	player->set_color(player_color);
	player->set_on_dead(on_player_dead);
	player_controller->set_control_unit(player);
	artificial_player_controller->set_enemy_unit(player);
}

void spawn_enemy(){
	enemy 	= (GAME_OBJECT_PLAYER*) spawn(new GAME_OBJECT_PLAYER(), enemy_spawn->get_position(),enemy_spawn->get_normal());
	enemy->set_color(enemy_color);
	enemy->set_on_dead(on_enemy_dead);
	artificial_player_controller->set_control_unit(enemy);

}

void start_round(){
	if(!player)
		spawn_player();
	//player->set_invul(true);
	
	if(!enemy)
		spawn_enemy();
	//enemy->set_invul(true);
	
}

void end_round(){
	round_begin = true;
	start_timer->set(start_time);
	/*
	despawn(player);
	despawn(enemy);
	*/
	
	
	
	printf("Player % d Enemy % d\n",counter_player, counter_enemy);

}

void on_player_dead(GAME_OBJECT* Killer){
	GAME_OBJECT_BULLET* bullet;
	if(Killer->get_type() == GOT_BULLET){
		bullet = (GAME_OBJECT_BULLET*) Killer;
		if(bullet->get_owner() == player){
			counter_player--;
		}
		
		if(bullet->get_owner() == enemy){
			counter_enemy++;
		}
		
		player_controller->set_control_unit(nullptr);
		artificial_player_controller->set_enemy_unit(nullptr);
		player = nullptr;
	}
	
	end_round();
}

void on_enemy_dead(GAME_OBJECT* Killer){
	GAME_OBJECT_BULLET* bullet;
	if(Killer->get_type() == GOT_BULLET){
		bullet = (GAME_OBJECT_BULLET*) Killer;
		if(bullet->get_owner() == enemy){
			counter_enemy--;
		}
		
		if(bullet->get_owner() == player){
			counter_player++;
		}
		
		artificial_player_controller->set_control_unit(nullptr);
		enemy = nullptr;
	}
	
	end_round();
}

void print_v(const VECTOR2D& V ){
	printf("V %f %f \n", V.x, V.y);
}

void ENGINE::init_scene(){
	
	CAMERA& scene_camera = scene.camera;	

	camera_scale = 0.05;
	scene_camera.set_area(VECTOR_RECTANGLE(vector2d(0,0), create_normal(), 3, 2));
	scene_camera.set_scale(camera_scale);

	init_key_map();
	
	set_draw_collision_models(draw_collisions);

	player_controller 				= new PLAYER_CONTROLLER();
	artificial_player_controller	= new ARTIFICIAL_PLAYER_CONTROLLER();
	
	grid 		 = spawn(new METRIC_GRID(40.0, 20.0), vector2d(0,0), create_normal());
	grid->set_visible(false);
	
	player_spawn = spawn(new GAME_OBJECT_POINT(), vector2d(-10,0), create_normal());
	enemy_spawn  = spawn(new GAME_OBJECT_POINT(), vector2d( 10,0),-create_normal());
	
	build_walls();
	
	init_game();

}

void ENGINE::free_scene(){
	
	delete player_controller;
	delete artificial_player_controller;
}

void ENGINE::compute(double Frame_time){
	VECTOR2D position;
	VECTOR2D normal;
	
	if(round_begin && start_timer->condition()){
		start_round();
		round_begin = false;
	}
	
	if(key_front_down(KEY_X)){
		draw_collisions = !draw_collisions;
		set_draw_collision_models(draw_collisions);
	}
	
	if(key_front_down(KEY_Z)){
		grid->set_visible(!grid->is_visible());
	}
	
	if(key_state(KEY_ARROW_LEFT)){
		scene.camera.set_normal(
			rotate_vector(
				scene.camera.get_normal(), 
				rot_angle_quant
			)
		);
	}
	
	if(key_state(KEY_ARROW_RIGHT)){
		scene.camera.set_normal(
			rotate_vector(
				scene.camera.get_normal(), 
				-rot_angle_quant
			)
		);
	}
	
	if(key_front_down(KEY_C)){
		if(camera_focus == true){
			scene.camera.set_position(vector2d(0.0, 0.0));
			camera_focus = false;
		}else
			camera_focus = true;
	}
	
	if(key_front_down(KEY_ESC)){
		platform_terminate();
		
	}
	
	player_controller->compute();
	artificial_player_controller->compute();
	
}

void ENGINE::pre_render(){
	if(camera_focus){
		if(player)
			scene.camera.set_position	(player->get_position());
	
	}
	//scene.camera.set_normal	(player->get_normal());
}

void ENGINE::event(const EVENT_MSG& Event){
	const EVENT_KEY_RECORD& 	key_record 		= Event.event_record.key;
	const EVENT_MOUSE_RECORD& 	mouse_record 	= Event.event_record.mouse;
	
	unsigned int key;
	
	switch(Event.event){
		case EV_KEY_PRESS:
			
			key = find_key_by_code(key_record.key_code);
			if(key != INVALID_KEY){
				if(key_record.key_down == true){
					toggle_key_down(key);
				}else
					toggle_key_up(key);
			}else{
				printf("KEY_PRESS %s KEY_CODE %d\n", (key_record.key_down == true)? "DOWN":"UP", key_record.key_code);
			}
							
		break;
		case EV_MOUSE_WHEEL:
			
			if(mouse_record.delta > 0 && camera_scale < 1.0){
				camera_scale+= 0.01;
				scene.camera.set_scale(camera_scale);
			}else if(mouse_record.delta < 0 && camera_scale > 0.05){
				camera_scale-= 0.01;
				scene.camera.set_scale(camera_scale);
			}
			
			
		break;
		case EV_MOUSE_MOVE:
			mouse_coord = scale_vector(vector2d(mouse_record.x , mouse_record.y),camera_proportion);
			if(key_state(KEY_MOUSE_M)){
				VECTOR2D position;
				if(!camera_focus){
					position = scene.camera.get_position();
					position -= scale_vector(mouse_coord - mouse_last,0.5/camera_scale);
					scene.camera.set_position(position);
				}
				mouse_last = mouse_coord;
			}
		break;
		case EV_MOUSE_MKEY_DOWN:
			toggle_key_down(KEY_MOUSE_M);
			mouse_coord = scale_vector(vector2d(mouse_record.x , mouse_record.y),camera_proportion);
			mouse_last = mouse_coord;
		break;
		case EV_MOUSE_MKEY_UP:
			toggle_key_up(KEY_MOUSE_M);
		break;
		case EV_MOUSE_LKEY_DOWN:
			toggle_key_down(KEY_MOUSE_L);
		break;
		case EV_MOUSE_LKEY_UP:
			toggle_key_up(KEY_MOUSE_L);
		break;
		case EV_MOUSE_RKEY_DOWN:
			toggle_key_down(KEY_MOUSE_R);
		break;
		case EV_MOUSE_RKEY_UP:
			toggle_key_up(KEY_MOUSE_R);
		break;
	}
}


