#ifndef _PHYSICS_H_
#define _PHYSICS_H_
	
	#include <geometry.h>
	#include <collisions.h>
	
	class PHYSICAL_MODEL{
	public:
		bool		dinamic;
		
		VECTOR2D 	position;
		VECTOR2D 	velocity;
		VECTOR2D	acceleration;
		VECTOR2D	force;
		VECTOR2D	pulse;
		
		VECTOR2D 	normal;
		double 		angular_velocity;
		double 		torque;
		
		double 		mass;
		
		PHYSICAL_MODEL();
		~PHYSICAL_MODEL();
		
		void compute(double );
	};
	
#endif
