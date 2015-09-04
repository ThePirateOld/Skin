#pragma once

struct Keys
{
	bool down;
	bool released;
	bool pressed;
};

class KeyInput
{
public:
	 bool IsKeyDown ( int keyCode );
	 bool WasKeyPressed ( int keyCode );
	 bool WasKeyReleased ( int keyCode );

	 void SetKey ( int keyCode, bool isDown, bool isReleased, bool isPressed );
	 void ResetKeys ();
private:
	Keys s_keys [ 255 ];
};