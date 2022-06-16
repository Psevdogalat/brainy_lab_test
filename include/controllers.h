#ifndef _CONTROLLERS_H_
#define _CONTROLLERS_H_
	
	#include <engine.h>
	#include <objects.h>
	
	class CONTROLLER{
		protected:
		
		public:
			CONTROLLER();
			virtual ~CONTROLLER();
			virtual void init	() 	= 0;
			virtual void compute() 	= 0;
	};
	
	class PLAYER_CONTROLLER{
		protected:
			GAME_OBJECT_PLAYER* control_unit;
		
		public:
			PLAYER_CONTROLLER();
			~PLAYER_CONTROLLER();
			
			void init	();
			void compute();
			
			void set_control_unit(GAME_OBJECT_PLAYER* );
	};
	
	class ARTIFICIAL_PLAYER_CONTROLLER{
		protected:
			GAME_OBJECT_PLAYER* control_unit;
			GAME_OBJECT_PLAYER*	enemy_unit;
			
			TIMER* dummy_timer;
			double dummy_time;
		
		public:
			ARTIFICIAL_PLAYER_CONTROLLER();
			~ARTIFICIAL_PLAYER_CONTROLLER();
			
			void init	();
			void compute();
			
			void set_control_unit	(GAME_OBJECT_PLAYER* );
			void set_enemy_unit		(GAME_OBJECT_PLAYER* );
	};
	
	
#endif