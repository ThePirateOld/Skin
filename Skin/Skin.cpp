// Skin.cpp : Defines the exported functions for the DLL application.
//

#include "CMenu.h"

CMenuManager g_Menu;

void SkinInit ()
{

	g_Menu.InitializeDeviceObjs ( NULL );

	g_Menu.AddMenu ( 0 );
	g_Menu.AddMenu ( 1 );
	g_Menu.AddMenu ( 2 );
	g_Menu.AddMenu ( 3 );
	g_Menu.AddMenu ( 4 );
	g_Menu.AddMenu ( 5 );
	g_Menu.AddMenu ( 6 );
	g_Menu.AddMenu ( 7 );
	g_Menu.AddMenu ( 8 );
	g_Menu.AddMenu ( 9 );

	g_Menu.GetMenu ( 0 )->AddColumn ( 0, "Main menu", 200 );

	g_Menu.GetMenu ( 0 )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), "Change Player Clothes" );
	g_Menu.GetMenu ( 0 )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), "New Skins" );
	g_Menu.GetMenu ( 0 )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), "Ped Skins" );
	g_Menu.GetMenu ( 0 )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), "Special Skins" );


}
