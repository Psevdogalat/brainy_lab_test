#include <geometry.h>
#include <std_defines.h>

#include <cstdio>
#include <cmath>

/* VECTOR2D methods realisation */

VECTOR2D operator+(const VECTOR2D& V1, const VECTOR2D& V2){
	VECTOR2D r_vector;
	r_vector.x = V1.x + V2.x;
	r_vector.y = V1.y + V2.y;
	return r_vector;
}

VECTOR2D operator-(const VECTOR2D& V1, const VECTOR2D& V2){
	VECTOR2D r_vector;
	r_vector.x = V1.x - V2.x;
	r_vector.y = V1.y - V2.y;
	return r_vector;
}

VECTOR2D operator- (const VECTOR2D& V){
	VECTOR2D r_vector;
	r_vector.x = -V.x;
	r_vector.y = -V.y;
	return r_vector;
}

VECTOR2D& operator+=(VECTOR2D& V1, const VECTOR2D& V2){
	V1.x += V2.x;
	V1.y += V2.y;
	return V1;
}

VECTOR2D& operator-=(VECTOR2D& V1, const VECTOR2D& V2){
	V1.x -= V2.x;
	V1.y -= V2.y;
	return V1;
}

VECTOR2D  operator* (const VECTOR2D& V, double Multiplier){
	VECTOR2D r_vector;
	r_vector.x = V.x * Multiplier;
	r_vector.y = V.y * Multiplier;
	return r_vector;
}

VECTOR2D operator* (double Multiplier, const VECTOR2D& V){
	return V * Multiplier;
}

VECTOR2D  operator/ (const VECTOR2D& V, double Divider){
	VECTOR2D r_vector;
	r_vector.x = V.x / Divider;
	r_vector.y = V.y / Divider;
	return r_vector;
}

VECTOR2D& operator*=(VECTOR2D& V, double Multiplier){
	V.x *= Multiplier;
	V.y *= Multiplier;
	return V;
}

VECTOR2D& operator/=(VECTOR2D& V, double Divider){
	V.x /= Divider;
	V.y /= Divider;
	return V;
}

bool operator==(const VECTOR2D& V1, const VECTOR2D& V2){
	if((V1.x == V2.x) && (V1.y == V2.y))
		return true;
	
	return false;
}

bool operator!=(const VECTOR2D& V1, const VECTOR2D& V2){
	if(	V1 == V2)
		return false;
	
	return true;
}

VECTOR2D vector2d(double X, double Y){
	VECTOR2D r_vector;
	r_vector.x = X;
	r_vector.y = Y;
	return r_vector;
}

VECTOR2D create_normal(){
	return vector2d(1,0);
}

double scalar_product2d(const VECTOR2D& V1, const VECTOR2D& V2){
	return V1.x * V2.x + V1.y * V2.y;
}

double vector_product2d(const VECTOR2D& V1, const VECTOR2D& V2){
	return V1.x * V2.y - V2.x * V1.y;
}

double 	 vector_length		(const VECTOR2D& V){
	return sqrt(scalar_product2d(V,V));
}

VECTOR2D normalize_vector(const VECTOR2D& V){
	double 		v_length;
	
	v_length = vector_length(V);
	
	if(v_length == 0)
		return create_normal();
	
	return V / v_length;
}


VECTOR2D rotate_vector(const VECTOR2D& V, double Angle){
	VECTOR2D r_vector;
	r_vector.x = V.x*cos(Angle) - V.y*sin(Angle);
	r_vector.y = V.x*sin(Angle) + V.y*cos(Angle);
	
	return r_vector;
}

VECTOR2D rotate_vector(const VECTOR2D& V, const VECTOR2D& Rotator){
	VECTOR2D normal;
	VECTOR2D r_vector;
	
	normal = normalize_vector(Rotator);
	r_vector.x = V.x * normal.x  - V.y * normal.y; 
	r_vector.y = V.x * normal.y  + V.y * normal.x;
	
	return r_vector;
}


VECTOR2D scale_vector(const VECTOR2D& V, double Scale){
	return V * Scale;
}

VECTOR2D scale_vector(const VECTOR2D& V, const VECTOR2D& Scale_vector){
	VECTOR2D r_vector;
	
	r_vector.x = V.x * Scale_vector.x;
	r_vector.y = V.y * Scale_vector.y;
	
	return r_vector;
}

VECTOR2D mirror_vector(const VECTOR2D& V, const VECTOR2D& Direction){
	VECTOR2D normal = normalize_vector(Direction);
	return V - 2 * scalar_product2d(V,normal)* normal; 
}

VECTOR2D transform_vertex(const VECTOR2D& Vertex,const VECTOR2D& Position, const VECTOR2D& Normal, double Scale ){
	return rotate_vector(Vertex, Normal) * Scale + Position;
}

/* class VECTOR_OBJECT */

VECTOR_OBJECT::VECTOR_OBJECT(){
	scale 		= 1.0;
	position	= vector2d(0.0, 0.0);
	normal 		= create_normal();
}

VECTOR_OBJECT::VECTOR_OBJECT(const VECTOR2D& Position, const VECTOR2D& Normal){
	scale 		= 1.0;
	position 	= Position;
	normal		= normalize_vector(Normal);
}

VECTOR_OBJECT::VECTOR_OBJECT(const VECTOR_OBJECT& Object){
	type 		= Object.type;
	scale 		= Object.scale;
	position 	= Object.position;
	normal		= Object.normal;
}


VECTOR_OBJECT::~VECTOR_OBJECT(){
	//printf("VECTOR_OBJECT destruct!\n");
}

UINT VECTOR_OBJECT::get_type(){
	return type;
}

void VECTOR_OBJECT::set_position(const VECTOR2D& Position){
	position = Position;
}

VECTOR2D VECTOR_OBJECT::get_position(){
	return position;
}

void VECTOR_OBJECT::set_normal(const VECTOR2D& Normal){
	normal		= normalize_vector(Normal);
}

VECTOR2D VECTOR_OBJECT::get_normal(){
	return normal;
}

void VECTOR_OBJECT::rotate(double Angle){
	normal = rotate_vector(normal, Angle);
}

void VECTOR_OBJECT::rotate(const VECTOR2D& Rotator){
	normal = rotate_vector(normal, Rotator);
}

void VECTOR_OBJECT::move( const VECTOR2D& Vector){
	position += Vector;
}

void VECTOR_OBJECT::set_scale(double Scale){
	if(Scale >= 0.0001 && Scale <= 1000.0)
		scale = Scale;
}

double VECTOR_OBJECT::get_scale(){
	return scale;
}


/* class VECTOR_POINT */

VECTOR_POINT::VECTOR_POINT(){
	type = VECTOR_OBJECT::T_POINT;
}

VECTOR_POINT::VECTOR_POINT(const VECTOR2D& Position, const VECTOR2D& Normale): 
VECTOR_OBJECT(Position, Normale){
	type = VECTOR_OBJECT::T_POINT;
}

VECTOR_POINT::VECTOR_POINT(const VECTOR_POINT& Point):
VECTOR_OBJECT((const VECTOR_OBJECT&)Point){
	type = VECTOR_OBJECT::T_POINT;
}

VECTOR_POINT::~VECTOR_POINT(){
	//printf("Point destruct!\n");
}

VECTOR_OBJECT* VECTOR_POINT::new_copy(){
	return new VECTOR_POINT(*this);
}

VECTOR_POINT& VECTOR_POINT::operator= (const VECTOR_POINT& Point){
	*((VECTOR_OBJECT*)this) = (const VECTOR_OBJECT&)Point;
	
	return *this;
}

/* class VECTOR_POLYLINE */

VECTOR_POLYLINE::VECTOR_POLYLINE(){
	type 				= VECTOR_OBJECT::T_POLYLINE;
	vertices_quantity	= MIN_VERTICES_QUANTITY;
	vertices 			= new VECTOR2D[vertices_quantity];
	
}

VECTOR_POLYLINE::VECTOR_POLYLINE(const VECTOR2D& Position, const VECTOR2D& Normale, UINT Quantity):
VECTOR_OBJECT(Position, Normale){
	type = VECTOR_OBJECT::T_POLYLINE;
	
	if(Quantity < MIN_VERTICES_QUANTITY){
		vertices_quantity = MIN_VERTICES_QUANTITY;
	}else
		vertices_quantity = Quantity;
	
	vertices 			= new VECTOR2D[vertices_quantity];
}

VECTOR_POLYLINE::VECTOR_POLYLINE(const VECTOR_POLYLINE& Polyline):
VECTOR_OBJECT((const VECTOR_OBJECT&) Polyline){
	type = VECTOR_OBJECT::T_POLYLINE;
	
	vertices_quantity 	= Polyline.vertices_quantity;
	vertices 			= new VECTOR2D[vertices_quantity];
	for(UINT ivertex = 0 ; ivertex < vertices_quantity; ivertex++)
		vertices[ivertex] = Polyline.vertices[ivertex];
}

VECTOR_POLYLINE::~VECTOR_POLYLINE(){
	delete [] vertices;
	//printf("VECTOR_POLYLINE destruct!\n");
}

VECTOR_OBJECT* VECTOR_POLYLINE::new_copy(){
	return new VECTOR_POLYLINE(*this);
}

VECTOR_POLYLINE& VECTOR_POLYLINE::operator= (const VECTOR_POLYLINE& Polyline ){
	*((VECTOR_OBJECT*)this) = (const VECTOR_OBJECT&)Polyline;
	
	delete [] vertices;
	
	vertices_quantity 	= Polyline.vertices_quantity;
	vertices 			= new VECTOR2D[vertices_quantity];
	for(UINT ivertex = 0 ; ivertex < vertices_quantity; ivertex++)
		vertices[ivertex] = Polyline.vertices[ivertex];
	
	return *this;
}

UINT VECTOR_POLYLINE::get_vertices_quantity() const{
	return vertices_quantity;
}

RETCODE VECTOR_POLYLINE::get_vertex(UINT Index, VECTOR2D& R_vertex) const{
	if(Index >= vertices_quantity)
		return RET_IDETIFIER_INVALID;
	
	R_vertex = vertices[Index];
	
	return RET_SUCCESS;
}

RETCODE VECTOR_POLYLINE::set_vertex(UINT Index, const VECTOR2D& Vertex){
	if(Index >= vertices_quantity)
		return RET_IDETIFIER_INVALID;
	
	vertices[Index] = Vertex;
	
	return RET_SUCCESS;
}


/* class VECTOR_LINE realisation */

VECTOR_LINE::VECTOR_LINE(){
	type = VECTOR_OBJECT::T_LINE;
}

VECTOR_LINE::VECTOR_LINE(const VECTOR2D& Position ,const VECTOR2D& Normale, double Length, double Center_offset):
	VECTOR_POLYLINE(Position, Normale, VECTOR_POLYLINE::MIN_VERTICES_QUANTITY){
	type = VECTOR_OBJECT::T_LINE;
	
	if(Length < 0.0){
		length = 0.0;
	}else
		length = Length;
	
	if(Center_offset < -1.0){
		center_offset = -1.0;
	}else if(Center_offset > 1.0){
		center_offset = 1.0;
	}else
		center_offset = Center_offset;
	
	vertices[0] = vector2d(-(length * center_offset)		, 0.0);
	vertices[1] = vector2d( (length * (1.0 - center_offset)), 0.0);
}

VECTOR_LINE::VECTOR_LINE(const VECTOR_LINE& Line):
	VECTOR_POLYLINE((const VECTOR_POLYLINE&)Line){
	
	type 			= VECTOR_OBJECT::T_LINE;
	length 			= Line.length;
	center_offset 	= Line.center_offset; 
}

VECTOR_LINE::~VECTOR_LINE(){
	//printf("VECTOR_LINE destruct!\n");
}

VECTOR_OBJECT* VECTOR_LINE::new_copy(){
	return new VECTOR_LINE(*this);
}

VECTOR_LINE& VECTOR_LINE::operator= (const VECTOR_LINE& Line){
	*((VECTOR_POLYLINE*)this) = (const VECTOR_POLYLINE&)Line;
	type 			= VECTOR_OBJECT::T_LINE;
	length 			= Line.length;
	center_offset 	= Line.center_offset;
	return *this;
}

double VECTOR_LINE::get_length(){
	return length;
}

double VECTOR_LINE::get_center_offset(){
	return center_offset;
}

/* class VECTOR_SHAPE */

VECTOR_SHAPE::VECTOR_SHAPE(){
	type = VECTOR_OBJECT::T_SHAPE;
}

VECTOR_SHAPE::VECTOR_SHAPE(const VECTOR2D& Position, const VECTOR2D& Normale, UINT Quantity):
VECTOR_POLYLINE(Position, Normale, Quantity < MIN_VERTICES_QUANTITY?MIN_VERTICES_QUANTITY:Quantity){
	type = VECTOR_OBJECT::T_SHAPE;
}

VECTOR_SHAPE::VECTOR_SHAPE(const VECTOR_SHAPE& Shape):
VECTOR_POLYLINE((const VECTOR_POLYLINE&) Shape){
	type = VECTOR_OBJECT::T_SHAPE;
}

VECTOR_SHAPE::~VECTOR_SHAPE(){
	//printf("VECTOR_SHAPE destruct!\n");
}

VECTOR_OBJECT* VECTOR_SHAPE::new_copy(){
	return new VECTOR_SHAPE(*this);
}

VECTOR_SHAPE& VECTOR_SHAPE::operator= (const VECTOR_SHAPE& Shape){
	*((VECTOR_POLYLINE*)this) = (const VECTOR_POLYLINE&)Shape;
	return *this;
}


/* class VECTOR_RECTANGLE realisation */

VECTOR_RECTANGLE::VECTOR_RECTANGLE():
VECTOR_SHAPE(vector2d(0, 0), create_normal(), 4){
	type 	= VECTOR_OBJECT::T_RECTANGLE;
	width 	= 0;
	height 	= 0;
}

VECTOR_RECTANGLE::VECTOR_RECTANGLE(const VECTOR2D& Position, const VECTOR2D& Normale, double Width, double Height ):
VECTOR_SHAPE(Position, Normale, 4){
	type 	= VECTOR_OBJECT::T_RECTANGLE;
	width 	= std::abs(Width);
	height 	= std::abs(Height);
	
	vertices[0] = vector2d( -width/2, height/2);
	vertices[1] = vector2d(  width/2, height/2);
	vertices[2] = vector2d(  width/2,-height/2);
	vertices[3] = vector2d( -width/2,-height/2);
}

VECTOR_RECTANGLE::VECTOR_RECTANGLE(const VECTOR_RECTANGLE& Rectangle):
VECTOR_SHAPE((const VECTOR_SHAPE&) Rectangle){
	type 	= VECTOR_OBJECT::T_RECTANGLE;
	width	= Rectangle.width;
	height	= Rectangle.height;
}

VECTOR_RECTANGLE::~VECTOR_RECTANGLE(){
	//printf("VECTOR_RECTANGLE destruct!\n");
}

VECTOR_OBJECT* VECTOR_RECTANGLE::new_copy(){
	return new VECTOR_RECTANGLE(*this);
}

VECTOR_RECTANGLE& VECTOR_RECTANGLE::operator= (const VECTOR_RECTANGLE& Rectangle){
	*((VECTOR_SHAPE*)this) = (const VECTOR_SHAPE&)Rectangle;
	
	width	= Rectangle.width;
	height	= Rectangle.height;
	return *this;
}

double VECTOR_RECTANGLE::get_width(){
	return width;
}

double VECTOR_RECTANGLE::get_height(){
	return height;
}


/* class VECTOR_CIRCLE */

VECTOR_CIRCLE::VECTOR_CIRCLE(){
	type 	= VECTOR_OBJECT::T_CIRCLE;
	radius 	= 0;
}

VECTOR_CIRCLE::VECTOR_CIRCLE(const VECTOR2D& Position, const VECTOR2D& Normale, double Radius):
VECTOR_OBJECT(Position, Normale){
	type 	= VECTOR_OBJECT::T_CIRCLE;
	radius 	= std::abs(Radius);
}

VECTOR_CIRCLE::VECTOR_CIRCLE(const VECTOR_CIRCLE& Circle):
VECTOR_OBJECT((const VECTOR_OBJECT&)Circle){
	type 	= VECTOR_OBJECT::T_CIRCLE;
	radius 	= Circle.radius;
}

VECTOR_CIRCLE::~VECTOR_CIRCLE(){
	//printf("VECTOR_CIRCLE destruct!\n");
}

VECTOR_OBJECT* VECTOR_CIRCLE::new_copy(){
	return new VECTOR_CIRCLE(*this);
}

double VECTOR_CIRCLE::get_radius(){
	return radius;
}

/* class VECTOR_COMPOSITE */

VECTOR_COMPOSITE::VECTOR_COMPOSITE(){
	type 				= VECTOR_OBJECT::T_COMPOSITE;
	components_quantity = 0;
}

VECTOR_COMPOSITE::VECTOR_COMPOSITE(const VECTOR2D& Position, const VECTOR2D& Normale):
VECTOR_OBJECT(Position, Normale){
	type 				= VECTOR_OBJECT::T_COMPOSITE;
	components_quantity = 0;
}

VECTOR_COMPOSITE::VECTOR_COMPOSITE(const VECTOR_COMPOSITE& Composite ):
VECTOR_OBJECT((const VECTOR_OBJECT&)Composite){
	type 				= VECTOR_OBJECT::T_COMPOSITE;
	components_quantity = Composite.components_quantity;
	for(VECTOR_OBJECT* object: Composite.components)
		components.push_back(object->new_copy());
}

VECTOR_COMPOSITE::~VECTOR_COMPOSITE(){
	for(VECTOR_OBJECT* object: components)
		delete object;
}

VECTOR_OBJECT* VECTOR_COMPOSITE::new_copy(){
	return new VECTOR_COMPOSITE(*this);
}

VECTOR_COMPOSITE& VECTOR_COMPOSITE::operator= (const VECTOR_COMPOSITE& Composite){
	*((VECTOR_OBJECT*)this) = (const VECTOR_OBJECT&)Composite;
	
	components_quantity = Composite.components_quantity;
	for(VECTOR_OBJECT* object: Composite.components)
		components.push_back(object->new_copy());
	
	return *this;
}

UINT VECTOR_COMPOSITE::get_components_quantity(){
	return components_quantity;
}

void VECTOR_COMPOSITE::push_component(VECTOR_OBJECT* Object){
	if(Object != nullptr){
		components.push_back(Object->new_copy());
		components_quantity++;
	}
}

const std::list<VECTOR_OBJECT*>& VECTOR_COMPOSITE::get_components(){
	return (const std::list<VECTOR_OBJECT*>&)components;
}