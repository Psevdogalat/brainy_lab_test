#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

	#include <std_defines.h>
	#include <geometry.h>
	#include <list>
	
	
	class RGB_COLOR{
	public:
		float r;
		float g;
		float b;
		RGB_COLOR();
		RGB_COLOR(float, float, float);
		RGB_COLOR(const RGB_COLOR& );
		RGB_COLOR& operator= (const RGB_COLOR& );
	};
	

	
	class GRAPHIC_VECTOR_PRIMITIVE{
		public:
			static const UINT T_POLYLINE = 0;
			static const UINT T_POLYGON	 = 1;
			UINT 					type;
			RGB_COLOR				color;
			std::list <VECTOR2D>	verteces;
			
			GRAPHIC_VECTOR_PRIMITIVE();
			GRAPHIC_VECTOR_PRIMITIVE(UINT, const RGB_COLOR& );
			GRAPHIC_VECTOR_PRIMITIVE(const GRAPHIC_VECTOR_PRIMITIVE& );
			GRAPHIC_VECTOR_PRIMITIVE& operator= (const GRAPHIC_VECTOR_PRIMITIVE&);
	};
	
	class GRAPHIC_MODEL{	
		protected:
			UINT type;
			
		public:
			static const UINT T_VECTOR = 1;
			static const UINT T_RASTER = 2;
		
			GRAPHIC_MODEL();
			GRAPHIC_MODEL(const GRAPHIC_MODEL& );
			virtual ~GRAPHIC_MODEL();
			virtual GRAPHIC_MODEL* 	new_copy() = 0;
			
			UINT 	get_type();
			
			
	};
	
	typedef std::list<RGB_COLOR> COLOR_LIST;
	
	class GRAPHIC_MODEL_VECTOR: public GRAPHIC_MODEL{
		protected:
			VECTOR_OBJECT* 			vector_object;
			COLOR_LIST 				color_list;
			
		public:
			GRAPHIC_MODEL_VECTOR();
			GRAPHIC_MODEL_VECTOR(const GRAPHIC_MODEL_VECTOR& );
			GRAPHIC_MODEL_VECTOR(VECTOR_OBJECT* );
			~GRAPHIC_MODEL_VECTOR();
			
			GRAPHIC_MODEL* 	new_copy();
			
			void				set_vector_object	(VECTOR_OBJECT* );
			VECTOR_OBJECT*		get_vector_object	();
			void 				push_color			(RGB_COLOR );
			COLOR_LIST& 		get_color_list		();
			void				clear_color_list	();
			
			GRAPHIC_MODEL_VECTOR& operator= (const GRAPHIC_MODEL_VECTOR& );
			
	};
	
	void set_draw_graphic_models	(bool );
	void set_draw_collision_models	(bool );
	void set_draw_physical_models	(bool );

#endif