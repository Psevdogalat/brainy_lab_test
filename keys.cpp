#include <keys.h>

#define KEY_UNPRESSED	0
#define KEY_PRESSED 	1
#define KEY_TOGGLE_UP	2
#define KEY_TOGGLE_DOWN	3

unsigned int  KEY_MAP		[KEY_MAP_LENGTH] = {0};
unsigned char KEY_STATES	[KEY_MAP_LENGTH] = {0};
unsigned char KEY_TOGGLES	[KEY_MAP_LENGTH] = {0};

void init_key_map(){
	KEY_MAP[KEY_Q ] 			= 81;
	KEY_MAP[KEY_E ] 			= 69;
	KEY_MAP[KEY_W ] 			= 87;
	KEY_MAP[KEY_S ] 			= 83;
	KEY_MAP[KEY_A ] 			= 65;
	KEY_MAP[KEY_D ] 			= 68;
	KEY_MAP[KEY_ARROW_RIGHT ] 	= 39;
	KEY_MAP[KEY_ARROW_LEFT  ] 	= 37;
	KEY_MAP[KEY_ARROW_UP 	]	= 38;
	KEY_MAP[KEY_ARROW_DOWN  ] 	= 40;
	KEY_MAP[KEY_SHIFT  		] 	= 16;
	KEY_MAP[KEY_SPACE  		] 	= 32;	
	KEY_MAP[KEY_C  			] 	= 67;
	KEY_MAP[KEY_X  			] 	= 88;
	KEY_MAP[KEY_Z			]   = 90;
	KEY_MAP[KEY_ESC			]   = 27;
	KEY_MAP[KEY_B			]   = 66;
	
}

unsigned int find_key_by_code(unsigned int Code){
	for(unsigned int key = 0; key < KEY_MAP_LENGTH; key++)
		if(KEY_MAP[key] == Code)
			return key;
	
	return INVALID_KEY;
}

bool key_state(unsigned int Key){
	if(Key < KEY_MAP_LENGTH){
		switch(KEY_STATES[Key]){
			case KEY_TOGGLE_DOWN:
				KEY_STATES[Key] = KEY_PRESSED;
			case KEY_PRESSED: 
				return true; 
				
				
			case KEY_TOGGLE_UP:
				KEY_STATES[Key] = KEY_UNPRESSED;
			case KEY_UNPRESSED:
				return false;
		}
	}
	return false;
}

bool key_front_up(unsigned int Key){
	if(Key < KEY_MAP_LENGTH){
		switch(KEY_STATES[Key]){
			case KEY_TOGGLE_UP:
				KEY_STATES[Key] = KEY_UNPRESSED; 
				return true;
				
			case KEY_TOGGLE_DOWN:
				KEY_STATES[Key] = KEY_PRESSED; 
				return false; 
		}
	}
	return false;
}

bool key_front_down(unsigned int Key){
	if(Key < KEY_MAP_LENGTH){
		switch(KEY_STATES[Key]){
			case KEY_TOGGLE_UP:
				KEY_STATES[Key] = KEY_UNPRESSED; 
				return false;
				
			case KEY_TOGGLE_DOWN:
				KEY_STATES[Key] = KEY_PRESSED; 
				return true; 
		}
	}
	return false;
}

void toggle_key_down(unsigned int Key){
	if(Key < KEY_MAP_LENGTH){
		if(KEY_STATES[Key] != KEY_PRESSED)
			KEY_STATES[Key] = KEY_TOGGLE_DOWN;
	}
}
void toggle_key_up(unsigned int Key){
	if(Key < KEY_MAP_LENGTH){
		if(KEY_STATES[Key] != KEY_UNPRESSED)
			KEY_STATES[Key] = KEY_TOGGLE_UP;
	}
}
	