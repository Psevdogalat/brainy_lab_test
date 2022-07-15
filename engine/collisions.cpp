#include <collisions.h>

/* class COLLISION_MODEL */
//=================================================================================================

COLLISION_MODEL::COLLISION_MODEL(){
	vertices_quantity 	= 0;
	vertices 			= nullptr;
	
}

COLLISION_MODEL::COLLISION_MODEL(const VECTOR_SHAPE& Shape){

	vertices_quantity 	= Shape.get_vertices_quantity();
	vertices 			= new VECTOR2D[vertices_quantity];
	
	for(UINT i = 0; i < vertices_quantity; i++)
		Shape.get_vertex(i, vertices[i]);
	
}

COLLISION_MODEL::~COLLISION_MODEL(){
	if(vertices != nullptr)
		delete [] vertices;
	
}

UINT COLLISION_MODEL::get_vertices_quantity() const{
	return vertices_quantity;
}

const VECTOR2D* COLLISION_MODEL::get_vertices() const{
	return (const VECTOR2D*) vertices;
}

/* GJK algorith implementation */
//=================================================================================================
VECTOR2D gjk_support(
	const VECTOR2D* Vertices, UINT Vertices_quantity, 
	const VECTOR2D& Direction
){
	UINT 		imax_vertex;
	double 		max_distance;
	double 		distance;
	
	if(Vertices_quantity < 1)
		return vector2d(0.0, 0.0);
	
	imax_vertex	= 0;
	max_distance = scalar_product2d(Vertices[imax_vertex], Direction);
		
	for(UINT i = 1; i < Vertices_quantity; i++){
		distance = scalar_product2d(Vertices[i], Direction);
		if(distance > max_distance){
			max_distance 	= distance;
			imax_vertex 	= i;
		}
	}
	
	return Vertices[imax_vertex];
}


VECTOR2D gjk_support_sco(
	const VECTOR2D* Vertices1, UINT Vertices1_quantity, 
	const VECTOR2D* Vertices2, UINT Vertices2_quantity, 
	const VECTOR2D& Direction
){
	return (
		gjk_support(Vertices1, Vertices1_quantity, Direction) - 
		gjk_support(Vertices2, Vertices2_quantity,-Direction)
	);
}


bool gjk_validate_simplex(VECTOR2D* Simplex, UINT& Simplex_size, VECTOR2D& Direction){
	VECTOR2D ba_vector;
	VECTOR2D ac_vector;
	
	switch(Simplex_size){
		case 1:
			Direction = -Direction;
		break;
		case 2:
			ba_vector = Simplex[1] - Simplex[0];
			Direction = vector2d(-ba_vector.y, ba_vector.x);
			
			if(vector_product2d(ba_vector, -Simplex[1]) < 0.0){
				ba_vector  = Simplex[0];
				Simplex[0] = Simplex[1];
				Simplex[1] = ba_vector;
				Direction  = -Direction;
			}
		break;
		case 3:
			ba_vector = Simplex[2] - Simplex[1];
			ac_vector = Simplex[0] - Simplex[2];
			if(vector_product2d(ac_vector, -Simplex[2]) < 0.0){
				if(vector_product2d(ba_vector, -Simplex[1]) < 0.0){
					Simplex[0]		= Simplex[2];
					Simplex_size	= 1;
					Direction		= -Simplex[0];
					
				}else{
					Simplex[1] 		= Simplex[2];
					Simplex_size 	= 2;
					Direction 		= vector2d(ac_vector.y, -ac_vector.x);
				}
				
			}else
				if(vector_product2d(ba_vector, -Simplex[1]) < 0.0){
					Simplex[0]		= Simplex[2];
					Simplex_size 	= 2;
					Direction 		= vector2d(ba_vector.y, -ba_vector.x);
					
				}else
					return true;
		break;
	}
	
	return false;
}


bool gjk_collision(
	const VECTOR2D* Vertices1	, UINT  Vertices1_quantity, 
	const VECTOR2D* Vertices2	, UINT  Vertices2_quantity, 
	VECTOR2D*& 	Simplex			, UINT& Simplex_size
){
	VECTOR2D direction;
	VECTOR2D support_vertex;
	
	Simplex_size 	= 0;
	Simplex 		= new VECTOR2D[3];
	direction 		= vector2d(0.0, 1.0);
	
	while(true){
		support_vertex = gjk_support_sco(
			Vertices1, Vertices1_quantity, 
			Vertices2, Vertices2_quantity, 
			direction
		);
		
		if(scalar_product2d(support_vertex, direction) < 0.0)
			break;
		
		Simplex[Simplex_size++] = support_vertex;
		
		if(gjk_validate_simplex(Simplex, Simplex_size, direction))
			return true;

	}
	
	delete [] Simplex;
	Simplex 		= nullptr;
	Simplex_size 	= 0;
	
	return false;
}

/* EPA algorith implementation */
//=================================================================================================
EPA_EDGE_INFO epa_find_closest_edge(const VECTOR2D* Polytope, UINT Polytope_size){
	EPA_EDGE_INFO 	edge_info;
	UINT 			i;
	VECTOR2D 		edge, normal;
	double 			distance;

	i = Polytope_size-1;
	
	edge 				= Polytope[i] - Polytope[0];
	edge_info.normal 	= normalize_vector(vector2d(-edge.y, edge.x));
	edge_info.distance	= scalar_product2d(Polytope[i], edge_info.normal);
	edge_info.index		= i;
	
	for(i--; i < Polytope_size; i--){
		edge = Polytope[i] - Polytope[i+1];
		normal 		= normalize_vector(vector2d(-edge.y, edge.x));
		distance	= scalar_product2d(Polytope[i], normal);
		if(distance < edge_info.distance){
			edge_info.distance 	= distance;
			edge_info.normal 	= normal;
			edge_info.index		= i;
		}			
	}
	
	return edge_info;
}

void epa_insert_vertex(VECTOR2D* Polytope , UINT& Polytope_size, UINT Index, VECTOR2D Vertex){
	for(UINT i = Polytope_size; i > Index+1; i--)
		Polytope[i] = Polytope[i-1];
	
	Polytope[Index+1] = Vertex;
	Polytope_size++;
}

EPA_INFO epa_collision_info(
	const VECTOR2D* Vertices1, UINT  Vertices1_quantity, 
	const VECTOR2D* Vertices2, UINT  Vertices2_quantity,
	const VECTOR2D* Simplex 	
){
	EPA_INFO 		info;
	VECTOR2D*		polytope;
	UINT			polytope_size;
	UINT			polytope_size_max;
	VECTOR2D 		vertex;
	double 			distance;
	EPA_EDGE_INFO	closest_edge;
	
	//alloc memory for polytope, polytope size not higher sco size
	polytope_size_max =  Vertices1_quantity * Vertices2_quantity;
	polytope = new VECTOR2D[polytope_size_max];
	
	//copy simplex in polytope
	polytope_size = 3;
	for(UINT i = 0; i < polytope_size; i++)
		polytope[i] = Simplex[i];
	
	//polytope expansion
	while(true){
		closest_edge = epa_find_closest_edge(polytope, polytope_size);
		
		vertex = gjk_support_sco(
			Vertices1, Vertices1_quantity,
			Vertices2, Vertices2_quantity,
			closest_edge.normal
		);
		
		distance = scalar_product2d(vertex, closest_edge.normal);
		//0.0000001 tolerance constant to prevent looping due floating point error
		if(distance - closest_edge.distance > 0.0000001 && polytope_size < polytope_size_max){
			epa_insert_vertex(
				polytope			, polytope_size, 
				closest_edge.index	, vertex
			);
			
		}else{
			info.distance 	= closest_edge.distance;
			info.normal		= closest_edge.normal;
			break;
		}
	}
	
	delete [] polytope;
		
	return info;
}

/* raycast to shape implementation */
//=================================================================================================
bool raw_raycast(
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