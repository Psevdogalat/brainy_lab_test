#include <cstdlib>
#include <cstdio>
#include <cstring> 

#include <engine.h>
#include <algorithm>

using namespace GAME_ENGINE;

/* engine instance */
	static ENGINE engine;

/* class TIMER */
//=================================================================================================
	TIMER::TIMER(){time = 0.0; }
	TIMER::~TIMER(){ }
	
	bool TIMER::condition(){
		return time == 0.0;
	}
	
	void TIMER::set(double Time){
		time = Time;
	}
	
	void TIMER::countdown(double Delta){
		if((time -= Delta) < 0.0)
			time = 0.0;		
	}

/* class COLLISION_NODE */
//=================================================================================================
	COLLISION_NODE::COLLISION_NODE(GAME_OBJECT* Game_object, bool Active, bool Passive, bool Epa, COLLISION_MODEL* Model){
		game_object = Game_object;
		model		= Model;
		active		= Active;
		passive		= Passive;
		epa			= Epa;
		
	}
	
	COLLISION_NODE::~COLLISION_NODE(){
		
		
	}
	
	bool COLLISION_NODE::is_active	() const{
		return active;
		
	}
	
	bool COLLISION_NODE::is_passive	() const{
		return passive;
		
	}
	
	bool COLLISION_NODE::is_epa		() const{
		return epa;
		
	}
	
	void COLLISION_NODE::add_collision(const COLLISION_INFO&  Info){
		collisions.push_back(Info);
		
	}
	
	const COLLISIONS_LIST& 	COLLISION_NODE::get_collisions	()const{
		return collisions;
		
	}
	
	void COLLISION_NODE::clear_collisions(){
		collisions.clear();
		
	}

	GAME_OBJECT*	COLLISION_NODE::get_game_object	()const{
		return game_object;
		
	}
	
	COLLISION_MODEL*	COLLISION_NODE::get_collision_model() const{
		return model;
		
	}
	
	void COLLISION_NODE::compute(COLLISION_NODE* Partner_node){
		bool 				collised;
		
		COLLISION_INFO 		info;
		COLLISION_INFO		info_partner;
		
		GAME_OBJECT*		object1;
		GAME_OBJECT*		object2;
		
		COLLISION_MODEL*	collision_model1;
		COLLISION_MODEL*	collision_model2;
		
		const VECTOR2D*		model_vertices1;
		const VECTOR2D*		model_vertices2;
		VECTOR2D*			vertices1;
		VECTOR2D*			vertices2;
		UINT				vertices1_quantity;
		UINT				vertices2_quantity;
		VECTOR2D*			simplex;
		UINT				simplex_size;

		collised 	= false;

		object1		= get_game_object();
		object2		= Partner_node->get_game_object();
		
		for(COLLISION_INFO collision: collisions){
			if(collision.node == Partner_node){
				collised = true;
				info = collision;
				break;
			}
		}
		
		if(!collised){
			collision_model1 = get_collision_model();
			collision_model2 = Partner_node->get_collision_model();
			
			vertices1_quantity 	= collision_model1->get_vertices_quantity();					
			model_vertices1		= collision_model1->get_vertices();
			vertices1			= new VECTOR2D[vertices1_quantity];
			
			for(UINT i = 0; i < vertices1_quantity; i++)
				vertices1[i] = transform_vertex(model_vertices1[i],
					(const VECTOR2D)object1->get_position(),
					(const VECTOR2D)object1->get_normal(), 
					1.0);
			
			
					
			vertices2_quantity 	= collision_model2->get_vertices_quantity();
			model_vertices2 	= collision_model2->get_vertices();
			vertices2			= new VECTOR2D[vertices2_quantity];
			
			for(UINT i = 0; i < vertices2_quantity; i++)
				vertices2[i] = transform_vertex(model_vertices2[i],
					(const VECTOR2D)object2->get_position(),
					(const VECTOR2D)object2->get_normal(), 
					1.0);
			
			if(gjk_collision(
				vertices2, 	vertices2_quantity,
				vertices1, 	vertices1_quantity,
				simplex, 	simplex_size)
			){
				info.node = Partner_node;
				
				if(epa && Partner_node->epa){
					info.type = COLLISION_INFO::TYPE_EPA;
					info.epa_info =  epa_collision_info(
						vertices2, vertices2_quantity,
						vertices1, vertices1_quantity,
						(const VECTOR2D*)simplex
					);
					
				}else{
					info.type = COLLISION_INFO::TYPE_RAW_GJK;
					for(UINT i = 0; i < 3; i++)
						info.simplex[i] = simplex[i];
					
				}
				
				add_collision(info);
				
				if(Partner_node->is_active()){
					info_partner = info;
					info_partner.node = this;
					if(info.type == COLLISION_INFO::TYPE_EPA){
						info_partner.epa_info.normal *= -1.0;
					}
					Partner_node->add_collision(info_partner);
				}
				
				delete [] simplex;
				
				collised = true;
			}
			
			delete [] vertices1;
			delete [] vertices2;
			
		}
		if(collised){
			object1->collision(object2, &info);
		
		}
	}
	
/* class GAME_OBJECT */
/* basic game object class */
//=================================================================================================
	GAME_OBJECT::GAME_OBJECT(){
		type 			= GOT_UNDEFINE;
		
		visible 		= true;
		
		position		= {0.0f, 0.0f};
		normal			= create_normal();
		
		name 			= nullptr;
		collision_node 	= nullptr;
		graphic_model 	= nullptr;
		physical_model  = nullptr;
	}

	GAME_OBJECT::GAME_OBJECT(const char* Name, const VECTOR2D& Position, const VECTOR2D& Normal):GAME_OBJECT(){
		set_name(Name);
		position 		= Position;
		normal			= normalize_vector(Normal);
	}

	GAME_OBJECT::~GAME_OBJECT(){
		set_name			(nullptr);
		set_graphic_model	(nullptr);
		set_collision_node	(nullptr);
	}
	
	const char*	GAME_OBJECT::get_name() const{
		return (const char*) name;
	}
	
	void GAME_OBJECT::set_name(const char* Name){
		if(name != nullptr){
			free(name);
			name = nullptr;
		}
		
		if(Name != nullptr){
			name = (char*)malloc(strlen(Name)+1);
			strcpy(name,Name);
		}
	}
	
	const UINT GAME_OBJECT::get_type() const{
		return type;
	}
	
	void GAME_OBJECT::compute(){ };
	
	void GAME_OBJECT::collision(GAME_OBJECT* Object, const COLLISION_INFO* Param){
		//printf("GO %s collision with GO %s\n", get_name(), Object->get_name());
	};
	
	void GAME_OBJECT::spawn(){};
	void GAME_OBJECT::despawn(){};
	
	//-------------------------------------------------------------------------
	void GAME_OBJECT::set_graphic_model(GRAPHIC_MODEL* Graphic_model){
		graphic_model = Graphic_model;
		
	}
	
	GRAPHIC_MODEL* GAME_OBJECT::get_graphic_model() const{
		return graphic_model;
	}
	
	void GAME_OBJECT::set_visible(bool Visible){
			visible = Visible;
	}
	
	bool GAME_OBJECT::is_visible() const{
		if(graphic_model != nullptr && visible)
			return true;
		
		return false;
	}
	
	//-------------------------------------------------------------------------
	void GAME_OBJECT::set_collision_node(COLLISION_NODE* Node){
		collision_node = Node;
	}

	COLLISION_NODE* GAME_OBJECT::get_collision_node() const{
		return collision_node;
	}
	
	bool GAME_OBJECT::is_collisible() const{
		if(collision_node != nullptr)
			return true;
			
		return false;
	}
	
	//-------------------------------------------------------------------------
	void GAME_OBJECT::set_physical_model(PHYSICAL_MODEL* Model_ptr){	
		physical_model = Model_ptr;
	}
	
	PHYSICAL_MODEL*	GAME_OBJECT::get_physical_model() const{
		return physical_model;
	}
	
	bool GAME_OBJECT::is_physical() const{
		if(physical_model != nullptr)
			return true;
		
		return false;
	}
	
	//-------------------------------------------------------------------------
	void 	GAME_OBJECT::set_position(const VECTOR2D& Position ){
		position = Position;
		
		if(physical_model != nullptr)
			physical_model->position = position;	
	}
	
	VECTOR2D	GAME_OBJECT::get_position() const{
		return position;
	}
	
	void 	GAME_OBJECT::set_normal(const VECTOR2D& Normal){
		normal = normalize_vector(Normal);
		
		if(physical_model != nullptr)
			physical_model->normal = normal;
	}
	
	VECTOR2D	GAME_OBJECT::get_normal() const{
		return normal;
	}
	
	
	

/* class CAMERA */
//=================================================================================================
	CAMERA::CAMERA(){
		
	}

	CAMERA::~CAMERA(){
		
	}
	
	void CAMERA::set_area(const VECTOR_RECTANGLE& Area){
		*((VECTOR_RECTANGLE*)this) = Area;
	}
	
	void CAMERA::set_constraint(const VECTOR_RECTANGLE& Constraint){
		constraint = Constraint;
	}
	
	void CAMERA::move(const VECTOR2D& Position){
		VECTOR2D new_position;
		
		VECTOR2D constraint_vertices[4];
		VECTOR2D constraint_position;
		double	 constraint_scale;
		
		constraint_scale	= constraint.get_scale();
		constraint_position = constraint.get_position();
		
		for(UINT ivertex=0; ivertex < 4; ivertex++ ){
			constraint.get_vertex(ivertex, constraint_vertices[ivertex]);
			constraint_vertices[ivertex] *= constraint_scale;
			constraint_vertices[ivertex] += constraint_position;
		}
		
		new_position = Position;
		if(new_position.x < constraint_vertices[3].x){
			new_position.x = constraint_vertices[3].x;
			
		}else if(new_position.x > constraint_vertices[1].x){
			new_position.x = constraint_vertices[1].x;
			
		}
		
		if(new_position.y < constraint_vertices[3].y){
			new_position.y = constraint_vertices[3].y;
			
		}else if(new_position.y > constraint_vertices[1].y){
			new_position.y = constraint_vertices[1].y;
			
		}
		
		position = new_position;
	}

/* class SCENE */
//=================================================================================================
	SCENE::SCENE(){
		printf("scene construct!\n");
	}

	SCENE::~SCENE(){
		for(GAME_OBJECT* object: spawn_list)
			delete object;
			
		for(GAME_OBJECT* object: game_objects)
			delete object;
		
		printf("scene destruct!\n");
	}

/* engine methods definition */
//=================================================================================================
	ENGINE::ENGINE(){
		
	}
	
	ENGINE::~ENGINE(){
		
	}
	
	void ENGINE::init(){
		init_graphic();
		init_scene();
	}
	
	void ENGINE::free(){
		
		free_scene();
		
		for(TIMER* timer: timers)
			delete timer;
		
		for(PHYSICAL_MODEL* model: physical_models)
			delete model;
			
	}

	void ENGINE::key_parser(const char* ){
		
	}
	
	/* timers */
	//=============================================================================================
	TIMER* 	ENGINE::allocate_timer(){
		TIMER* new_timer = new TIMER();
		timers.push_back(new_timer);
		return new_timer;
	}

	void ENGINE::free_timer(TIMER* Timer_ptr){
		TIMERS_LIST::iterator timer_container;
		
		timer_container = std::find(timers.begin(), timers.end(), Timer_ptr);
		if(timer_container != timers.end()){
			timers.erase(timer_container);
			delete Timer_ptr;
		}
			
	}

	void ENGINE::compute_timers(double Frame_time){
		for(TIMER* timer: timers)
			timer->countdown(Frame_time);
		
	}
	
	void ENGINE::compute_objects(double Frame_time){
		for(GAME_OBJECT* object: scene.game_objects )
			object->compute();
		
	}
	
	/* physics and collisions */ 
	//=============================================================================================
	PHYSICAL_MODEL* ENGINE::allocate_physical_model(){
		PHYSICAL_MODEL* new_physical_model  = new PHYSICAL_MODEL();
		physical_models.push_back(new_physical_model);
		return new_physical_model;
		
	}

	void ENGINE::free_physical_model(PHYSICAL_MODEL* Physical_model_ptr){
		PHYSICS_LIST::iterator model_container;
		
		model_container = std::find(physical_models.begin(), physical_models.end(), Physical_model_ptr);
		if(model_container != physical_models.end()){
			physical_models.erase(model_container);
			delete Physical_model_ptr;
		}
			
	}
	
	COLLISION_NODE*	ENGINE::allocate_collision_node(GAME_OBJECT* Object, bool Active, bool Passive, bool Epa, COLLISION_MODEL* Model){
		COLLISION_NODE* node;
		
		if(Object == nullptr)
			return nullptr;
		
		if(Model == nullptr)
			return nullptr;
		
		node = new COLLISION_NODE(Object, Active, Passive, Epa, Model);
		
		if(Active)
			scene.active_collisions.push_back(node);
		
		if(Passive)
			scene.passive_collisions.push_back(node);
		
		return node;
	}
	
	void ENGINE::free_collision_node(COLLISION_NODE* Node){
		if(Node == nullptr)
			return;
		
		if(Node->is_active())
			scene.active_collisions.remove(Node);
		
		if(Node->is_passive())
			scene.passive_collisions.remove(Node);
		
		delete Node;
	}
	
	const COLLISION_NODE_LIST* ENGINE::get_passive_collision_list(){
		return &scene.passive_collisions;
	}

	void ENGINE::compute_physics(double Frame_time){
		PHYSICAL_MODEL* 	physical_model;
		
		for(GAME_OBJECT* object: scene.game_objects){
			
			if(!object->is_physical())
				continue;
		
			physical_model = object->get_physical_model();			
		
			physical_model->position 	= object->get_position();
			physical_model->normal		= object->get_normal();
			
			physical_model->compute(Frame_time);
			
			object->set_position(physical_model->position);
			object->set_normal	(physical_model->normal);
			
		}
	}
	
	void ENGINE::compute_collisions	(){
		for(COLLISION_NODE* active_collision : scene.active_collisions){
			for(COLLISION_NODE* passive_collision: scene.passive_collisions){
				if(active_collision == passive_collision)
					continue;
				
				active_collision->compute(passive_collision);
			}
			
			active_collision->clear_collisions();
		}
	}
	
	void ENGINE::post_collisions(double Frame_time){
		
	}
	
	
	/* spawn / despawn */ 
	//=============================================================================================
	void ENGINE::process_spawn_list(){
		
		for(GAME_OBJECT* object: scene.spawn_list){
			object->spawn();
			scene.game_objects.push_back(object);
		}
		
		scene.spawn_list.clear();
	}
	
	void ENGINE::process_despawn_list(){
		GAME_OBJECTS_LIST::iterator it;
		for(GAME_OBJECT* object: scene.despawn_list){
			
			it = std::find(scene.game_objects.begin(), scene.game_objects.end(), object);
			if(it != scene.game_objects.end()){
				scene.game_objects.erase(it);
				object->despawn();
				delete object;
			}
		}
		
		scene.despawn_list.clear();
	}
	
	GAME_OBJECT* ENGINE::spawn_game_object(GAME_OBJECT* Object, const VECTOR2D& Position, const VECTOR2D& Normal){
		PHYSICAL_MODEL* physical_model;
		
		if(Object == nullptr)
			return nullptr;
		
		Object->set_position(Position);
		Object->set_normal(Normal);
		
		scene.spawn_list.push_back(Object);

		return Object;
	}
	
	void ENGINE::despawn_game_object(GAME_OBJECT* Object){
		if(Object == nullptr)
			return;
		
		scene.despawn_list.push_back(Object);
			
	}
	

/* platform methods definition */
//=================================================================================================
	void GAME_ENGINE::platform_init(const char* Cmd_line){
		engine.key_parser((const char*) Cmd_line);
		engine.init();
	}

	void GAME_ENGINE::platform_free(){
		engine.free();
	};

	void GAME_ENGINE::platform_compute(double Frame_time){
		
		engine.process_spawn_list();
		
		engine.compute_timers	(Frame_time);
		engine.compute			(Frame_time);
		engine.compute_objects	(Frame_time);
		
		engine.compute_physics(Frame_time);
		engine.compute_collisions();
		engine.post_collisions(Frame_time);
		
		engine.process_despawn_list();
	}

	void GAME_ENGINE::platform_render(){

		engine.pre_render();
		engine.render();
	}
	
	void GAME_ENGINE::platform_event(const EVENT_MSG& Message){
		engine.event(Message);
	}

/* common engine methods definition */
//=================================================================================================
	TIMER* 	GAME_ENGINE::allocate_timer	(){
		return engine.allocate_timer();
	}
	
	void GAME_ENGINE::free_timer(TIMER* Timer_ptr ){
		engine.free_timer(Timer_ptr);
	}

	PHYSICAL_MODEL*	GAME_ENGINE::allocate_physical_model(){
		return engine.allocate_physical_model();
	}
	
	void GAME_ENGINE::free_physical_model(PHYSICAL_MODEL* Model_ptr){
		engine.free_physical_model(Model_ptr);
	}

	COLLISION_NODE*	GAME_ENGINE::allocate_collision_node(GAME_OBJECT* Object, bool Active, bool Passive, bool Epa, COLLISION_MODEL* Model){
		return engine.allocate_collision_node(Object, Active, Passive, Epa, Model);
	}
	
	void GAME_ENGINE::free_collision_node(COLLISION_NODE* Node){
		engine.free_collision_node(Node);
	}
	
	const COLLISION_NODE_LIST* GAME_ENGINE::get_passive_collision_list(){
		return engine.get_passive_collision_list();
	}

	GAME_OBJECT* GAME_ENGINE::spawn(GAME_OBJECT* Object, const VECTOR2D& Position, const VECTOR2D& Normal){
		return engine.spawn_game_object(Object, Position, Normal); 
	}
	
	void GAME_ENGINE::despawn(GAME_OBJECT* Object){
		engine.despawn_game_object(Object);
		
	}


