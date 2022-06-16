#ifndef _STD_DEFINES_H_
#define _STD_DEFINES_H_

	typedef unsigned int 	UINT;
	typedef int				INT;
	
	typedef unsigned int 	RETCODE;
	//common return codes
	#define RET_SUCCESS 				0x0000
	#define RET_OBJECT_INVALID 			0x0001
	#define RET_OBJECT_UNDEFINE 		0x0002
	#define RET_ALLOCATION_INVALID		0x0003
	#define RET_IDETIFIER_INVALID		0x0004
	#define RET_IDETIFIER_MULTIPLE		0x0005
	#define RET_UNDEFINE_ERROR			0x00FF
	#define RET_USER					0x0100
	
#endif