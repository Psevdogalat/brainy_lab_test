#include <engine.h>
#include <gl/gl.h>
#include <list>
#include <cstdio>

using namespace GAME_ENGINE;

RGB_COLOR::RGB_COLOR(){
	
}

RGB_COLOR::RGB_COLOR(float R, float G, float B){
	r = R;
	g = G;
	b = B;
}

RGB_COLOR::RGB_COLOR(const RGB_COLOR& Color){
	r = Color.r;
	g = Color.g;
	b = Color.b;
}

RGB_COLOR& RGB_COLOR::operator= (const RGB_COLOR& Color){
	r = Color.r;
	g = Color.g;
	b = Color.b;
	return *this;
}

/* GRAPHIC_VECTOR_PRIMITIVE  */

GRAPHIC_VECTOR_PRIMITIVE::GRAPHIC_VECTOR_PRIMITIVE(){
	
}

GRAPHIC_VECTOR_PRIMITIVE::GRAPHIC_VECTOR_PRIMITIVE(UINT Type,const RGB_COLOR& Color){
	type 	= Type;
	color 	= Color;
}

GRAPHIC_VECTOR_PRIMITIVE::GRAPHIC_VECTOR_PRIMITIVE(const GRAPHIC_VECTOR_PRIMITIVE& Primitive){
	type	= Primitive.type;
	color 	= Primitive.color;
	for(VECTOR2D vertex: Primitive.verteces)
		verteces.push_back(vertex);
	
}

GRAPHIC_VECTOR_PRIMITIVE& GRAPHIC_VECTOR_PRIMITIVE::operator= (const GRAPHIC_VECTOR_PRIMITIVE& Primitive){
	type 	= Primitive.type;
	color	= Primitive.color;
	verteces.clear();
	for(VECTOR2D vertex: Primitive.verteces)
		verteces.push_back(vertex);
	
	return *this;
}

/* base class GRAPHIC_MODEL  */
GRAPHIC_MODEL::GRAPHIC_MODEL(){
	
}

GRAPHIC_MODEL::GRAPHIC_MODEL(const GRAPHIC_MODEL& Model){
	type = Model.type;
}

GRAPHIC_MODEL::~GRAPHIC_MODEL(){
	
}

UINT GRAPHIC_MODEL::get_type(){
	return type;
}


/* class GRAPHIC_MODEL_VECTOR */
GRAPHIC_MODEL_VECTOR::GRAPHIC_MODEL_VECTOR(){
	type = GRAPHIC_MODEL::T_VECTOR;
	vector_object = nullptr;
}

GRAPHIC_MODEL_VECTOR::GRAPHIC_MODEL_VECTOR(const GRAPHIC_MODEL_VECTOR& Vector_model ):
GRAPHIC_MODEL((const GRAPHIC_MODEL&)Vector_model){
	if(Vector_model.vector_object != nullptr){
		vector_object = Vector_model.vector_object->new_copy();
	}else
		vector_object = nullptr;
	
	for(RGB_COLOR color: Vector_model.color_list)
		color_list.push_back(color);
}

GRAPHIC_MODEL_VECTOR::GRAPHIC_MODEL_VECTOR(VECTOR_OBJECT* Vector_object ){
	type = GRAPHIC_MODEL::T_VECTOR;
	if(Vector_object != nullptr){
		vector_object = Vector_object->new_copy();
	}else
		vector_object = nullptr;
}

GRAPHIC_MODEL_VECTOR::~GRAPHIC_MODEL_VECTOR(){
	if(vector_object != nullptr)
		delete vector_object;
}

GRAPHIC_MODEL* GRAPHIC_MODEL_VECTOR::new_copy(){
	return new GRAPHIC_MODEL_VECTOR(*this); 
}

void GRAPHIC_MODEL_VECTOR::set_vector_object(VECTOR_OBJECT* Object){
	if(vector_object != nullptr)
			delete vector_object;
		
	if(Object != nullptr){
		vector_object =  Object->new_copy();
	}else
		vector_object = nullptr;
	
}

VECTOR_OBJECT* GRAPHIC_MODEL_VECTOR::get_vector_object(){
	return vector_object;
}

COLOR_LIST& GRAPHIC_MODEL_VECTOR::get_color_list(){
	return color_list;
}

void GRAPHIC_MODEL_VECTOR::push_color (RGB_COLOR Color ){
	color_list.push_back(Color);
}

void GRAPHIC_MODEL_VECTOR::clear_color_list(){
	color_list.clear();
}

GRAPHIC_MODEL_VECTOR& GRAPHIC_MODEL_VECTOR::operator= (const GRAPHIC_MODEL_VECTOR& Model){
	set_vector_object(Model.vector_object);
	color_list.clear();
	for(RGB_COLOR color: Model.color_list)
		color_list.push_back(color);
	
	return *this;
}

/* class ENGINE */
static bool			draw_graphic_models;

void set_draw_graphic_models(bool Flag){
	draw_graphic_models = Flag;
}

static bool 		draw_collision_models;
static RGB_COLOR	collision_model_color;

void set_draw_collision_models(bool Flag){
	draw_collision_models = Flag;
}

static bool 		draw_physical_models;
static RGB_COLOR	physical_model_color;

void set_draw_physical_models(bool Flag){
	draw_physical_models = Flag;
}

static std::list<GRAPHIC_VECTOR_PRIMITIVE*> primitive_list;
static RGB_COLOR default_color;



void ENGINE::init_graphic(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	default_color 			= RGB_COLOR(0.5f, 0.5f, 0.5f);
	collision_model_color	= RGB_COLOR(1.0f, 1.0f, 0.0f);
	physical_model_color	= RGB_COLOR(0.0f, 1.0f, 0.0f);
	
	draw_graphic_models 	= true;
	draw_collision_models	= false;
	draw_physical_models	= false;
}

static void build_primitives_POINT(VECTOR_POINT* Object, RGB_COLOR Color, const VECTOR2D& Position, const VECTOR2D& Normal, double Scale){
	GRAPHIC_VECTOR_PRIMITIVE* primitive;
	VECTOR2D position;
	VECTOR2D vertex;
	
	static const double coord = 0.01;
	
	position	= transform_vertex(Object->get_position(), Position, Normal, Scale);
	
	primitive = new GRAPHIC_VECTOR_PRIMITIVE(GRAPHIC_VECTOR_PRIMITIVE::T_POLYLINE, Color);
	primitive->verteces.push_back(vector2d(-coord, coord) +  position);
	primitive->verteces.push_back(vector2d( coord,-coord) +  position);
	primitive_list.push_back(primitive);
	
	primitive = new GRAPHIC_VECTOR_PRIMITIVE(GRAPHIC_VECTOR_PRIMITIVE::T_POLYLINE, Color);
	primitive->verteces.push_back(vector2d( coord, coord) + position);
	primitive->verteces.push_back(vector2d(-coord,-coord) + position);
	primitive_list.push_back(primitive);
}

static void build_primitives_POLYLINE(VECTOR_POLYLINE* Object, RGB_COLOR Color, const VECTOR2D& Position, const VECTOR2D& Normal, double Scale){
	GRAPHIC_VECTOR_PRIMITIVE* primitive;
	VECTOR2D vertex;
	VECTOR2D normal;
	VECTOR2D position;
	double   scale;
	
	scale 		= Object->get_scale();
	position	= Object->get_position();
	normal 		= Object->get_normal();
	
	primitive = new GRAPHIC_VECTOR_PRIMITIVE(GRAPHIC_VECTOR_PRIMITIVE::T_POLYLINE, Color);
	for(UINT ivertex = 0; ivertex < Object->get_vertices_quantity(); ivertex++){
		Object->get_vertex(ivertex,vertex);
		vertex = transform_vertex(vertex, position, normal, scale);
		vertex = transform_vertex(vertex, Position, Normal, Scale);
		primitive->verteces.push_back(vertex);
	}
	primitive_list.push_back(primitive);
}

static void build_primitives_SHAPE(VECTOR_SHAPE* Object, RGB_COLOR Color, const VECTOR2D& Position, const VECTOR2D& Normal, double Scale){
	GRAPHIC_VECTOR_PRIMITIVE* primitive;
	VECTOR2D vertex;
	VECTOR2D normal;
	VECTOR2D position;
	double   scale;		
	
	scale 		= Object->get_scale();
	position	= Object->get_position();
	normal 		= Object->get_normal();
	
	primitive = new GRAPHIC_VECTOR_PRIMITIVE(GRAPHIC_VECTOR_PRIMITIVE::T_POLYGON, Color);
	for(UINT ivertex = 0; ivertex < Object->get_vertices_quantity(); ivertex++){
		Object->get_vertex(ivertex,vertex);
		vertex = transform_vertex(vertex, position, normal, scale);
		vertex = transform_vertex(vertex, Position, Normal, Scale);
		primitive->verteces.push_back(vertex);
	}
	primitive_list.push_back(primitive);
}

static void build_primitives_CIRCLE(VECTOR_CIRCLE* Object, RGB_COLOR Color, const VECTOR2D& Position, const VECTOR2D& Normal, double Scale){
	GRAPHIC_VECTOR_PRIMITIVE* primitive;
	UINT 	 verteces_quantity = 32;
	double 	 angle 	= 2*M_PI/verteces_quantity;
	VECTOR2D vertex;
	VECTOR2D normal;
	VECTOR2D position;
	double   scale;	
	double 	 radius;
	
	radius		= Object->get_radius();
	scale 		= Object->get_scale();
	position	= Object->get_position();
	normal 		= Object->get_normal();
	
	primitive = new GRAPHIC_VECTOR_PRIMITIVE(GRAPHIC_VECTOR_PRIMITIVE::T_POLYGON, Color);
	for(UINT ivertex = 0; ivertex < verteces_quantity; ivertex++){
		vertex = rotate_vector(create_normal(),angle * ivertex) * radius; 
		vertex = transform_vertex(vertex, position, normal, scale);
		vertex = transform_vertex(vertex, Position, Normal, Scale);
		primitive->verteces.push_back(vertex);
	}
	primitive_list.push_back(primitive);
}


static void build_primitives(VECTOR_OBJECT* Object,COLOR_LIST& Color_list, COLOR_LIST::iterator& Color,
	const VECTOR2D& Position, const VECTOR2D& Normal, double Scale){
	
	VECTOR_COMPOSITE* composite;
	RGB_COLOR	color;
	VECTOR2D 	normal;
	VECTOR2D 	position;
	double 		scale;
	
	COLOR_LIST::iterator next_color;
	
	if(Color != Color_list.end()){
		color = *Color;
	}else{
		color = default_color;
	}
	
	switch(Object->get_type()){
		case VECTOR_OBJECT::T_COMPOSITE:
			composite 	= (VECTOR_COMPOSITE*) Object;
			position	= transform_vertex(composite->get_position(), Position, Normal, Scale);
			normal 		= rotate_vector(composite->get_normal(), Normal);
			scale 	  	= composite->get_scale() * Scale;
			
			for(VECTOR_OBJECT* Object: composite->get_components()){
				build_primitives(Object, Color_list, Color, position, normal, scale);
			}
			return;
		break;
		case VECTOR_OBJECT::T_POINT:
			build_primitives_POINT((VECTOR_POINT*)Object, color, Position, Normal, Scale);
		break;
		case VECTOR_OBJECT::T_LINE:
		case VECTOR_OBJECT::T_POLYLINE:
			build_primitives_POLYLINE((VECTOR_POLYLINE*)Object, color, Position, Normal, Scale);
		break;
		case VECTOR_OBJECT::T_SHAPE:
		case VECTOR_OBJECT::T_RECTANGLE:
			build_primitives_SHAPE((VECTOR_SHAPE*)Object, color, Position, Normal, Scale);
		break;
		case VECTOR_OBJECT::T_CIRCLE:
			build_primitives_CIRCLE((VECTOR_CIRCLE*)Object, color, Position, Normal, Scale);
		break;
	}
	
	next_color = Color;
	if(++next_color != Color_list.end())
		Color++;
}

static void build_primitives_collision_model(COLLISION_MODEL* Model,const VECTOR2D& Position, const VECTOR2D& Normal, double Scale){
	GRAPHIC_VECTOR_PRIMITIVE* 	primitive;
	const VECTOR2D* 			vertices;
	UINT						vertices_quantity;
	
	vertices_quantity	= Model->get_vertices_quantity();
	vertices 			= Model->get_vertices();
	
	primitive = new GRAPHIC_VECTOR_PRIMITIVE(GRAPHIC_VECTOR_PRIMITIVE::T_POLYGON, collision_model_color);
	for(UINT i = 0; i < vertices_quantity; i++){
		primitive->verteces.push_back(
			transform_vertex(vertices[i], Position, Normal, Scale)
		);
	}
	primitive_list.push_back(primitive);
}


void ENGINE::render(){
	
	CAMERA& camera = scene.camera;
	
	GRAPHIC_MODEL* 			graphic_model;
	GRAPHIC_MODEL_VECTOR*	graphic_model_vector;
	
	VECTOR2D				camera_position;
	VECTOR_RECTANGLE		camera_rectangle;
	VECTOR2D				camera_reverse_normal;
	double 					camera_scale;
	VECTOR2D				camera_proportion;
	
	camera_rectangle = (VECTOR_RECTANGLE)camera;
	camera_rectangle.set_scale(0.999);
	camera_rectangle.set_normal(create_normal());
	camera_rectangle.set_position(vector2d(0,0));
	
	camera_position			= camera.get_position();
	camera_scale 			= camera.get_scale();
	
	camera_reverse_normal	= camera.get_normal();
	camera_reverse_normal.y *= -1.0;
	
	camera_proportion.x =  camera.get_height()/camera.get_width();
	camera_proportion.y =  1.0;
	camera_proportion 	*= 2.0;
	
	//printf("camera_proportion %f :: %f ; scale %f\n", camera_proportion.x, camera_proportion.y, camera_scale);
	//printf("camera_position %f :: %f ; scale %f\n", camera_position.x, camera_position.y, camera_scale);
	
	
	if(draw_graphic_models)
		for(GAME_OBJECT* game_object: scene.game_objects){
			
			if(!game_object->is_visible())continue;
			
			graphic_model = game_object->get_graphic_model();
			if(graphic_model == nullptr)
				continue;
			
			if(graphic_model->get_type() == GRAPHIC_MODEL::T_VECTOR){
				graphic_model_vector = (GRAPHIC_MODEL_VECTOR*)graphic_model;
				COLOR_LIST& 			color_list 		= graphic_model_vector->get_color_list();
				COLOR_LIST::iterator 	color 			= color_list.begin();
				VECTOR_OBJECT* 			vector_object 	= graphic_model_vector->get_vector_object();
				
				if(vector_object == nullptr)
					continue;
				
				build_primitives(graphic_model_vector->get_vector_object(), 
					color_list, color,
					transform_vertex(game_object->get_position() - camera_position, 
						vector2d(0.0, 0.0), camera_reverse_normal, camera_scale),
					rotate_vector(game_object->get_normal()  , camera_reverse_normal), 
					camera_scale
				);

			}
		}
	
	COLLISION_NODE* collision_node;
	if(draw_collision_models)
		for(GAME_OBJECT* game_object: scene.game_objects){
			if(game_object->is_collisible()){
				collision_node = game_object->get_collision_node();
				build_primitives_collision_model(collision_node->get_collision_model(),
					transform_vertex(game_object->get_position() - camera_position, 
						vector2d(0.0, 0.0), camera_reverse_normal, camera_scale),
					rotate_vector(game_object->get_normal()  , camera_reverse_normal), 
					camera_scale);
			}
		}
		
	if(draw_physical_models)
		for(GAME_OBJECT* game_object: scene.game_objects){
			
		}
	
	build_primitives_SHAPE(&camera_rectangle, RGB_COLOR(1.0f, 0.0f, 0.0f), vector2d(0, 0), create_normal(), 1.0);
	
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();

	UINT 		gl_paint_command;
	RGB_COLOR	color;
	//printf("primitives: %d \n",primitive_list.size());
	for(GRAPHIC_VECTOR_PRIMITIVE* primitive: primitive_list){
		switch(primitive->type){
			case GRAPHIC_VECTOR_PRIMITIVE::T_POLYLINE:
				gl_paint_command = GL_LINE_STRIP;
			break;
			case GRAPHIC_VECTOR_PRIMITIVE::T_POLYGON:
				gl_paint_command = GL_LINE_LOOP;
			break;
			default:
				gl_paint_command = GL_LINE_STRIP;
		}
		color = primitive->color;
		
		glColor3f(color.r, color.g, color.b);   
		glBegin(gl_paint_command);
			for(VECTOR2D vertex :primitive->verteces){
				vertex = scale_vector(vertex, camera_proportion);
				glVertex2f(vertex.x, vertex.y);
			}
			
		glEnd();
		delete primitive;
	}
	glPopMatrix();
	
	primitive_list.clear();
	
}