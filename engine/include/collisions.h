#ifndef _COLLISIONS_H_
#define _COLLISIONS_H_

	#include <std_defines.h>
	#include <geometry.h>
	
	typedef struct{
		VECTOR2D 	normal;
		double 		distance;
		UINT		index;
	}EPA_EDGE_INFO;
	
	typedef struct{
		VECTOR2D 	normal;
		double 		distance;
	}EPA_INFO;
	
	class COLLISION_MODEL{
	protected:
		VECTOR2D* 	vertices;
		UINT		vertices_quantity;
		
		friend COLLISION_MODEL* create_collision_model(const VECTOR_SHAPE& ); 
		
	public:
		
		COLLISION_MODEL();
		COLLISION_MODEL(const VECTOR_SHAPE& );
		~COLLISION_MODEL();
		
		UINT 			get_vertices_quantity();
		const VECTOR2D*	get_vertices();
		
	};

	VECTOR2D 		gjk_support			 (const VECTOR2D*, UINT  , const VECTOR2D& );
	VECTOR2D 		gjk_support_sco		 (const VECTOR2D*, UINT  , const VECTOR2D*, UINT, const VECTOR2D&);
	bool 			gjk_validate_simplex (VECTOR2D*, UINT& , VECTOR2D& );
	bool 			gjk_collision		 (const VECTOR2D*, UINT  , const VECTOR2D*	, UINT, VECTOR2D*&, UINT& );
	
	EPA_EDGE_INFO 	epa_find_closest_edge(const VECTOR2D*, UINT );
	void 			epa_insert_vertex	 (VECTOR2D*, UINT& , UINT , VECTOR2D );
	EPA_INFO 		epa_collision_info	 (const VECTOR2D*, UINT  , const VECTOR2D*	, UINT, const VECTOR2D(&)[3]);
	
#endif