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
void ray_cast(const VECTOR2D&, const VECTOR2D&, const VECTOR2D*, const UINT, VECTOR2D*&, UINT&);
VECTOR_SHAPE extract_pure_shape(GAME_OBJECT* );
VECTOR_COMPOSITE operation_composite( VECTOR_SHAPE& , VECTOR_SHAPE&, void(*)(VECTOR2D*, UINT, VECTOR2D*, UINT, VECTOR2D*&, UINT&));

typedef struct{
	const GAME_OBJECT* 	object;
	VECTOR2D			normal;
	double 				distance;		
}RAYCAST_INFO;

typedef std::list<RAYCAST_INFO> RAYCAST_INFO_LIST;

class GAME_OBJECT_FILTER{
	protected:
		static bool default_filt(const std::list<GAME_OBJECT*>& , const GAME_OBJECT* );
	public:
		std::list<GAME_OBJECT*> pointers;
		bool(* filter_func)(const std::list<GAME_OBJECT*>& , const GAME_OBJECT* );
		bool inverse;
		bool filt(const GAME_OBJECT* ) const;
		GAME_OBJECT_FILTER();
		~GAME_OBJECT_FILTER();
};

GAME_OBJECT_FILTER::GAME_OBJECT_FILTER(){
		filter_func = default_filt;
		inverse 	= false;
}

GAME_OBJECT_FILTER::~GAME_OBJECT_FILTER(){
	
}

bool GAME_OBJECT_FILTER::filt(const GAME_OBJECT* Object) const{
	if(!filter_func)
		return true;
	
	return filter_func(pointers, Object) != inverse;
}

bool GAME_OBJECT_FILTER::default_filt(const std::list<GAME_OBJECT*>& Pointers, const GAME_OBJECT* Object){
	
	for(GAME_OBJECT* target: Pointers){
		if(Object == target)
			return true;
	}
		
	return false;
}



bool raycast(const VECTOR2D&, const VECTOR2D&, const GAME_OBJECT_FILTER*, RAYCAST_INFO& );
bool raycast(const VECTOR2D&, const VECTOR2D*, const UINT, double&, VECTOR2D&);
bool bouncing_raycast(
	const VECTOR2D&, const VECTOR2D&, 
	const GAME_OBJECT_FILTER*, const GAME_OBJECT_FILTER*, 
	UINT, RAYCAST_INFO_LIST& 
);

static bool draw_collisions = false;

static CAMERA* 	 				camera;
static GAME_OBJECT* 			grid;
static GAME_OBJECT* 			mov_object;
static std::list<GAME_OBJECT*> 	mov_list;

static GAME_OBJECT*	unit;
static GAME_OBJECT* ray_caster;
static GAME_OBJECT* ray;
static GAME_OBJECT* cursor;
static GAME_OBJECT* colliding_point;

static GAME_OBJECT_FILTER filter;
static GAME_OBJECT_FILTER stop_filter;

static GRAPHIC_MODEL_VECTOR*	ray_caster_model;
static GRAPHIC_MODEL_VECTOR*	ray_model;
static GRAPHIC_MODEL_VECTOR*	cursor_model;
static GRAPHIC_MODEL_VECTOR*	colliding_point_model;

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
	camera = &scene.camera;
	
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
	
	if(bouncing_raycast(ray_origin, ray_direction, nullptr, &stop_filter, polyline->get_vertices_quantity()-2 ,info_list)){
		
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


 
VECTOR_SHAPE extract_pure_shape(GAME_OBJECT* Object){
	GRAPHIC_MODEL_VECTOR* 	model;
	VECTOR_SHAPE 			shape;
	VECTOR2D 	shape_position;
	VECTOR2D	shape_normal;
	VECTOR2D	object_position;
	VECTOR2D	object_normal;
	double 	shape_scale;
	double 	object_scale;
	VECTOR2D vertex;

	model = (GRAPHIC_MODEL_VECTOR*) Object->get_graphic_model();
	shape = *(VECTOR_SHAPE*) model->get_vector_object();

	object_normal		= Object->get_normal();
	object_position		= Object->get_position();
	shape_scale			= shape.get_scale();
	shape_position 		= shape.get_position();
	shape_normal		= shape.get_normal();

	for(UINT ivertex = 0; ivertex < shape.get_vertices_quantity(); ivertex++){
		 shape.get_vertex(ivertex, vertex);
		 vertex = transform_vertex(vertex, shape_position,  shape_normal,  shape_scale);
		 vertex = transform_vertex(vertex, object_position, object_normal, 1.0);
		 shape.set_vertex(ivertex, vertex);
	}
	shape.set_position(vector2d(0.0, 0.0));
	shape.set_normal(create_normal());
	shape.set_scale(1.0);

	 return shape;
}
 

 
VECTOR_COMPOSITE operation_composite(
	VECTOR_SHAPE& Shape1, VECTOR_SHAPE& Shape2,
	void(* Operation)(VECTOR2D*, UINT, VECTOR2D*, UINT, VECTOR2D*&, UINT&)
){
	VECTOR_COMPOSITE composite;
	VECTOR_OBJECT*	 vobject	= nullptr;
	VECTOR_POINT	 point;
	
	UINT 		vertices1_quantity;
	VECTOR2D*	vertices1;
	
	UINT 		vertices2_quantity;
	VECTOR2D*	vertices2;
	
	UINT 		result_vertices_quantity;
	VECTOR2D*	result_vertices;
	
	UINT 		hull_vertices_quantity;
	VECTOR2D*	hull_vertices;
	
	VECTOR2D 	average_vertex;
	
	if(Operation == nullptr)
		return composite;
	
	vertices1_quantity = Shape1.get_vertices_quantity();
	vertices2_quantity = Shape2.get_vertices_quantity();
	
	vertices1 = new VECTOR2D[vertices1_quantity];
	for(UINT i = 0; i < vertices1_quantity; i++)
		Shape1.get_vertex(i,vertices1[i]);
	
	vertices2 = new VECTOR2D[vertices2_quantity];
	for(UINT i = 0; i < vertices2_quantity; i++)
		Shape2.get_vertex(i,vertices2[i]);
	
	Operation(
		vertices1		, vertices1_quantity, 
		vertices2		, vertices2_quantity, 
		result_vertices	, result_vertices_quantity);
	
	delete [] vertices1;
	delete [] vertices2;
	
	
	if(result_vertices_quantity > 0 ){
		
		average_vertex 	= vector2d(0.0, 0.0);
		for(UINT i = 0; i < result_vertices_quantity; i++)
			average_vertex += result_vertices[i];
		
		average_vertex /= result_vertices_quantity;
		point.set_position(average_vertex);
		composite.push_component(&point);
	
		get_mch_jarvis(result_vertices, result_vertices_quantity, hull_vertices, hull_vertices_quantity); 
		if(hull_vertices_quantity > 0){
			
		
			if(hull_vertices_quantity == 2){
				vobject = new VECTOR_POLYLINE();
				for(UINT i = 0; i < hull_vertices_quantity; i++)
					((VECTOR_POLYLINE*)vobject)->set_vertex(i, hull_vertices[i]);
		
			}else if(hull_vertices_quantity > 2){
				vobject = new VECTOR_SHAPE(vector2d(0.0, 0.0), create_normal(), hull_vertices_quantity);
				for(UINT i = 0; i < hull_vertices_quantity; i++)
					((VECTOR_POLYLINE*)vobject)->set_vertex(i, hull_vertices[i]);
		
			}
			
			composite.push_component(vobject);			
			
			delete vobject;
			delete [] hull_vertices;
		}
		
		for(UINT i = 0; i < result_vertices_quantity; i++){
			point.set_position(result_vertices[i]);
			composite.push_component(&point);
		}
		
		delete [] result_vertices;
	}
	
	return composite;
} 

bool bouncing_raycast(
	const VECTOR2D& Origin, const VECTOR2D& Direction, 
	const GAME_OBJECT_FILTER* Filter, const GAME_OBJECT_FILTER* Stop_filter, 
	UINT Bounce_limit, RAYCAST_INFO_LIST& Info_list 
){
	UINT		 intersections;
	RAYCAST_INFO info;
	VECTOR2D	 origin;
	VECTOR2D	 direction;
	
	intersections	= 0;	
	origin 			= Origin;
	direction		= Direction;
	
	while(raycast(origin, direction, Filter, info)){
		intersections++;
		Info_list.push_back(info);
		
		if(Stop_filter && Stop_filter->filt(info.object))
			break;
		
		if(Bounce_limit == intersections - 1)
			break;
		
		origin 		+= direction * info.distance;
		direction	= mirror_vector(direction, info.normal);
	}
	
	if(intersections)
		return true;
	
	return false;
}

bool raycast(const VECTOR2D& Origin, const VECTOR2D& Direction,const GAME_OBJECT_FILTER* Filter, RAYCAST_INFO& Info){
	
	const GAME_OBJECT*			object;
	const COLLISION_NODE_LIST*	collisions_list;
	const COLLISION_MODEL*		collision_model;
	const VECTOR2D*				model_vertices;
	VECTOR2D*					vertices;
	UINT						vertices_quantity;
	double 						distance;
	VECTOR2D					normal;
	RAYCAST_INFO				info;
	
	info.object 	= nullptr;
	collisions_list	= GAME_ENGINE::get_passive_collision_list();
	
	for(COLLISION_NODE* node: *collisions_list){
		object = node->get_game_object();
		
		if(Filter && !Filter->filt(object))
			continue;
		
		collision_model 	= node->get_collision_model();
		vertices_quantity 	= collision_model->get_vertices_quantity();
		model_vertices		= collision_model->get_vertices();
		
		vertices = new VECTOR2D[vertices_quantity];
		for(UINT i = 0; i < vertices_quantity; i++)
			vertices[i] = transform_vertex(
				model_vertices[i], 
				object->get_position() - Origin, 
				object->get_normal(), 1.0
			);
		
		if(raycast(Direction, vertices, vertices_quantity, distance, normal) && distance >= 0.0)
			if(info.object == nullptr || info.distance > distance){
				info.object 	= object;
				info.distance	= distance;
				info.normal		= normal;
			}
			
		delete [] vertices;
			
	}

	if(info.object){
		Info = info;
		return true;
	}
	
	return false;
}	

bool raycast(
	const VECTOR2D& Direction, 
	const VECTOR2D* Vertices, const UINT Vertices_quantity, 
	double& Distance, VECTOR2D& Normal
){
	
	VECTOR2D 	vertex_a;
	VECTOR2D 	vertex_b;
	double 		vec_a;
	double 		vec_b;
	double 		distance;
	UINT		intersections;
	
	intersections 	= 0;
	vertex_a 		= Vertices[Vertices_quantity-1];
	
	for(UINT i = 0; i < Vertices_quantity; i++){
		vertex_b = Vertices[i];
		
		vec_a = vector_product2d(Direction, vertex_a);
		vec_b = vector_product2d(Direction, vertex_b);
		
		if(vec_a * vec_b < 0){
			distance 		= vector_product2d(vertex_a, vertex_b)/ (vec_b - vec_a);
			
			if(!intersections || Distance > distance){
				Distance 	= distance;
				Normal 		= left_orto_normal(vertex_b - vertex_a);
				intersections++;
			}
		}
		
		vertex_a = vertex_b;
	}
	
	if(intersections)
		return true;
	
	return false;
}

void ray_cast(
	const VECTOR2D& Origin,				const VECTOR2D& Direction , 
	const VECTOR2D* Shape_vertices, 	const UINT Shape_vertices_quantity, 
	VECTOR2D*&		Collision_points, 	UINT& Collision_points_quantity
){
	VECTOR2D* Vertices = new VECTOR2D[Shape_vertices_quantity];
	for(UINT i=0; i< Shape_vertices_quantity; i++)
		Vertices[i] = Shape_vertices[i] - Origin;
	
	Collision_points 			= nullptr;
	Collision_points_quantity 	= 0;
	
	VECTOR2D vertex_a;
	VECTOR2D vertex_b;
	VECTOR2D collision_point;
	double vec_a;
	double vec_b;
	double distance;
	
	vertex_a = Vertices[Shape_vertices_quantity-1];
	
	for(UINT i = 0; i < Shape_vertices_quantity; i++){
		vertex_b = Vertices[i];
		
		vec_a = vector_product2d(Direction, vertex_a);
		vec_b = vector_product2d(Direction, vertex_b);
		
		if(vec_a * vec_b < 0){
			distance 		= vector_product2d(vertex_a, vertex_b)/ (vec_b - vec_a);
			
			if(distance >= 0.0){			
				collision_point = Direction * distance;
			
				if(!Collision_points_quantity){
					Collision_points_quantity = 2;
					Collision_points = new VECTOR2D[Collision_points_quantity];
					Collision_points[0] = collision_point;
					Collision_points[1] = collision_point;
					
				}else{
					if(distance < scalar_product2d(Direction, Collision_points[0]))
						Collision_points[0] = collision_point;
					
					if(distance > scalar_product2d(Direction, Collision_points[1]))
						Collision_points[1] = collision_point;
				}
			}
		}
		
		vertex_a = vertex_b;
	}
	
	if(Collision_points_quantity)
		for(UINT i = 0; i < Collision_points_quantity; i++)
			Collision_points[i] += Origin;
	
	delete [] Vertices;
}
