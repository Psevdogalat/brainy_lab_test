#ifndef _KEYS_H_
#define _KEYS_H_
 
	#include <climits>

	#define KEY_Q 			0
	#define KEY_E 			1
	#define KEY_W 			2
	#define KEY_S 			3
	#define KEY_A 			4
	#define KEY_D 			5
	#define KEY_ARROW_RIGHT 6
	#define KEY_ARROW_LEFT  7
	#define KEY_ARROW_UP 	8
	#define KEY_ARROW_DOWN  9
	#define KEY_SPACE  		10
	#define KEY_SHIFT  		11
	#define KEY_C			12
	#define KEY_MOUSE_M		13
	#define KEY_MOUSE_L		14
	#define KEY_MOUSE_R		15
	#define KEY_X			16
	#define KEY_Z			17

	#define KEY_MAP_LENGTH 18
	#define INVALID_KEY UINT_MAX 
	
	void 			init_key_map		();
	unsigned int 	find_key_by_code	(unsigned int );
	bool 			key_state			(unsigned int );
	bool			key_front_up		(unsigned int );
	bool			key_front_down		(unsigned int );
	void 			toggle_key_down		(unsigned int );
	void 			toggle_key_up		(unsigned int );
	

#endif