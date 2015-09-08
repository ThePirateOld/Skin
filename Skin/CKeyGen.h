#pragma once

#include <Windows.h>

#define FUNC_CKeyGen_GetKey_len         0x53ced0
#define FUNC_CKeyGen_GetKey             0x53cf00
#define FUNC_CKeyGen_GetUppercaseKey    0x53cf30
#define FUNC_CKeyGen_AppendStringToKey  0x53cf70

class CKeyGen
{
public:
	static unsigned int GetKey ( char const *szString );
	static unsigned int GetKey ( char const *szString, int uiLength );
	static unsigned int GetUppercaseKey ( char const *szString );
	static unsigned int AppendStringToKey ( unsigned int uiKey, char const *szString );
}; 
