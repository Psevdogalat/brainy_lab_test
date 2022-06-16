#ifndef _EVENTS_H_
#define _EVENTS_H_
	
	#include <std_defines.h>
	
	namespace GAME_ENGINE{
	
		typedef struct{
			unsigned char 	key_down;
			unsigned int 	key_code;
			unsigned int 	control_key_state;
		}EVENT_KEY_RECORD;
		
		
		typedef struct{
			double	x;
			double 	y;
			int 	delta;
		}EVENT_MOUSE_RECORD;
		
		
		#define EV_KEY_PRESS 		0x0005
		#define EV_MOUSE_WHEEL		0x0006
		#define EV_MOUSE_MOVE		0x0007
		#define EV_MOUSE_LKEY_DOWN	0x0008
		#define EV_MOUSE_LKEY_UP	0x0009
		#define EV_MOUSE_RKEY_DOWN	0x000A
		#define EV_MOUSE_RKEY_UP	0x000B
		#define EV_MOUSE_MKEY_DOWN	0x000C
		#define EV_MOUSE_MKEY_UP	0x000D
		
		
		
		typedef struct{
			UINT 	event;
			union 	{
				EVENT_KEY_RECORD 	key;
				EVENT_MOUSE_RECORD	mouse;
			}event_record;
		}EVENT_MSG;
	
	}
	
#endif