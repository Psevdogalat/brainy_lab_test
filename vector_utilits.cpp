#include <vector_utilits.h>
#include <geometry.h>
#include <cstdio>

void print_vector2d(const VECTOR2D& Vector){
	printf("V2D %f %f\n", Vector.x, Vector.y);
}

void print_polyline_additional_info(VECTOR_OBJECT* Object){
	VECTOR_POLYLINE* 		polyline;
	UINT 					quantity;
	VECTOR2D				vertex;
	
	polyline = (VECTOR_POLYLINE*)Object;
	quantity = polyline->get_vertices_quantity();
	
	printf("\t vertices_quantity %d \n", quantity);
	
	for(UINT ivertex = 0; ivertex < quantity; ivertex++){
		polyline->get_vertex(ivertex, vertex);
		printf("\t vertex %4d {% 10.4f % 10.4f}\n", ivertex, vertex.x, vertex.y);
	}
		
}

void print_line_additional_info(VECTOR_OBJECT* Object){
	VECTOR_LINE*	line;
	double 			length;
	double 			center_offset;
	
	line 			= (VECTOR_LINE*)Object;
	length			= line->get_length();
	center_offset	= line->get_center_offset();
	
	printf("\tlength % 10.4f center_offset %.4f\n", length, center_offset);
	
	print_polyline_additional_info(Object);
}

void print_shape_additional_info(VECTOR_OBJECT* Object){
	
	print_polyline_additional_info(Object);
}

void print_rectangle_additional_info(VECTOR_OBJECT* Object){
	VECTOR_RECTANGLE*	rectangle;
	double 				width;
	double 				height;
	
	rectangle 	= (VECTOR_RECTANGLE*)Object;
	width		= rectangle->get_width();
	height		= rectangle->get_height();
	
	printf("\twidth % 10.4f height % 10.4f\n", width, height);
	
	print_shape_additional_info(Object);
}

void print_circle_additional_info(VECTOR_OBJECT* Object){
	VECTOR_CIRCLE*	circle;
	double			radius;
	
	circle 		= (VECTOR_CIRCLE*)Object;
	radius		= circle->get_radius();
	
	printf("\tradius % 10.4f\n", radius);
}

void print_composite_additional_info(VECTOR_OBJECT* Object){
	VECTOR_COMPOSITE*	composite;
	UINT				components_quantity;
	
	composite 			= (VECTOR_COMPOSITE*)Object;
	components_quantity	= composite->get_components_quantity();
	
	printf("\tcomponents_quantity %d\n", components_quantity);
	printf(">>>>===================\n");
	
	for(VECTOR_OBJECT* object: composite->get_components())
		print_vector_object_info(object);
	
	printf("===================<<<\n");
}

void print_vector_object_info(VECTOR_OBJECT* Object){
	UINT 			type;
	const char*		type_name;
	VECTOR2D 		position;
	VECTOR2D		normal;
	double			scale;
	void 			(*additional_info_printer)(VECTOR_OBJECT*);
	
	static const char* type_names[8] = {
		"UNDEFINE "	, "POINT    "	, "POLYLINE ", "LINE     ", 
		"SHAPE    "	, "RECTANGLE"	, "CIRCLE   ", "COMPOSITE"
	};
	
	static void ( *additional_info_printers[6])(VECTOR_OBJECT*) = {
		print_polyline_additional_info	, print_line_additional_info, 
		print_shape_additional_info		, print_rectangle_additional_info,
		print_circle_additional_info	, print_composite_additional_info
	};
	
	additional_info_printer = nullptr;
	
	type = Object->get_type();
	switch(type){
		case VECTOR_OBJECT::T_POINT 	: type_name = type_names[1]; break;
		case VECTOR_OBJECT::T_POLYLINE	: 
			type_name = type_names[2];
			additional_info_printer = additional_info_printers[0];
		break;
		case VECTOR_OBJECT::T_LINE		: 
			type_name = type_names[3]; 
			additional_info_printer = additional_info_printers[1];
		break;
		case VECTOR_OBJECT::T_SHAPE		: 
			type_name = type_names[4]; 
			additional_info_printer = additional_info_printers[2];
		break;
		case VECTOR_OBJECT::T_RECTANGLE	: 
			type_name = type_names[5]; 
			additional_info_printer = additional_info_printers[3];
		break;
		case VECTOR_OBJECT::T_CIRCLE	: 
			type_name = type_names[6]; 
			additional_info_printer = additional_info_printers[4];
		break;
		case VECTOR_OBJECT::T_COMPOSITE	: 
			type_name = type_names[7]; 
			additional_info_printer = additional_info_printers[5];
		break;
		default: 
			type_name = type_names[0];
	}
	
	position 	= Object->get_position();
	normal		= Object->get_normal();	
	scale		= Object->get_scale();
	
	printf("%s position{% 10.4f % 10.4f} norm{% 10.4f % 10.4f} scale % 10.4f\n",
		type_name, position.x, position.y, normal.x, normal.y, scale);
	if(additional_info_printer != nullptr)
		additional_info_printer(Object);
	
}