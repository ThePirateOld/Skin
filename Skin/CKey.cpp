#include "CKey.h"

struct
{
	bool bKey;
	DWORD dwStart;
}KeyBoardState [ 255 ] [ 2 ];

bool IsKeyPressed ( int vKey, bool bUseTime )
{
	if ( GetAsyncKeyState ( vKey ) )
	{
		if ( bUseTime )
		{
			if ( KeyBoardState [ vKey ] [ 0 ].bKey ||
				 KeyBoardState [ vKey ] [ 0 ].dwStart < timeGetTime () )
			{
				KeyBoardState [ vKey ] [ 0 ].bKey = false;
				return true;
			}
		}
		else
		{
			if ( KeyBoardState [ vKey ] [ 0 ].bKey )
			{
				KeyBoardState [ vKey ] [ 0 ].bKey = false;
				return true;
			}
		}
	}
	else
	{
		KeyBoardState [ vKey ] [ 0 ].bKey	 = true;
		KeyBoardState [ vKey ] [ 0 ].dwStart = ( DWORD ) ( 1.f * 1000 ) + timeGetTime ();
	}

	return false;
}

bool IsKeyJustPressed ( int vKey )
{
	if ( GetAsyncKeyState ( vKey ) )
	{
		KeyBoardState [ vKey ] [ 1 ].bKey = true;
	}
	else
	{
		if ( KeyBoardState [ vKey ] [ 1 ].bKey )
		{
			KeyBoardState [ vKey ] [ 1 ].bKey = false;
			return true;
		}
	}

	return false;
}