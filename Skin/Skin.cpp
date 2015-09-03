// Skin.cpp : Defines the exported functions for the DLL application.
//

#include "CSkinsName.h"
#include "Clothes.h"

#include "CMenu.h"

CMenuManager g_Menu;

#define MENU_ID_MAIN_MENU 0

#define MENU_ID_NEW_SKINS 1
#define MENU_ID_PED_SKINS 2
#define MENU_ID_SPECIAL_SKINS 3
#define MENU_ID_CHANGE_CLOTHES 4

#define MENU_ID_CLOTHES_TORSO 5
#define MENU_ID_CLOTHES_HAIR 6
#define MENU_ID_CLOTHES_LEGS 7
#define MENU_ID_CLOTHES_SHOES 8
#define MENU_ID_CLOTHES_LEFT_UPPER_ARM 9
#define MENU_ID_CLOTHES_LEFT_LOWER_ARM 10
#define MENU_ID_CLOTHES_RIGHT_UPPER_ARM 11
#define MENU_ID_CLOTHES_RIGHT_LOWER_ARM 12
#define MENU_ID_CLOTHES_BACK_TOP 13
#define MENU_ID_CLOTHES_LEFT_CHEST 14
#define MENU_ID_CLOTHES_RIGHT_CHEST 15
#define MENU_ID_CLOTHES_STOMACH 16
#define MENU_ID_CLOTHES_LOWER_BACK 17
#define MENU_ID_CLOTHES_CHAINS 18
#define MENU_ID_CLOTHES_WATCHES 19
#define MENU_ID_CLOTHES_GLASSES 20
#define MENU_ID_CLOTHES_HATS 21
#define MENU_ID_CLOTHES_SPECIAL 22

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

	// Add sub-menus
	g_Menu.GetMenu ( MENU_ID_MAIN_MENU )->AddSubMenu ( g_Menu.GetMenu ( MENU_ID_CHANGE_CLOTHES ), 0 );
	g_Menu.GetMenu ( MENU_ID_MAIN_MENU )->AddSubMenu ( g_Menu.GetMenu ( MENU_ID_NEW_SKINS ), 1 );
	g_Menu.GetMenu ( MENU_ID_MAIN_MENU )->AddSubMenu ( g_Menu.GetMenu ( MENU_ID_PED_SKINS ), 2 );
	g_Menu.GetMenu ( MENU_ID_MAIN_MENU )->AddSubMenu ( g_Menu.GetMenu ( MENU_ID_SPECIAL_SKINS ), 3 );


	g_Menu.GetMenu ( MENU_ID_MAIN_MENU )->AddColumn ( 0, "Main menu", 200 );

	g_Menu.GetMenu ( MENU_ID_MAIN_MENU )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), "New Skins" );
	g_Menu.GetMenu ( MENU_ID_MAIN_MENU )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), "Ped Skins" );
	g_Menu.GetMenu ( MENU_ID_MAIN_MENU )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), "Special Skins" );
	g_Menu.GetMenu ( MENU_ID_MAIN_MENU )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), "Change Player Clothes" );


	for ( size_t i = 0; i < PLAYER_CLOTHING_SLOTS; i++ )
	{
		g_Menu.GetMenu ( MENU_ID_CHANGE_CLOTHES )->AddColumn ( 0, sClothesNames [ i ].szName, 200 );

		g_Menu.GetMenu ( MENU_ID_MAIN_MENU )->AddSubMenu ( g_Menu.GetMenu ( i + MENU_ID_CLOTHES_TORSO ), i );

		switch ( i + MENU_ID_CLOTHES_TORSO )
		{
			case MENU_ID_CLOTHES_TORSO:
			{
				for ( size_t j = 0; j < TORSO_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sTorsoClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_HAIR:
			{
				for ( size_t j = 0; j < HAIR_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sHairClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_LEGS:
			{
				for ( size_t j = 0; j < LEGS_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sLegsClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_SHOES:
			{
				for ( size_t j = 0; j < SHOES_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sShoesClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_LEFT_UPPER_ARM:
			{
				for ( size_t j = 0; j < LEFT_UPPER_ARM_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sLeftUpperArmClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_LEFT_LOWER_ARM:
			{
				for ( size_t j = 0; j < LEFT_LOWER_ARM_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sLeftLowerArmClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_RIGHT_UPPER_ARM:
			{
				for ( size_t j = 0; j < RIGHT_UPPER_ARM_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sRightUpperArmClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_RIGHT_LOWER_ARM:
			{
				for ( size_t j = 0; j < RIGHT_LOWER_ARM_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sRightLowerArmClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_BACK_TOP:
			{
				for ( size_t j = 0; j < BACK_TOP_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sBackTopClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_LEFT_CHEST:
			{
				for ( size_t j = 0; j < LEFT_CHEST_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sLeftChestClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_RIGHT_CHEST:
			{
				for ( size_t j = 0; j < RIGHT_CHEST_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sRightChestClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_STOMACH:
			{
				for ( size_t j = 0; j < STOMACH_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sStomachClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_LOWER_BACK:
			{
				for ( size_t j = 0; j < LOWER_BACK_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sLowerBackClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_CHAINS:
			{
				for ( size_t j = 0; j < CHAINS_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sChainsClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_WATCHES:
			{
				for ( size_t j = 0; j < WATCHES_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sWatchesClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_GLASSES:
			{
				for ( size_t j = 0; j < GLASSES_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sGlassesClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_HATS:
			{
				for ( size_t j = 0; j < HATS_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sHatsClothing [ j ].szName );
				break;
			}
			case MENU_ID_CLOTHES_SPECIAL:
			{
				for ( size_t j = 0; j < SPECIAL_CLOTHING_MAX; j++ )
					g_Menu.GetMenu ( i )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sSpecialClothing [ j ].szName );
				break;
			}
		}
	}

	g_Menu.GetMenu ( MENU_ID_PED_SKINS )->AddColumn ( 0, "Ped Skins", 200 );
	g_Menu.GetMenu ( MENU_ID_PED_SKINS )->AddColumn ( 1, "ID", 200 );

	for ( size_t i = 0; i < PED_MODEL_MAX; i++ )
	{
		g_Menu.GetMenu ( MENU_ID_PED_SKINS )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sPedModel [ i ].szPedName );
		g_Menu.GetMenu ( MENU_ID_PED_SKINS )->AddColumnItem ( 1, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), "%i", sPedModel [ i ].uiPedID );
	}

	g_Menu.GetMenu ( MENU_ID_SPECIAL_SKINS )->AddColumn ( 0, "Special Skins", 200 );
	g_Menu.GetMenu ( MENU_ID_SPECIAL_SKINS )->AddColumn ( 1, "Model", 200 );

	for ( size_t i = 0; i < SPECIAL_MODEL_MAX; i++ )
	{
		g_Menu.GetMenu ( MENU_ID_SPECIAL_SKINS )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sSpecialModel [ i ].szName );
		g_Menu.GetMenu ( MENU_ID_SPECIAL_SKINS )->AddColumnItem ( 1, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), sSpecialModel [ i ].szModel );
	}
}
