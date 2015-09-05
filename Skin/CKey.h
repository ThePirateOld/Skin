#pragma once

#include <Windows.h>
#pragma comment (lib, "winmm.lib")

bool IsKeyPressed ( int vKey, bool bUseTime );
bool IsKeyJustPressed ( int vKey );