#ifndef _COLLISIONS2_H_
#define _COLLISIONS2_H_

	#include <geometry.h>
	#include <collisions.h>
	
	UINT find_left_bottom_vertex(VECTOR2D*, UINT );
	UINT find_distant_vertex	(VECTOR2D*, UINT, const VECTOR2D& );
	
	void get_mch_jarvis		(VECTOR2D*, UINT, VECTOR2D*&, UINT& );
	void minkovskiy_sum		(VECTOR2D*, UINT, VECTOR2D*, UINT, VECTOR2D*&, UINT& );
	void minkovskiy_diff	(VECTOR2D*, UINT, VECTOR2D*, UINT, VECTOR2D*&, UINT& );
	void gjk_intersection	(VECTOR2D*, UINT, VECTOR2D*, UINT, VECTOR2D*&, UINT& );
	void epa_intersection	(VECTOR2D*, UINT, VECTOR2D*, UINT, VECTOR2D*&, UINT& );

#endif