#include <objects.h>
#include <utilits.h>

#include <cstdio>
#include <cmath>

using namespace GAME_ENGINE;

/* class GRID object */
//=================================================================================================

METRIC_GRID::METRIC_GRID(double Width, double Height){
	if(Width < 1.0){
		width = 1.0;
	}else
		width = Width;
	
	if(Height < 1.0){
		height = 1.0;
	}else
		height = Height;
	
	set_graphic_model(create_grid_model(width, height));
}

METRIC_GRID::~METRIC_GRID(){
	delete get_graphic_model();
}


/* class GAME_OBJECT_POINT */
//=================================================================================================
GAME_OBJECT_POINT::GAME_OBJECT_POINT(){
	type = GOT_POINT;
	set_graphic_model(create_point_model());
}

GAME_OBJECT_POINT::~GAME_OBJECT_POINT(){
	delete get_graphic_model();
}

/* class GAME_OBJECT_WALL */
//=================================================================================================
GAME_OBJECT_WALL::GAME_OBJECT_WALL(){
	type 	= GOT_WALL;
	width 	= 1.0;
	height	= 1.0;
	color	= RGB_COLOR(0.5f, 0.5f, 0.5f);
}

GAME_OBJECT_WALL::GAME_OBJECT_WALL(double Width, double Height, RGB_COLOR Color):GAME_OBJECT_WALL(){
	width 	= Width;
	height	= Height;
	color	= Color;
}

void GAME_OBJECT_WALL::spawn(){
	VECTOR_RECTANGLE rect(vector2d(0.0, 0.0), create_normal(), width, height);
	
	GRAPHIC_MODEL_VECTOR* graphic_model = new GRAPHIC_MODEL_VECTOR();
	graphic_model->set_vector_object(&rect);
	graphic_model->push_color(color);
	set_graphic_model(graphic_model);
	
	set_collision_node	(
		allocate_collision_node(
			this, false, true, true, new COLLISION_MODEL(rect)
		)
	);
}

void GAME_OBJECT_WALL::despawn(){
	delete get_graphic_model();
	free_collision_node(get_collision_node());
}

/* class GAME_OBJECT_BULLET */
//=================================================================================================
GAME_OBJECT_BULLET::GAME_OBJECT_BULLET(){
	type 	= GOT_BULLET;
	owner	= nullptr;
	life_timer = nullptr;
}

void GAME_OBJECT_BULLET::spawn(){
	VECTOR_RECTANGLE		collision_shape(vector2d(0.0, 0.0), create_normal(), 0.2, 0.2);
	
	set_graphic_model	(create_bullet_model());
	set_physical_model	(allocate_physical_model());
	set_collision_node	(
		allocate_collision_node(
			this, true, true, true, new COLLISION_MODEL(collision_shape)
		)
	);
	
	
	life_timer = allocate_timer();
	
	life_time = 20;
	life_timer->set(life_time);
	
	move_speed = 10;
	physical_model->velocity = normal * move_speed;
	
	bounce_resource = 10;
}

void GAME_OBJECT_BULLET::compute(){
	if(life_timer->condition()){
		GAME_ENGINE::despawn(this);
		return;
	}
}

void GAME_OBJECT_BULLET::despawn(){
	free_timer(life_timer);
	delete get_graphic_model();
	free_physical_model(get_physical_model());
	free_collision_node(get_collision_node());
}

void GAME_OBJECT_BULLET::collision(GAME_OBJECT* Object, const COLLISION_INFO* Info){
	PHYSICAL_MODEL* physical_model;
	
	switch(Object->get_type()){
		case GOT_WALL:
			
			physical_model	= get_physical_model();
			
			position += (Info->epa_info.normal * Info->epa_info.distance);
			physical_model->velocity = mirror_vector(physical_model->velocity, Info->epa_info.normal);
			
			/*
			if(bounce_resource){
				bounce_resource--;
			}else
				GAME_ENGINE::despawn(this);
			*/
			
		break;
		case GOT_BULLET:
		break;
		default:	
		GAME_ENGINE::despawn(this);
	}
}

void GAME_OBJECT_BULLET::set_owner(GAME_OBJECT* Owner){
	owner = Owner;
}

GAME_OBJECT* GAME_OBJECT_BULLET::get_owner(){
	return (GAME_OBJECT*) owner;
}

/* PLAYER game object*/
//=================================================================================================
GAME_OBJECT_PLAYER::GAME_OBJECT_PLAYER(){
	type = GOT_PLAYER;
	on_dead = nullptr;
	color = RGB_COLOR(0.5f, 0.5f, 0.5f);
	
	invul				= false;
}

void GAME_OBJECT_PLAYER::spawn(){
	VECTOR_RECTANGLE	collision_shape(vector2d(0.0, 0.0), create_normal(), 1.0, 1.0);
	
	set_graphic_model (create_base_unit_model());
	((GRAPHIC_MODEL_VECTOR*)get_graphic_model())->push_color(color);
	
	
	set_physical_model	(allocate_physical_model());
	set_collision_node	(
		allocate_collision_node(
			this, true, true, true, new COLLISION_MODEL(collision_shape)
		)
	);
	
	fire_timer		= allocate_timer();
	
	move_speed 		= 4;
	rotate_speed 	= M_PI/2;
	fire_speed		= 0.1;
	
	
	
	do_forward_move		= false;
	do_backward_move	= false;
	do_left_rotate		= false;
	do_right_rotate		= false;
	do_fire				= false;
	
	state_reload		= false;
	
};
	
void GAME_OBJECT_PLAYER::despawn(){
	free_timer(fire_timer);
	delete get_graphic_model();
	free_physical_model(get_physical_model());
	free_collision_node(get_collision_node());
	
};

void GAME_OBJECT_PLAYER::compute(){
	GAME_OBJECT_BULLET* bullet;
	
	double cur_move_speed 	= 0;
	double cur_rotate_speed = 0;
	
	if(do_forward_move)
		cur_move_speed += move_speed;
	
	if(do_backward_move)
		cur_move_speed -= move_speed;
	
	if(do_left_rotate)
		cur_rotate_speed += rotate_speed;
	
	if(do_right_rotate)
		cur_rotate_speed -= rotate_speed;
	
	if(do_fire){
		if(fire_timer->condition()){
			bullet = (GAME_OBJECT_BULLET*)GAME_ENGINE::spawn(new GAME_OBJECT_BULLET(),position + normal * 1, normal);
			bullet->set_owner(this);
			fire_timer->set(fire_speed);
		}
	}
	
	physical_model->angular_velocity 	= cur_rotate_speed;
	physical_model->velocity 			= normal * cur_move_speed;
	
	do_forward_move		= false;
	do_backward_move	= false;
	do_left_rotate		= false;
	do_right_rotate		= false;
	do_fire				= false;
}

void GAME_OBJECT_PLAYER::move_forward(){
	do_forward_move = true;
}

void GAME_OBJECT_PLAYER::move_backward(){
	do_backward_move = true;
}

void GAME_OBJECT_PLAYER::rotate_left(){
	do_left_rotate = true;
}

void GAME_OBJECT_PLAYER::rotate_right(){
	do_right_rotate = true;
}

void GAME_OBJECT_PLAYER::fire(){
	do_fire = true;
}

void GAME_OBJECT_PLAYER::set_color	(RGB_COLOR Color){
	color = Color;
	
}

void GAME_OBJECT_PLAYER::set_invul(bool Invul){
	invul = Invul;
}

bool GAME_OBJECT_PLAYER::is_invul(){
	return invul;
}

void GAME_OBJECT_PLAYER::set_on_dead(void (*On_dead) (GAME_OBJECT* )){
	on_dead = On_dead;
}

void GAME_OBJECT_PLAYER::collision(GAME_OBJECT* Object, const COLLISION_INFO* Info){
	switch(Object->get_type()){
	case GOT_BULLET:
		if(invul != true){
			if(on_dead != nullptr)
				on_dead(Object);
			
			GAME_ENGINE::despawn(this);
		}
	break;
	case GOT_WALL:
		position += 1*(Info->epa_info.normal * Info->epa_info.distance);
		//printf("{%f %f} %f\n",Info->epa_info.normal.x,Info->epa_info.normal.y, Info->epa_info.distance);
	break;
	default:
		;
	}
}


