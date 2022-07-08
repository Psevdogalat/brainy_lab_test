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

using namespace GAME_ENGINE;

 
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
		 vertex = rotate_vector(vertex * shape_scale	, shape_normal	) + shape_position;
		 vertex = rotate_vector(vertex 					, object_normal	) + object_position;
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
 
GAME_OBJECT* mov_object;
GAME_OBJECT* object1;
GAME_OBJECT* object2;
GAME_OBJECT* object3;
GAME_OBJECT* object4;
 
double camera_scale;
static const double camera_rot_quant = M_PI/32;
static const double camera_mov_quant = 0.2;

VECTOR2D camera_proportion;
VECTOR2D mouse_last	 = {0.0, 0.0};
VECTOR2D mouse_coord = {0.0, 0.0};

void ENGINE::init_scene(){
	
	init_key_map();
	set_draw_graphic_models		(true	);
	set_draw_collision_models	(false	);
	
	camera_scale = 0.1;
	scene.camera.set_area(VECTOR_RECTANGLE(vector2d(0,0), create_normal(), 1.5, 1.0));
	scene.camera.set_scale(camera_scale);
	camera_proportion = vector2d(1.5 , 1.0);
	
	spawn(new METRIC_GRID(1.5 * 10.0, 10.0), vector2d(0.0, 0.0), create_normal());
	
	GRAPHIC_MODEL_VECTOR* model;
	
	/*
	VECTOR_SHAPE shape1(vector2d(0.0, 0.0), create_normal(), 4);
	shape1.set_vertex(0, vector2d(-0.5, 0.5));
	shape1.set_vertex(1, vector2d( 0.5, 0.5));
	shape1.set_vertex(2, vector2d( 0.5,-0.5));
	shape1.set_vertex(3, vector2d(-0.5,-0.5));
	
	object1 = (GAME_OBJECT_WALL*) spawn(new GAME_OBJECT_WALL(), vector2d(0.0, 0.0), create_normal());
	model.set_vector_object(&shape1);
	model.clear_color_list();
	model.push_color(RGB_COLOR(1.0f, 1.0f, 0.0f));
	object1->set_graphic_model(&model);
	*/
	
	
	VECTOR_POINT point1;
	
	VECTOR_SHAPE shape1(vector2d(0.0, 0.0), create_normal(), 4);
	shape1.set_vertex(0, vector2d(-1.0, 1.0));
	shape1.set_vertex(1, vector2d( 1.0, 1.0));
	//shape1.set_vertex(2, vector2d( 0.0,-1.0));
	
	shape1.set_vertex(2, vector2d( 1.0,-1.0));
	shape1.set_vertex(3, vector2d(-1.0,-1.0));
	
	
	VECTOR_SHAPE shape2(vector2d(0.0, 0.0), create_normal(), 4);
	/*
	shape2.set_vertex(0, vector2d(-1.0,-1.0));
	shape2.set_vertex(1, vector2d( 1.0,-1.0));
	shape2.set_vertex(2, vector2d( 0.0, 1.0));
	*/
	shape2.set_vertex(0, vector2d(-1.0, 1.0));
	shape2.set_vertex(1, vector2d( 1.0, 1.0));
	shape2.set_vertex(2, vector2d( 1.0,-1.0));
	shape2.set_vertex(3, vector2d(-1.0,-1.0));
	
	
	object1 = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
		object1->set_name("Object1");
		
		model = new GRAPHIC_MODEL_VECTOR();
		model->set_vector_object(&shape1);
		model->push_color(RGB_COLOR(1.0f, 1.0f, 0.0f));
		
		object1->set_graphic_model(model);
		//object1->set_visible(false);
	
	object2 = spawn(new GAME_OBJECT(), vector2d(5.0, 0.0), create_normal());
		object2->set_name("Object2");

		model = new GRAPHIC_MODEL_VECTOR();
		model->set_vector_object(&shape2);
		model->push_color(RGB_COLOR(0.0f, 1.0f, 0.0f));
		
		object2->set_graphic_model(model);
		//object2->set_visible(false);
	
	object3 = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
		model = new GRAPHIC_MODEL_VECTOR();
		model->push_color(RGB_COLOR(0.0f, 0.0f, 1.0f));
		model->push_color(RGB_COLOR(0.0f, 0.0f, 0.5f));
		model->push_color(RGB_COLOR(0.0f, 0.5f, 0.5f));
		object3->set_graphic_model(model);
	
	object4 = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
		model = new GRAPHIC_MODEL_VECTOR();
		model->push_color(RGB_COLOR(1.0f, 0.0f, 0.0f));
		model->push_color(RGB_COLOR(0.0f, 1.0f, 0.5f));
		model->push_color(RGB_COLOR(1.0f, 0.5f, 0.5f));
		object4->set_graphic_model(model);
	
	mov_object = object1;
}

void ENGINE::free_scene(){
	
}

void ENGINE::compute(double Frame_time){
	VECTOR2D normal;
	VECTOR2D position;
	
	if(key_state(KEY_SHIFT)){
		if(key_front_down(KEY_ARROW_LEFT)){
			scene.camera.set_normal(
				rotate_vector(
					scene.camera.get_normal(),
					camera_rot_quant
				)
			);
		}
		
		if(key_front_down(KEY_ARROW_RIGHT)){
			scene.camera.set_normal(
				rotate_vector(
					scene.camera.get_normal(),
					-camera_rot_quant
				)
			);
		}
		
	}else{
		if(key_front_down(KEY_ARROW_RIGHT)){
			position = scene.camera.get_position();
			position.x += camera_mov_quant;
			scene.camera.set_position(position);
		}
		
		if(key_front_down(KEY_ARROW_LEFT)){
			position = scene.camera.get_position();
			position.x -= camera_mov_quant;
			scene.camera.set_position(position);
		}
		if(key_front_down(KEY_ARROW_UP)){
			position = scene.camera.get_position();
			position.y += camera_mov_quant;
			scene.camera.set_position(position);
		}
		
		if(key_front_down(KEY_ARROW_DOWN)){
			position = scene.camera.get_position();
			position.y -= camera_mov_quant;
			scene.camera.set_position(position);
		}
	}
	
	if(key_state(KEY_W)){
		position = mov_object->get_position();
		position.y += 0.1;
		mov_object->set_position(position);
	}
	
	if(key_state(KEY_S)){
		position = mov_object->get_position();
		position.y -= 0.1;
		mov_object->set_position(position);
	}
	
	if(key_state(KEY_A)){
		position = mov_object->get_position();
		position.x -= 0.1;
		mov_object->set_position(position);
	}
	
	if(key_state(KEY_D)){
		position = mov_object->get_position();
		position.x += 0.1;
		mov_object->set_position(position);
	}
	
	if(key_state(KEY_Q)){
		mov_object->set_normal(
			rotate_vector(
				mov_object->get_normal(),
				M_PI/32
			)
		);
	}
	
	if(key_state(KEY_E)){
		mov_object->set_normal(
			rotate_vector(
				mov_object->get_normal(),
				-M_PI/32
			)
		);
	}
	
	if(key_front_down(KEY_C)){
		printf("Break point!\n");
	}
	
	if(key_front_down(KEY_X)){
		if(mov_object == object1){
			mov_object = object2;
		}else{
			mov_object = object1;
		}
	}
	
	/*
	printf("Obj1 % f % f N % f % f\n",
		object1->get_position().x	, object1->get_position().y,
		object1->get_normal().x		, object1->get_normal().y);
	*/
	
	VECTOR_SHAPE 			shape1,shape2;
	VECTOR_COMPOSITE 		shape3,shape4;
	GRAPHIC_MODEL_VECTOR*	model;
	
	shape1 = extract_pure_shape(object1);
	shape2 = extract_pure_shape(object2);
	shape3 = operation_composite(shape2 , shape1, epa_intersection);
	shape4 = operation_composite(shape2 , shape1, minkovskiy_diff);
	
	VECTOR2D* simplex;
	UINT	  simplex_size;
	UINT	  vertices1_quantity;
	UINT	  vertices2_quantity;
	VECTOR2D* vertices1;
	VECTOR2D* vertices2;
	EPA_INFO  epa_info;
	
	vertices1_quantity = shape1.get_vertices_quantity();
	vertices2_quantity = shape2.get_vertices_quantity();
	
	vertices1 = new VECTOR2D[vertices1_quantity];
	for(UINT i = 0; i < vertices1_quantity; i++)
		shape1.get_vertex(i,vertices1[i]);
	
	vertices2 = new VECTOR2D[vertices2_quantity];
	for(UINT i = 0; i < vertices2_quantity; i++)
		shape2.get_vertex(i,vertices2[i]);
	
	
	if(gjk_collision(
		vertices2,vertices2_quantity,
		vertices1,vertices1_quantity,
		simplex  , simplex_size)
	){
		epa_info = epa_collision_info(
			vertices2, vertices2_quantity,
			vertices1, vertices1_quantity,
			(const VECTOR2D*)simplex
		);
		
		delete [] simplex;
		printf("{%f %f} %f\n",epa_info.normal.x, epa_info.normal.y, epa_info.distance);
	}
		
	
	delete [] vertices1;
	delete [] vertices2;
	

	model = (GRAPHIC_MODEL_VECTOR*)object3->get_graphic_model();
	model->set_vector_object(&shape4);
	
	model = (GRAPHIC_MODEL_VECTOR*)object4->get_graphic_model();
	model->set_vector_object(&shape3);
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
			if(mouse_record.delta > 0 && camera_scale < 1.0){
				camera_scale+= 0.01;
				scene.camera.set_scale(camera_scale);
			}else if(mouse_record.delta < 0 && camera_scale > 0.1){
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


