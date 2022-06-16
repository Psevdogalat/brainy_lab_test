#ifndef _GAME_OBJECTS_H_
#define _GAME_OBJECTS_H_

	#include <engine.h>
	
	using namespace GAME_ENGINE;
	
	class METRIC_GRID: public GAME_OBJECT{
		protected:
			double 		width;
			double 		height;
			
		public: 
			METRIC_GRID(double, double);
			~METRIC_GRID();
	};
	
	class GAME_OBJECT_POINT: public GAME_OBJECT{
		public:
			GAME_OBJECT_POINT();
			~GAME_OBJECT_POINT();
	};
	
	class GAME_OBJECT_WALL: public GAME_OBJECT{
		protected:
			double 		width;
			double 		height;
			RGB_COLOR	color;
			
		public:
			GAME_OBJECT_WALL();
			GAME_OBJECT_WALL(double, double, RGB_COLOR );
			
			void spawn		();
			void despawn	();
	};
	
	class GAME_OBJECT_BULLET: public GAME_OBJECT{
		protected: 
			GAME_OBJECT* owner;
			TIMER*		 life_timer;
			double 		 life_time;
			double 		 move_speed;

		public:
			GAME_OBJECT_BULLET();
			
			void spawn	();
			void despawn();
			void compute();
			void collision(GAME_OBJECT* );
			
			void 			set_owner(GAME_OBJECT* );
			GAME_OBJECT*	get_owner();
			
	};
	
	class GAME_OBJECT_PLAYER: public GAME_OBJECT{
		protected:
			double move_speed;
			double rotate_speed;
			double fire_speed;
			
			bool invul;
			void (*on_dead) (GAME_OBJECT* );
			
			bool do_forward_move;
			bool do_backward_move;
			bool do_left_rotate;
			bool do_right_rotate;
			bool do_fire;
			
			bool state_reload;
			
			TIMER* fire_timer;
			
			RGB_COLOR color;
			
		public:
			GAME_OBJECT_PLAYER();
			
			void spawn		();
			void despawn	();
			void compute	();
			void collision	(GAME_OBJECT* );
			
			void set_color	(RGB_COLOR );
			
			void move_forward 	();
			void move_backward 	();
			void rotate_left	();
			void rotate_right	();
			
			void fire			();
			
			void set_invul	(bool );
			bool is_invul	();
			
			void set_on_dead(void (*) (GAME_OBJECT* ));
	};
	

#endif