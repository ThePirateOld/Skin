#include "CKeyBoard.h"

bool KeyInput::IsKeyDown ( int keyCode )
{
	return s_keys [ keyCode ].down;
}

bool KeyInput::WasKeyPressed ( int keyCode )
{
	return s_keys [ keyCode ].pressed;
}
bool KeyInput::WasKeyReleased ( int keyCode )
{
	return s_keys [ keyCode ].released;
}

void KeyInput::SetKey ( int keyCode, bool isDown, bool isReleased, bool isPressed )
{
	s_keys [ keyCode ].down = isDown;
	s_keys [ keyCode ].released = isReleased;
	s_keys [ keyCode ].pressed = isPressed;
}

void KeyInput::ResetKeys ()
{
	for ( int i = 0; i < 255; i++ )
	{
		s_keys [ i ].released = false;
		s_keys [ i ].pressed = false;
	}
}