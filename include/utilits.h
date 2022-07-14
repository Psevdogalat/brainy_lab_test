#ifndef _UTILITS_H_
#define _UTILITS_H_

	#include <engine.h>
	
	GRAPHIC_MODEL* create_arrow_model();
	GRAPHIC_MODEL* create_point_model();
	GRAPHIC_MODEL* create_grid_model(double, double);
	GRAPHIC_MODEL* create_base_unit_model();
	GRAPHIC_MODEL* create_bullet_model();

	void scale_vertices(VECTOR_POLYLINE* , const VECTOR2D& );
	
	GRAPHIC_MODEL_VECTOR create_wall_model_1(const VECTOR2D& );
	GRAPHIC_MODEL_VECTOR create_wall_model_2(const VECTOR2D& );
	GRAPHIC_MODEL_VECTOR create_wall_model_3(const VECTOR2D& );
	GRAPHIC_MODEL_VECTOR create_wall_model_4(const VECTOR2D& );
	
	

#endif