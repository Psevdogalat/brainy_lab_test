#include <utilits.h>

VECTOR_POLYLINE create_arrow(){
	VECTOR_POLYLINE polyline(vector2d(0.0, 0.0), create_normal(),4);
	polyline.set_vertex(0, vector2d(0.0, 0.0));
	polyline.set_vertex(1, vector2d(1.0, 0.0));
	polyline.set_vertex(2, vector2d(0.9, 0.1));
	polyline.set_vertex(3, vector2d(0.9, 0.0));
	return polyline;
}

GRAPHIC_MODEL* create_point_model(){
	GRAPHIC_MODEL_VECTOR* r_model;
	VECTOR_COMPOSITE 	composite;
	VECTOR_POINT 		point;
	VECTOR_CIRCLE		circle(vector2d(0.0, 0.0), create_normal(), 1);
	
	composite.push_component(&point);
	composite.push_component(&circle);
	
	r_model = new GRAPHIC_MODEL_VECTOR();
	r_model->set_vector_object(&composite);
	return r_model;
}


GRAPHIC_MODEL* create_grid_model(double Width, double Height){
	GRAPHIC_MODEL_VECTOR* model;
	
	UINT w_count;
	UINT h_count;
	static const double delta = 0.2; 
	
	VECTOR_COMPOSITE composite;
	RGB_COLOR	lo_color(0.2f, 0.2f, 0.2f);
	RGB_COLOR	hi_color(1.0f, 1.0f, 1.0f);
	
	VECTOR_LINE line1;
	VECTOR_LINE line2;
	
	model = new GRAPHIC_MODEL_VECTOR();
	
	line1 = VECTOR_LINE(vector2d(0.0, 0.0), vector2d(0.0, 1.0), Height	, 0.5);
	line2 = VECTOR_LINE(vector2d(0.0, 0.0), vector2d(1.0, 0.0), Width	, 0.5);
	composite.push_component(&line1);
	composite.push_component(&line2);
	model->push_color(lo_color);
	model->push_color(lo_color);
	
	w_count = ((UINT)(Width /delta))/2;
	h_count = ((UINT)(Height/delta))/2;
	
	for(UINT i = 1; i <= w_count; i++){
		
		if(i%5 == 0){
			line1 = VECTOR_LINE(vector2d( delta*(double)i, 0.0), vector2d(0.0, 1.0), Height, 0.5);
			line2 = VECTOR_LINE(vector2d(-delta*(double)i, 0.0), vector2d(0.0, 1.0), Height, 0.5);
			composite.push_component(&line1);
			composite.push_component(&line2);
			model->push_color(lo_color);
			model->push_color(lo_color);
			
			line1 = VECTOR_LINE(vector2d( delta*(double)i, 0.0), vector2d(0.0, 1.0), 2*delta, 0.5);
			line2 = VECTOR_LINE(vector2d(-delta*(double)i, 0.0), vector2d(0.0, 1.0), 2*delta, 0.5);
			composite.push_component(&line1);
			composite.push_component(&line2);
			model->push_color(hi_color);
			model->push_color(hi_color);
			
		}else{
			line1 = VECTOR_LINE(vector2d( delta*(double)i, 0.0), vector2d(0.0, 1.0),   delta, 0.5);
			line2 = VECTOR_LINE(vector2d(-delta*(double)i, 0.0), vector2d(0.0, 1.0),   delta, 0.5);
			composite.push_component(&line1);
			composite.push_component(&line2);
			model->push_color(hi_color);
			model->push_color(hi_color);
		}
	}
	
	for(UINT i = 1; i <= h_count; i++){
		
		if(i%5 == 0){
			line1 = VECTOR_LINE(vector2d(0.0,  delta*(double)i), vector2d(1.0, 0.0), Width, 0.5);
			line2 = VECTOR_LINE(vector2d(0.0, -delta*(double)i), vector2d(1.0, 0.0), Width, 0.5);
			composite.push_component(&line1);
			composite.push_component(&line2);
			model->push_color(lo_color);
			model->push_color(lo_color);
			
			line1 = VECTOR_LINE(vector2d(0.0,  delta*(double)i), vector2d(1.0, 0.0), 2*delta, 0.5);
			line2 = VECTOR_LINE(vector2d(0.0, -delta*(double)i), vector2d(1.0, 0.0), 2*delta, 0.5);
			composite.push_component(&line1);
			composite.push_component(&line2);
			model->push_color(hi_color);
			model->push_color(hi_color);
			
		}else{
			line1 = VECTOR_LINE(vector2d(0.0,  delta*(double)i), vector2d(1.0, 0.0),   delta, 0.5);
			line2 = VECTOR_LINE(vector2d(0.0, -delta*(double)i), vector2d(1.0, 0.0),   delta, 0.5);
			composite.push_component(&line1);
			composite.push_component(&line2);
			model->push_color(hi_color);
			model->push_color(hi_color);
		}
	}
	
	model->set_vector_object(&composite);
	
	return model;
}

GRAPHIC_MODEL* create_base_unit_model(){
	GRAPHIC_MODEL_VECTOR* r_model;
	
	VECTOR_COMPOSITE 	composite;
	VECTOR_RECTANGLE 	rectangle	(vector2d(0,0)	, create_normal(), 1	, 1	);
	VECTOR_CIRCLE		circle		(vector2d(0,0)	, create_normal(), 0.5		);
	VECTOR_LINE			line		(vector2d(0.5,0), create_normal(), 0.2	, 0	);		
	
	composite.push_component(&rectangle);
	composite.push_component(&circle);
	composite.push_component(&line);
	
	r_model = new GRAPHIC_MODEL_VECTOR();
	r_model->set_vector_object(&composite);
	return r_model;
}
	
GRAPHIC_MODEL* create_bullet_model(){
	GRAPHIC_MODEL_VECTOR* r_model;
	
	VECTOR_CIRCLE	circle	(vector2d(0,0)	, create_normal(), 0.1);		
	
	r_model = new GRAPHIC_MODEL_VECTOR();
	r_model->set_vector_object(&circle);
	r_model->push_color(RGB_COLOR(0.5f, 0.0f, 0.8f));
	return r_model;
}


void scale_vertices(VECTOR_POLYLINE* Polyline, const VECTOR2D& Scale){
	VECTOR2D Vertex;
	for(UINT ivertex = 0; ivertex < Polyline->get_vertices_quantity(); ivertex++){
		Polyline->get_vertex(ivertex,Vertex);
		Vertex.x *= Scale.x;
		Vertex.y *= Scale.y;
		Polyline->set_vertex(ivertex,Vertex);
	}
}

GRAPHIC_MODEL_VECTOR create_wall_model_1(const VECTOR2D& Scale){
	GRAPHIC_MODEL_VECTOR r_model;
	VECTOR_RECTANGLE rectangle(vector2d(0,0)	, create_normal(), Scale.x, Scale.y);
	
	r_model.set_vector_object(&rectangle);
	return r_model;
}


GRAPHIC_MODEL_VECTOR create_wall_model_2(const VECTOR2D& Scale){
	GRAPHIC_MODEL_VECTOR r_model;
	VECTOR_SHAPE shape(vector2d(0,0)	, create_normal(), 8);
	
	shape.set_vertex(0,vector2d(-0.5, 0.5));
	shape.set_vertex(1,vector2d( 0.5, 0.5));
	shape.set_vertex(2,vector2d( 0.4, 0.4));
	shape.set_vertex(3,vector2d(-0.4, 0.4));
	shape.set_vertex(4,vector2d(-0.4,-0.4));
	shape.set_vertex(5,vector2d( 0.4,-0.4));
	shape.set_vertex(6,vector2d( 0.5,-0.5));
	shape.set_vertex(7,vector2d(-0.5,-0.5));
	
	scale_vertices(&shape, Scale);
	
	r_model.set_vector_object(&shape);
	
	return r_model;
}

GRAPHIC_MODEL_VECTOR create_wall_model_3(const VECTOR2D& Scale){
	GRAPHIC_MODEL_VECTOR r_model;
	VECTOR_SHAPE shape(vector2d(0,0)	, create_normal(), 8);
	
	shape.set_vertex(0,vector2d(-0.5, 0.5));
	shape.set_vertex(1,vector2d(   0, 0.2));
	shape.set_vertex(2,vector2d( 0.5, 0.5));
	shape.set_vertex(3,vector2d( 0.2,   0));
	shape.set_vertex(4,vector2d( 0.5,-0.5));
	shape.set_vertex(5,vector2d(   0,-0.2));
	shape.set_vertex(6,vector2d(-0.5,-0.5));
	shape.set_vertex(7,vector2d(-0.2,   0));
	
	scale_vertices(&shape, Scale);
	
	r_model.set_vector_object(&shape);
	
	return r_model;
	
}


GRAPHIC_MODEL_VECTOR create_wall_model_4(const VECTOR2D& Scale){
	GRAPHIC_MODEL_VECTOR r_model;
	VECTOR_SHAPE shape(vector2d(0,0)	, create_normal(), 11);
	
	shape.set_vertex(0 ,vector2d(-0.5, 0.2));
	shape.set_vertex(1 ,vector2d(-0.4, 0.5));
	shape.set_vertex(2 ,vector2d(-0.1, 0.3));
	shape.set_vertex(3 ,vector2d( 0.0, 0.5));
	shape.set_vertex(4 ,vector2d( 0.3, 0.3));
	shape.set_vertex(5 ,vector2d( 0.5, 0.5));
	shape.set_vertex(6 ,vector2d( 0.4, 0.0));
	shape.set_vertex(7 ,vector2d( 0.5,-0.4));
	shape.set_vertex(8 ,vector2d( 0.0,-0.5));
	shape.set_vertex(9 ,vector2d(-0.5,-0.4));
	shape.set_vertex(10,vector2d(-0.3,-0.1));
	
	scale_vertices(&shape, Scale);
	
	r_model.set_vector_object(&shape);
	
	return r_model;
}