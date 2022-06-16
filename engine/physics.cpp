#include <physics.h>

/* class PHYSICAL_MODEL */
//=================================================================================================
PHYSICAL_MODEL::PHYSICAL_MODEL(){
	position 		= vector2d(0.0, 0.0);
	velocity 		= vector2d(0.0, 0.0);
	acceleration	= vector2d(0.0, 0.0);
	force			= vector2d(0.0, 0.0);
	pulse			= vector2d(0.0, 0.0);
	
	normal				= create_normal();
	angular_velocity	= 0.0;
	torque				= 0.0;
	mass				= 1.0;
	
}

PHYSICAL_MODEL::~PHYSICAL_MODEL(){
	
}

void PHYSICAL_MODEL::compute(double Frame_time){
	
	position += velocity * Frame_time;
	normal 	 =  rotate_vector(normal, angular_velocity * Frame_time);
				
}