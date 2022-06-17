#include <windows.h>

#include <gl/gl.h>
#include <engine.h>
#include <std_defines.h>

#include <cstdio>

static const UINT 		main_window_width  	= 640;
static const UINT 		main_window_height 	= 420;
static const UINT 		aver_frames 		= 10;	
static const double 	frame_lock 			= 30.0;
static const LPCSTR 	main_window_class_name 	= "main_window_class";
static const LPCSTR 	main_window_header	  	= "brainy lab test";

static HWND 			main_window;
static HGLRC 			render_context;
static UINT_PTR			frame_timer;
static DWORD			frame_sys_tick_last;
static DWORD			frame_sys_tick;
static double			frame_time;
static double 			aver_frame_time = 0;
static UINT				aver_cnt 		= 0;
static double			frame_counter;

LRESULT CALLBACK main_window_procedure(HWND, UINT, WPARAM, LPARAM);
void dispatch_engine_message(const MSG* );

UINT wm_paint_counter = 0;

void GAME_ENGINE::platform_terminate(){
	PostQuitMessage(0);
	
}

int APIENTRY WinMain(HINSTANCE Instance, HINSTANCE Prev_instance, LPSTR Cmd_line, int Cmd_show){
	
	HDC	 		main_window_dc;	
	WNDCLASSEX	wcex;
	UINT 		pixel_format_index;
	PIXELFORMATDESCRIPTOR pfd;
	RECT		area_size_rect;
	DWORD		window_style;
	
	GAME_ENGINE::platform_init((const char*) Cmd_line);	
	
	ZeroMemory(&wcex,sizeof(WNDCLASSEX));
	wcex.cbSize 		= sizeof(WNDCLASSEX);
	wcex.lpszClassName	= main_window_class_name;
	wcex.hInstance		= Instance;
	wcex.style			= CS_OWNDC;
	wcex.lpfnWndProc	= main_window_procedure;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	
	if(!RegisterClassEx(&wcex)){
		printf("Window class ""%s"" registration error!\n",main_window_class_name);
		return 1;
	}
	
	window_style = WS_CAPTION | WS_SYSMENU;
	ZeroMemory(&area_size_rect,sizeof(RECT));
	area_size_rect.right	= main_window_width;
	area_size_rect.bottom	= main_window_height;
	
	AdjustWindowRect(&area_size_rect,window_style,false);
	
	/*
	printf("Adjusted size %d %d %d %d \n", area_size_rect.left, area_size_rect.top, 
		area_size_rect.right, area_size_rect.bottom);
	*/
	
	area_size_rect.bottom 	-= (area_size_rect.top);
	area_size_rect.right	-= area_size_rect.left;
	
	main_window = CreateWindowEx(0, main_window_class_name, main_window_header,
		window_style,
		//WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		area_size_rect.right, area_size_rect.bottom, NULL, NULL, Instance, NULL);
	if(main_window == INVALID_HANDLE_VALUE){
		printf("Instance of window class ""%s"" creation error!\n",main_window_class_name);
		return 1;
	}
	
	main_window_dc = GetDC(main_window);

	ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize 		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iLayerType	= PFD_MAIN_PLANE;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= 24;
	pfd.cDepthBits 	= 16;
	
	pixel_format_index = ChoosePixelFormat(main_window_dc, &pfd);
	if(pixel_format_index != 0)
		SetPixelFormat(main_window_dc, pixel_format_index, &pfd);
	
	printf("OPENGL_VERSION: %s\n",glGetString(GL_VERSION));
	//printf("GetLastError %x\n",glGetError());
	
	render_context = wglCreateContext(main_window_dc);
	
	glViewport(0, 0, 
		main_window_width, main_window_height);
		
	ReleaseDC(main_window,main_window_dc);
	
	//printf("Timer val %d\n", (UINT)(1000.0/frame_lock));
	
	frame_timer = SetTimer(main_window,0,((UINT)(1000.0/frame_lock))-10,nullptr);
	frame_sys_tick_last = GetTickCount();
	
	ShowWindow(main_window,SW_NORMAL);
	
	MSG message;
	while(GetMessage(&message, NULL, 0, 0)){
		TranslateMessage(&message);
		DispatchMessage(&message);
		dispatch_engine_message(&message);
	}
	
	printf("App closing ... \n");
	GAME_ENGINE::platform_free();
	KillTimer(main_window,frame_timer);
	wglDeleteContext(render_context);
	
	
	return 0;
}

LRESULT CALLBACK main_window_procedure(HWND Window, UINT Message, WPARAM W_param, LPARAM L_param){
	PAINTSTRUCT paint_struct;
	LPSTR		window_title;
	static const UINT window_title_max_len = 50;
	
	switch(Message){
		case WM_CLOSE:
			GAME_ENGINE::platform_terminate();
			
		case WM_PAINT:
			
			/*
			wm_paint_counter++;
			printf("WM_MESSAGE %d\n",wm_paint_counter);
			*/
			
			if(render_context != INVALID_HANDLE_VALUE){
				BeginPaint(Window,&paint_struct);
				wglMakeCurrent(paint_struct.hdc, render_context);
			
				GAME_ENGINE::platform_render();
				
				SwapBuffers(paint_struct.hdc);
				wglMakeCurrent(NULL, NULL);
				EndPaint(Window,&paint_struct);
				
				frame_sys_tick = GetTickCount();
				if(frame_sys_tick_last < frame_sys_tick){
					frame_time 		= 0.001 * (double)(frame_sys_tick - frame_sys_tick_last);
				}
				frame_sys_tick_last = frame_sys_tick;
				
				if(aver_cnt < aver_frames){
					aver_frame_time += frame_time;
					aver_cnt++;
				}else{
					aver_frame_time /= aver_frames; 
					frame_counter 	= (1.0f)/aver_frame_time;
					aver_frame_time = 0;
					aver_cnt 		= 0;
				}
				
				window_title = (LPSTR)malloc(sizeof(char)*(window_title_max_len+1));
				sprintf(window_title, "%s :: fps %2.2f", main_window_header, frame_counter);
				SetWindowText(Window, window_title);
				free(window_title);
			}
		break;
		case  WM_TIMER:
			GAME_ENGINE::platform_compute(frame_time);
			InvalidateRect(Window, nullptr, false);
		break;
		default:
			return DefWindowProc(Window, Message, W_param, L_param);
	}
	return 0;
}

double transform_mouse_x(unsigned int Mouse_x){
	return (2.0 * (double)Mouse_x / (double)main_window_width) - 1.0;
}

double transform_mouse_y(unsigned int Mouse_y){
	return (2.0 * (double)(main_window_height - Mouse_y) / (double)main_window_height) - 1.0;
}

void dispatch_engine_message(const MSG* Win_message){
		GAME_ENGINE::EVENT_MSG engine_event;
		ZeroMemory(&engine_event,sizeof(GAME_ENGINE::EVENT_MSG));
		GAME_ENGINE::EVENT_KEY_RECORD& 		key   = engine_event.event_record.key;
		GAME_ENGINE::EVENT_MOUSE_RECORD& 	mouse = engine_event.event_record.mouse;
		
		switch(Win_message->message){
			case WM_KEYDOWN:
			case WM_KEYUP:
				engine_event.event = EV_KEY_PRESS;
				if(Win_message->message == WM_KEYDOWN)	
					key.key_down = true;
				
				key.key_code = Win_message->wParam; 
				GAME_ENGINE::platform_event(engine_event);
			break;
			case WM_MOUSEWHEEL:
				engine_event.event = EV_MOUSE_WHEEL;
				mouse.delta = GET_WHEEL_DELTA_WPARAM(Win_message->wParam);
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				GAME_ENGINE::platform_event(engine_event);
			break;
			case WM_MOUSEMOVE:
				engine_event.event = EV_MOUSE_MOVE;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				GAME_ENGINE::platform_event(engine_event);
			break;
			case WM_MBUTTONDOWN:
				engine_event.event = EV_MOUSE_MKEY_DOWN;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				GAME_ENGINE::platform_event(engine_event);
			break;
			case WM_MBUTTONUP:
				engine_event.event = EV_MOUSE_MKEY_UP;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				GAME_ENGINE::platform_event(engine_event);
			break;
			case WM_LBUTTONDOWN:
				engine_event.event = EV_MOUSE_LKEY_DOWN;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				GAME_ENGINE::platform_event(engine_event);
			break;
			case WM_LBUTTONUP:
				engine_event.event = EV_MOUSE_LKEY_UP;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				GAME_ENGINE::platform_event(engine_event);
			break;
			case WM_RBUTTONDOWN:
				engine_event.event = EV_MOUSE_RKEY_DOWN;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				GAME_ENGINE::platform_event(engine_event);
			break;
			case WM_RBUTTONUP:
				engine_event.event = EV_MOUSE_RKEY_UP;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				GAME_ENGINE::platform_event(engine_event);
			break;
		}
		
		
}