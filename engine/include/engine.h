#ifndef _ENGINE_H_
#define	_ENGINE_H_

	#include <std_defines.h>
	#include <geometry.h>
	#include <graphics.h>
	#include <collisions.h>
	#include <physics.h>
	#include <events.h>
	
	#include <list>
	
	namespace GAME_ENGINE{
		
		class TIMER{
			protected:
				double time;
			
			public:
				
				TIMER();
				~TIMER();
				
				bool condition	();
				void set		(double );
				void countdown	(double );
			
		};
		
		class COLLISION_NODE; //for cross reference;
		
		class GAME_OBJECT{
			protected:
				bool visible;
				
				char*				name;
				UINT 				type;
				
				VECTOR2D			position;
				VECTOR2D			normal;
				
				GRAPHIC_MODEL*		graphic_model;
				PHYSICAL_MODEL*		physical_model;
				COLLISION_NODE*		collision_node;

			public:
				static const UINT GOT_UNDEFINE 	= 0;
				static const UINT GOT_POINT		= 1;
				static const UINT GOT_WALL		= 2;
				static const UINT GOT_BULLET	= 3;
				static const UINT GOT_PLAYER	= 4;	
				
				GAME_OBJECT();
				GAME_OBJECT(const char*, const VECTOR2D&, const VECTOR2D& );
				virtual ~GAME_OBJECT();
				
				
				//virtual GAME_OBJECT* 	new_copy() = 0;
				virtual void compute();
				virtual void collision(GAME_OBJECT* );
				virtual void spawn();
				virtual void despawn();
				
				const char*		get_name();
				void 			set_name(const char* );
				
				const UINT 		get_type();
				
				void 			set_graphic_model	(GRAPHIC_MODEL* );
				GRAPHIC_MODEL* 	get_graphic_model	();
				void 			set_visible			(bool );
				bool 			is_visible			();
				
				void 			 set_collision_node	(COLLISION_NODE* );
				COLLISION_NODE*  get_collision_node	();
				bool			 is_collisible		();
				
				void 			set_physical_model	(PHYSICAL_MODEL* );
				PHYSICAL_MODEL*	get_physical_model	();
				bool			is_physical			();
				
				void 			set_position		(const VECTOR2D& );
				VECTOR2D		get_position		();
				
				void 			set_normal			(const VECTOR2D& );
				VECTOR2D		get_normal			();
				
		};
		
		typedef struct{
			COLLISION_NODE* 	node;
			enum{TYPE_RAW_GJK,TYPE_EPA} 	type;
			
			union{			
				VECTOR2D 		simplex[3];
				EPA_INFO		epa_info;
			};
			
		}COLLISION_INFO;
		
		typedef std::list<COLLISION_INFO> COLLISIONS_LIST;
		
		class COLLISION_NODE{
			protected:
				GAME_OBJECT*		game_object;
				bool 				active;
				bool 				passive;
				bool				epa;
				COLLISION_MODEL*	model;
				COLLISIONS_LIST		collisions;
				
			public:
			
				COLLISION_NODE(GAME_OBJECT*, bool, bool, bool, COLLISION_MODEL*);
				~COLLISION_NODE();
				
				bool is_active	() const;
				bool is_passive	() const;
				bool is_epa		() const;
				
				void 					add_collision	(const COLLISION_INFO& );
				const COLLISIONS_LIST& 	get_collisions	()const;
				void 					clear_collisions();

				GAME_OBJECT*			get_game_object		()const;
				COLLISION_MODEL*		get_collision_model	()const;
				
				void 					compute(COLLISION_NODE* );
				
		};
		
		class CAMERA: public VECTOR_RECTANGLE{
			protected:
				VECTOR_RECTANGLE constraint;
				
			public:
				CAMERA();
				~CAMERA();
				
				void move			(const VECTOR2D& );
				void set_area		(const VECTOR_RECTANGLE& );
				void set_constraint (const VECTOR_RECTANGLE& );

		};

		typedef std::list<GAME_OBJECT* > 	GAME_OBJECTS_LIST;
		typedef std::list<TIMER*>		 	TIMERS_LIST;
		typedef std::list<PHYSICAL_MODEL*>	PHYSICS_LIST;
		typedef std::list<COLLISION_NODE*>	COLLISION_NODE_LIST;

		class SCENE{
			public :
				CAMERA 				camera;
				GAME_OBJECTS_LIST	game_objects;
				GAME_OBJECTS_LIST	spawn_list;
				GAME_OBJECTS_LIST	despawn_list;
				COLLISION_NODE_LIST	active_collisions;
				COLLISION_NODE_LIST	passive_collisions;
				
			public:
				SCENE();
				~SCENE();
				
		};	
		
		class ENGINE{
			private:
				SCENE 			scene;
				TIMERS_LIST 	timers;
				PHYSICS_LIST 	physical_models;
				
			public:
				ENGINE();
				~ENGINE();
				
				void init				();
				void free				();
				void init_scene			();
				void free_scene			();
				void init_graphic		();
				
				void key_parser			(const char* );
				
				void event				(const EVENT_MSG& );
				void compute			(double );
				void compute_objects 	(double );
				void compute_timers		(double );
				void compute_physics	(double );
				void compute_collisions	();
				void post_collisions	(double);
				void pre_render			();
				void render				();
				
				TIMER* 			allocate_timer			();
				void 			free_timer				(TIMER* );
				
				PHYSICAL_MODEL* allocate_physical_model	();
				void 			free_physical_model		(PHYSICAL_MODEL* );
				
				COLLISION_NODE*	allocate_collision_node	(GAME_OBJECT*, bool, bool, bool, COLLISION_MODEL* );
				void			free_collision_node		(COLLISION_NODE* );
				
				void 			process_spawn_list		();
				void 			process_despawn_list	();
				
				GAME_OBJECT* 	spawn_game_object		(GAME_OBJECT*, const VECTOR2D&, const VECTOR2D& );
				void 			despawn_game_object		(GAME_OBJECT* );
				
		};
		
		
		/* platform call methods */
		void platform_init		(const char* );
		void platform_free		();
		
		void platform_compute	(double );
		void platform_render 	();
		void platform_event		(const EVENT_MSG& );
		
		/* common engine methods */
		TIMER* 				allocate_timer			();
		void 				free_timer				(TIMER* );
		
		PHYSICAL_MODEL*		allocate_physical_model	();
		void 				free_physical_model		(PHYSICAL_MODEL* );
		
		COLLISION_NODE*		allocate_collision_node	(GAME_OBJECT*, bool, bool, bool, COLLISION_MODEL* );
		void				free_collision_node		(COLLISION_NODE* );
		
		GAME_OBJECT* 		spawn					(GAME_OBJECT*, const VECTOR2D&, const VECTOR2D& );
		void 				despawn					(GAME_OBJECT* );
		
	}

#endif