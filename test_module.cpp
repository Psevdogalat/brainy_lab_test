#include <engine.h>

#include <objects.h>
#include <utilits.h>
#include <keys.h>
#include <controllers.h>

#include <vector_utilits.h>
#include <collisions2.h>

#include <cstdlib>
#include <cstdio>
#include <list>
#include <cmath>
#include <algorithm>

using namespace GAME_ENGINE;

void camera_movement();
void object_movement();

static CAMERA* 	 				camera;
static SCENE*					game_scene;
static GAME_OBJECT* 			grid;
static GAME_OBJECT* 			mov_object;
static std::list<GAME_OBJECT*> 	mov_list;

static GAME_OBJECT*				unit;
static GAME_OBJECT* 			ray_caster;
static GAME_OBJECT* 			ray;
static GAME_OBJECT* 			cursor;
static GAME_OBJECT* 			colliding_point;

static GAME_OBJECT_FILTER 		filter;
static GAME_OBJECT_FILTER 		stop_filter;

static GRAPHIC_MODEL_VECTOR*	ray_caster_model;
static GRAPHIC_MODEL_VECTOR*	ray_model;
static GRAPHIC_MODEL_VECTOR*	cursor_model;
static GRAPHIC_MODEL_VECTOR*	colliding_point_model;

static bool 		draw_collisions 	= false;

static double		camera_scale_max	= 1.0;
static double		camera_scale_min	= 0.05;
static double 		camera_scale		= 0.05;
static const double camera_rot_quant 	= M_PI/32;
static const double camera_mov_quant 	= 0.2;

static VECTOR2D 	camera_proportion 	= {0.0, 0.0};
static VECTOR2D 	mouse_last		   	= {0.0, 0.0};
static VECTOR2D 	mouse_coord	 		= {0.0, 0.0};

void build_walls(){
	GAME_OBJECT* wall;
	
	struct wall_info{
		VECTOR2D	position;
		VECTOR2D	normal;
		double		width;
		double 		height;
	}walls_info[] = {
		{vector2d( 3.0, 0.0)				, vector2d(1.0, 0.0), 1.0, 3.0},
		{vector2d( 3.0, 5.0)				, vector2d(1.0, 0.0), 2.0, 2.0},
		{vector2d(-3.0,-5.0)				, vector2d(1.0, 0.0), 2.0, 2.0},
		{vector2d(-5.0, 5.0)				, vector2d(1.0, 1.0), 4.0, 2.0},
		{vector2d( 5.0,-5.0)				, vector2d(1.0, 1.0), 4.0, 2.0},
		
		{vector2d( 0.0  , 8.75)				, create_normal(), 30.0, 0.5},
		{vector2d( 0.0  ,-8.75)				, create_normal(), 30.0, 0.5},
		{vector2d(-14.75, 0.0)				, create_normal(), 0.5 , 17.0},
		{vector2d( 14.75, 0.0)				, create_normal(), 0.5 , 17.0}
	};
	
	for(UINT i = 0; i < (sizeof(walls_info)/sizeof(wall_info)); i++){
		wall = spawn(
			new GAME_OBJECT_WALL(walls_info[i].width, walls_info[i].height, RGB_COLOR(0.0f, 0.5f, 0.0f)), 
			walls_info[i].position, 
			walls_info[i].normal
		);
		
		if(wall)
			mov_list.push_back(wall);
	}
	
}

void ENGINE::init_scene(){
	
	init_key_map();
	set_draw_graphic_models		(true	);
	set_draw_collision_models	(true	);
	
	
	scene.camera.set_area(VECTOR_RECTANGLE(vector2d(0,0), create_normal(), 1.5, 1.0));
	scene.camera.set_scale(camera_scale);
	camera_proportion = vector2d(1.5 , 1.0);
	camera 		= &scene.camera;
	game_scene  = &scene;
	
	grid = spawn(new METRIC_GRID(40.0, 20.0), vector2d(0,0), create_normal());
	grid->set_visible(true);
	
	build_walls();
	
	ray_caster = spawn(new GAME_OBJECT(), vector2d(-4.0, 2.0), rotate_vector(create_normal(), 2*M_PI/3));
	ray_caster_model = (GRAPHIC_MODEL_VECTOR*) create_arrow_model();
	ray_caster_model->push_color(RGB_COLOR(1.0f, 1.0f, 0.0f));
	ray_caster->set_graphic_model(ray_caster_model);
	mov_list.push_back(ray_caster);
	
	unit = spawn(new GAME_OBJECT_PLAYER(), vector2d(-8.0, 0.0), create_normal());
	mov_list.push_back(unit);
	stop_filter.pointers.push_back(unit);
	
	VECTOR_POLYLINE polyline(vector2d(0.0, 0.0), create_normal(), 10);
	ray = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
	ray_model = new GRAPHIC_MODEL_VECTOR();
	ray_model->set_vector_object(&polyline);
	ray_model->push_color(RGB_COLOR(0.0f, 1.0f, 1.0f));
	ray->set_graphic_model(ray_model);
	ray->set_visible(false);
	
	colliding_point = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
	colliding_point_model = (GRAPHIC_MODEL_VECTOR*) create_point_model();
	colliding_point_model->get_vector_object()->set_scale(0.2);
	colliding_point_model->push_color(RGB_COLOR(0.0f, 0.0f, 1.0f));
	colliding_point->set_graphic_model(colliding_point_model);
	colliding_point->set_visible(false);
	
	cursor = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
	cursor_model = (GRAPHIC_MODEL_VECTOR*) create_point_model();
	cursor_model->get_vector_object()->set_scale(0.2);
	cursor_model->push_color(RGB_COLOR(1.0f, 0.0f, 0.0f));
	cursor->set_graphic_model(cursor_model);
	cursor->set_visible(true);
	
	mov_object = ray_caster;
}

void ENGINE::free_scene(){
	delete ray_caster_model;
	delete ray_model;
	delete colliding_point_model;
	delete cursor_model;
	
}

void ENGINE::compute(double Frame_time){
	
	camera_movement();
	object_movement();
	
	if(key_front_down(KEY_B)){
		printf("Break point!\n");
	}
	
	if(key_front_down(KEY_X)){
		std::list<GAME_OBJECT*>::iterator it;
		if(mov_object){
			it = std::find(mov_list.begin(), mov_list.end(), mov_object);
			if(std::next(it,1) == mov_list.end()){
				mov_object = nullptr;
			}else{
				it = std::next(it,1);
				mov_object = *it;
			}
			
		}else{
			it = mov_list.begin();
			if(it != mov_list.end())
				mov_object = *it;
		}
	}
	
	if(key_front_down(KEY_Z))
		grid->set_visible(!grid->is_visible());
	
	if(key_front_down(KEY_C)){
		draw_collisions = !draw_collisions;
		set_draw_collision_models(draw_collisions);
	}
	
	if(key_front_down(KEY_ESC))
		platform_terminate();
	
	
	VECTOR_POLYLINE* 	polyline;
	RAYCAST_INFO_LIST	info_list;
	VECTOR2D			ray_origin;
	VECTOR2D			ray_direction;
	VECTOR2D			point;
	
	polyline 		= (VECTOR_POLYLINE*)ray_model->get_vector_object();
	ray_origin		= ray_caster->get_position();
	ray_direction	= ray_caster->get_normal();
	
	if(game_scene->bouncing_raycast(ray_origin, ray_direction, nullptr, &stop_filter, polyline->get_vertices_quantity()-2 ,info_list)){
		
		UINT i = 0;
		point = ray_origin;
		polyline->set_vertex(i, point);
		for(RAYCAST_INFO info: info_list){
			i++;
			point = ray_origin + ray_direction * info.distance;
			polyline->set_vertex(i, point);
			ray_origin 		= point;
			ray_direction 	= mirror_vector(ray_direction, info.normal); 
		}
		for(; i < polyline->get_vertices_quantity(); i++)
			polyline->set_vertex(i, point);
		
		ray->set_visible(true);
		
		colliding_point->set_position(point);
		colliding_point->set_visible(true);
		
	}else{
		colliding_point->set_visible(false);
		ray->set_visible(false);
	}
	
}

void ENGINE::pre_render(){
	
	
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
		case EV_MOUSE_WHEEL:
			if(mouse_record.delta > 0 && camera_scale < camera_scale_max){
				camera_scale+= 0.01;
				scene.camera.set_scale(camera_scale);
			}else if(mouse_record.delta < 0 && camera_scale > camera_scale_min){
				camera_scale-= 0.01;
				scene.camera.set_scale(camera_scale);
			}
		case EV_MOUSE_MOVE:
			mouse_coord = scale_vector(vector2d(mouse_record.x , mouse_record.y),camera_proportion);
			if(key_state(KEY_MOUSE_L)){
				VECTOR2D position;
				position = scene.camera.get_position();
				position -= scale_vector(mouse_coord - mouse_last,0.5/camera_scale);
				scene.camera.set_position(position);
				mouse_last = mouse_coord;
			}
		break;
		case EV_MOUSE_MKEY_DOWN:
			toggle_key_down(KEY_MOUSE_M);
		break;
		case EV_MOUSE_MKEY_UP:
			toggle_key_up(KEY_MOUSE_M);
		break;
		case EV_MOUSE_LKEY_DOWN:
			toggle_key_down(KEY_MOUSE_L);
			mouse_coord = scale_vector(vector2d(mouse_record.x , mouse_record.y),camera_proportion);
			mouse_last = mouse_coord;
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

void camera_movement(){
	
	if(key_state(KEY_SHIFT)){
		if(key_front_down(KEY_ARROW_LEFT))
			camera->set_normal(
				rotate_vector(camera->get_normal(), camera_rot_quant)
			);
		
		
		if(key_front_down(KEY_ARROW_RIGHT))
			camera->set_normal(
				rotate_vector(camera->get_normal(),-camera_rot_quant)
			);
		
		
	}else{
		if(key_front_down(KEY_ARROW_RIGHT))
			camera->set_position(
				camera->get_position() + vector2d( camera_mov_quant, 0.0)
			);
		
		
		if(key_front_down(KEY_ARROW_LEFT))
			camera->set_position(
				camera->get_position() + vector2d(-camera_mov_quant, 0.0)
			);
		
		
		if(key_front_down(KEY_ARROW_UP))
			camera->set_position(
				camera->get_position() + vector2d(0.0, camera_mov_quant)
			);
		
		
		if(key_front_down(KEY_ARROW_DOWN))
			camera->set_position(
				camera->get_position() + vector2d(0.0,-camera_mov_quant)
			);
		
	}
}

void object_movement(){
	
	if(!mov_object){
		cursor->set_visible(false);
		return;
	}
	
	cursor->set_visible(true);
		
	if(key_state(KEY_W))
		mov_object->set_position(
			mov_object->get_position() + vector2d( 0.0, 0.1)
		);
	
	if(key_state(KEY_S))
		mov_object->set_position(
			mov_object->get_position() + vector2d( 0.0,-0.1)
		);
	
	if(key_state(KEY_A))
		mov_object->set_position(
			mov_object->get_position() + vector2d(-0.1, 0.0)
		);
	
	if(key_state(KEY_D))
		mov_object->set_position(
			mov_object->get_position() + vector2d( 0.1, 0.0)
		);
	
	if(key_state(KEY_Q))
		mov_object->set_normal(
			rotate_vector(mov_object->get_normal(), M_PI/32)
		);
	
	
	if(key_state(KEY_E))
		mov_object->set_normal(
			rotate_vector(mov_object->get_normal(),-M_PI/32)
		);
		
	cursor->set_position(mov_object->get_position());
}
