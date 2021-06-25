#pragma once

#include "..\misc\vfunc.hpp"

/*
*	If you want to implement our own input system from the wndproc hook then go ahead but this is pretty clean imo.
*/

#define MAX_SPLITSCREEN_CLIENT_BITS 2
// this should == MAX_JOYSTICKS in InputEnums.h
#define MAX_SPLITSCREEN_CLIENTS	( 1 << MAX_SPLITSCREEN_CLIENT_BITS ) // 4

enum {
	MAX_JOYSTICKS = MAX_SPLITSCREEN_CLIENTS,
	MOUSE_BUTTON_COUNT = 5,
};

enum JoystickAxis_t {
	JOY_AXIS_X = 0,
	JOY_AXIS_Y,
	JOY_AXIS_Z,
	JOY_AXIS_R,
	JOY_AXIS_U,
	JOY_AXIS_V,
	MAX_JOYSTICK_AXES,
};

enum {
	JOYSTICK_MAX_BUTTON_COUNT = 32,
	JOYSTICK_POV_BUTTON_COUNT = 4,
	JOYSTICK_AXIS_BUTTON_COUNT = MAX_JOYSTICK_AXES * 2,
};

#define JOYSTICK_BUTTON_INTERNAL( _joystick, _button ) ( JOYSTICK_FIRST_BUTTON + ((_joystick) * JOYSTICK_MAX_BUTTON_COUNT) + (_button) )
#define JOYSTICK_POV_BUTTON_INTERNAL( _joystick, _button ) ( JOYSTICK_FIRST_POV_BUTTON + ((_joystick) * JOYSTICK_POV_BUTTON_COUNT) + (_button) )
#define JOYSTICK_AXIS_BUTTON_INTERNAL( _joystick, _button ) ( JOYSTICK_FIRST_AXIS_BUTTON + ((_joystick) * JOYSTICK_AXIS_BUTTON_COUNT) + (_button) )

#define JOYSTICK_BUTTON( _joystick, _button ) ( (ButtonCode_t)JOYSTICK_BUTTON_INTERNAL( _joystick, _button ) )
#define JOYSTICK_POV_BUTTON( _joystick, _button ) ( (ButtonCode_t)JOYSTICK_POV_BUTTON_INTERNAL( _joystick, _button ) )
#define JOYSTICK_AXIS_BUTTON( _joystick, _button ) ( (ButtonCode_t)JOYSTICK_AXIS_BUTTON_INTERNAL( _joystick, _button ) )


enum ButtonCode_t
{
	KEY_NONE,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,
	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_NONE + 1,
	MOUSE_FIRST = KEY_LAST + 1,
	MOUSE_LEFT = MOUSE_FIRST,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	KEY_MAX
};

enum MouseCodeState_t {
	BUTTON_RELEASED = 0,
	BUTTON_PRESSED,
	BUTTON_DOUBLECLICKED,
};

class IInputSystem {
public:
	void EnableInput(bool bEnable) {
		typedef void(__thiscall* OriginalFn)(void*, bool);
		return call_virtual<OriginalFn>(this, 11)(this, bEnable);
	}

	void ResetInputState() {
		typedef void(__thiscall* OriginalFn)(void*);
		return call_virtual<OriginalFn>(this, 39)(this);
	}

	bool IsButtonDown(ButtonCode_t code) {
		typedef bool(__thiscall* OriginalFn)(void*, ButtonCode_t);
		return call_virtual<OriginalFn>(this, 15)(this, code);
	}

	void GetCursorPosition(int* m_pX, int* m_pY) {
		typedef void(__thiscall* OriginalFn)(void*, int*, int*);
		return call_virtual<OriginalFn>(this, 56)(this, m_pX, m_pY);
	}

	const char* ButtonCodeToString(ButtonCode_t ButtonCode) {
		typedef const char*(__thiscall* OriginalFn)(void*, ButtonCode_t);
		return call_virtual<OriginalFn>(this, 40)(this, ButtonCode);
	}

	ButtonCode_t VirtualKeyToButtonCode(int nVirtualKey) {
		typedef ButtonCode_t(__thiscall* OriginalFn)(void*, int);
		return call_virtual<OriginalFn>(this, 45)(this, nVirtualKey);
	}

	int ButtonCodeToVirtualKey(ButtonCode_t code) {
		typedef int(__thiscall* OriginalFn)(void*, ButtonCode_t);
		return call_virtual<OriginalFn>(this, 46)(this, code);
	}
};