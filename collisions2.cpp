#include <collisions2.h>
#include <cstdio>

UINT find_left_bottom_vertex(VECTOR2D* Vertices, UINT Vertices_quantity){
	UINT iret_vertex = 0;
	
	for(UINT ivertex = 1; ivertex < Vertices_quantity; ivertex++)
		if( (Vertices[iret_vertex].x >  Vertices[ivertex].x) || 
			(	(Vertices[iret_vertex].x == Vertices[ivertex].x) && 
				(Vertices[iret_vertex].y >  Vertices[ivertex].y)
			)
		)iret_vertex = ivertex;
	
	return iret_vertex;
}
  
void get_mch_jarvis(VECTOR2D* Vertices, UINT Vertices_quantity, VECTOR2D*& Hull_vertices, UINT& Hull_vertices_quantity){
	UINT*	hull_indexes;
	UINT*	search_indexes;
	UINT	search_indexes_quantity;
	UINT	ivertex;
	UINT 	istart_vertex;
	UINT	icurent_vertex;
	UINT 	inext_vertex;
	
	VECTOR2D vertex;
	
	if(Vertices_quantity == 0){
		Hull_vertices_quantity 	= 0;
		Hull_vertices 			= nullptr;
		
		return;
	}
	
	if(Vertices_quantity < 4){
		Hull_vertices_quantity = Vertices_quantity;
		Hull_vertices = new VECTOR2D[Hull_vertices_quantity];
		for(ivertex = 0; ivertex < Hull_vertices_quantity; ivertex++)
			Hull_vertices[ivertex] = Vertices[ivertex]; 
		
		return;
	}
	
	Hull_vertices_quantity = 0;
	hull_indexes 	= new UINT[Vertices_quantity];
	search_indexes	= new UINT[Vertices_quantity];
	istart_vertex 	= find_left_bottom_vertex(Vertices, Vertices_quantity);	
	
	inext_vertex = istart_vertex;
	do{
		icurent_vertex 	= inext_vertex;
		hull_indexes[Hull_vertices_quantity++] = icurent_vertex;
		
		search_indexes_quantity = 0;
		bool forbidden;
		for(UINT i = 0; i < Vertices_quantity; i++){
			
			forbidden = false;
			for(UINT j = 0; j < Hull_vertices_quantity; j++)
				if(hull_indexes[j] == i){
					forbidden = true;
					break;
				}
			
			if(forbidden)
				continue;
			
			search_indexes[search_indexes_quantity++] = i;
		}
		
		search_indexes[search_indexes_quantity++] = istart_vertex;
		
		inext_vertex = search_indexes[0];
		for(UINT i = 1; i < search_indexes_quantity; i++){
			ivertex = search_indexes[i];
			
			if(vector_product2d(
				Vertices[inext_vertex] - Vertices[icurent_vertex],
				Vertices[ivertex	 ] - Vertices[icurent_vertex]
				) < 0.0
			)inext_vertex = ivertex;
		}
		
		if(Hull_vertices_quantity > Vertices_quantity){
			printf("AAA!!");
			getchar();
		}
	}while(inext_vertex != istart_vertex);
	
	Hull_vertices = new VECTOR2D[Hull_vertices_quantity];
	for(ivertex = 0; ivertex < Hull_vertices_quantity; ivertex++)
		Hull_vertices[ivertex] = Vertices[hull_indexes[ivertex]];
	
	delete [] hull_indexes;
	delete [] search_indexes;
	
	return;
}

void minkovskiy_sum	(
	VECTOR2D* Vertices1		, UINT Vertices1_quantity, 
	VECTOR2D* Vertices2		, UINT Vertices2_quantity, 
	VECTOR2D*& Sum_vertices , UINT& Sum_vertices_quantity
){
	UINT ivertex;
	
	Sum_vertices_quantity = Vertices1_quantity * Vertices2_quantity;
	if(Sum_vertices_quantity == 0){
		Sum_vertices = nullptr;
		return;
	}
	
	Sum_vertices = new VECTOR2D[Sum_vertices_quantity];
	ivertex = 0;
	for(UINT i = 0; i < Vertices1_quantity; i++)
		for(UINT j = 0; j < Vertices2_quantity; j++)
			Sum_vertices[ivertex++] = Vertices1[i] + Vertices2[j];
			
	return;
}

void minkovskiy_diff	(
	VECTOR2D* Vertices1		, UINT Vertices1_quantity, 
	VECTOR2D* Vertices2		, UINT Vertices2_quantity, 
	VECTOR2D*& Diff_vertices, UINT& Diff_vertices_quantity
){
   	UINT ivertex;
	
	Diff_vertices_quantity = Vertices1_quantity * Vertices2_quantity;
	if(Diff_vertices_quantity == 0){
		Diff_vertices = nullptr;
		return;
	}
	
	Diff_vertices = new VECTOR2D[Diff_vertices_quantity];
	ivertex = 0;
	for(UINT i = 0; i < Vertices1_quantity; i++)
		for(UINT j = 0; j < Vertices2_quantity; j++)
			Diff_vertices[ivertex++] = Vertices1[i] - Vertices2[j];
			
	return;
}


double vertex_projection(const VECTOR2D& Vertex, const VECTOR2D& Direction){
	return rotate_vector(Vertex, vector2d(Direction.x, -Direction.y)).x;
}

void gjk_intersection(
	VECTOR2D* 	Vertices1		, UINT  Vertices1_quantity, 
	VECTOR2D* 	Vertices2		, UINT  Vertices2_quantity, 
	VECTOR2D*& 	Simplex			, UINT& Simplex_size
){
	VECTOR2D direction;
	VECTOR2D support_vertex;
	bool	 intersection;
	
	Simplex_size 	= 0;
	Simplex 		= new VECTOR2D[3];
	
	direction 		= vector2d(0.0, 1.0);
	intersection	= false;

	do{
		support_vertex = gjk_support_sco(
			Vertices1, Vertices1_quantity, 
			Vertices2, Vertices2_quantity, 
			direction
		);
		
		if(scalar_product2d(support_vertex, direction) < 0.0)
			break;
		
		Simplex[Simplex_size++] = support_vertex;
		
		intersection = gjk_validate_simplex(Simplex, Simplex_size, direction);

	}while(!intersection);
}

void epa_intersection(
	VECTOR2D* 	Vertices1		, UINT  Vertices1_quantity, 
	VECTOR2D* 	Vertices2		, UINT  Vertices2_quantity, 
	VECTOR2D*& 	Polytope		, UINT& Polytope_size
){
	VECTOR2D* 	Simplex;
	UINT		Simplex_size;
	
	VECTOR2D 		vertex;
	double 			distance;
	EPA_EDGE_INFO	closest_edge;
	
	Polytope 		= nullptr;
	Polytope_size	= 0;
	
	gjk_intersection(
		Vertices1, Vertices1_quantity,
		Vertices2, Vertices2_quantity,
		Simplex	 , Simplex_size
	);
	
	//unformed gjk simplex, no intersection
	if(Simplex_size < 3){
		if(Simplex != nullptr)
			delete [] Simplex;
		
		return;
	}
	
	//alloc memory for polytope, polytope size not higher sco size
	Polytope = new VECTOR2D[Vertices1_quantity * Vertices2_quantity];
	
	//copy simplex in polytope
	for(UINT i = 0; i < Simplex_size; i++)
		Polytope[i] = Simplex[i];
	
	Polytope_size = Simplex_size;
	delete [] Simplex;
	
	
	//polytope expansion
	while(true){
		closest_edge = epa_find_closest_edge(Polytope, Polytope_size);
		
		vertex = gjk_support_sco(
			Vertices1, Vertices1_quantity,
			Vertices2, Vertices2_quantity,
			closest_edge.normal
		);
		
		distance = scalar_product2d(vertex, closest_edge.normal);
		//0.0000001 tolerance constant to prevent looping due floating point error
		if(distance - closest_edge.distance > 0.0000001){
			epa_insert_vertex(
				Polytope			, Polytope_size, 
				closest_edge.index	, vertex
			);
			
		}else
			break;
		
	}
		
}