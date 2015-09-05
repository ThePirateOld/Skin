#include "SKin.h"
#include "CSkinsName.h"
#include "Clothes.h"

#include "CMenu.h"
#include "CKey.h"

#include "injector\utility.hpp"
#include "internal\CallbackResetDevice.hpp"

static CallbackResetDevice ResetDevice;
static CallbackManager1C<0x53E4FF> DrawHUDFunc;
static CallbackManager1C<0x46937B> InitialiseTheScriptsFunc;
static CallbackManager1C<0x53BC21> TerminateGame;
static CallbackManager1C<0x748CFB> InitGameFunc;
static CallbackManager2C<0x748E09, 0x748E48> ReInitGameFunc;

void RegisterFuncs ( void )
{
	InitGameFunc.RegisterFuncAfter ( SkinInit );
	//ReInitGameFunc.RegisterFuncAfter ( ReInit );
	DrawHUDFunc.RegisterFuncAfter ( Draw );
	ResetDevice.RegisterFuncBefore ( OnLostDevice );
	ResetDevice.RegisterFuncAfter ( OnResetDevice );
	//TerminateGame.RegisterFuncAfter ( Remove );
}

CMenuManager *g_pMenuManager = NULL;

#define MENU_ID_MAIN_MENU 0

#define MENU_ID_NEW_SKINS 1
#define MENU_ID_PED_SKINS 2
#define MENU_ID_SPECIAL_SKINS 3
#define MENU_ID_CHANGE_CLOTHES 4

#define MENU_ID_CLOTHES_TORSO 5
#define MENU_ID_CLOTHES_HAIR 6
#define MENU_ID_CLOTHES_LEGS 7
#define MENU_ID_CLOTHES_SHOES 8
#define MENU_ID_CLOTHES_LEFT_LOWER_ARM 9
#define MENU_ID_CLOTHES_LEFT_UPPER_ARM 10
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

#define MENU_ID_CHANGE_STAT 23

WNDPROC gameProc;
LRESULT DefWndProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( g_pMenuManager )
		g_pMenuManager->HandleMessage ( uMsg, wParam, lParam );

	return CallWindowProc ( gameProc, hWnd, uMsg, wParam, lParam );
}

void CheckLimit ( int iMin, int iMax, int &iValue )
{
	if ( IsKeyPressed ( VK_RIGHT, true ) )
		iValue++;
	else if ( IsKeyPressed ( VK_LEFT, true ) )
		iValue--;

	if ( iValue <= iMin )
	{
		iValue = iMin;
	}
	else if ( iValue >= iMax )
	{
		iValue = iMax;
	}
}
#define setModelIndex  0x5E4880


void RequestSpecialModel ( int channel, const char *szModel, int index )
{
	DWORD dwFunc = 0x00409D10; // RequestSpecialModel
	__asm
	{
		push channel
		push szModel
		push index
		call dwFunc
		add  esp, 0xC

	};
}

void RequestAllModels (bool bOnlyForPriority)
{
	DWORD dwFunc = 0x0040EA10; // load all requested models
	DWORD dwPriority = bOnlyForPriority;
	__asm
	{
		push   dwPriority
		call   dwFunc
		add    esp, 4
	}
}



namespace CallbackHandlers
{
	void MenuNewSkins ( CMenu *pMenu, int iRow )
	{
	}

	void MenuPedSkins ( CMenu *pMenu, int iRow )
	{

	}

	void MenuSpecialSkins ( CMenu *pMenu, int iRow )
	{

	}

	void MenuClothes ( CMenu *pMenu, int iRow )
	{

	}

	void MenuStat ( CMenu *pMenu, int iRow )
	{
		static int iFat = 0, iMuscle = 0;

		if ( iRow == 0 )
		{
			CheckLimit ( 0, 1000, iMuscle );
		}
		else if ( iRow == 1 )
		{
			CheckLimit ( 0, 1000, iFat );
		}

		pMenu->SetNewItem ( 1, 0, "%i", iMuscle );
		pMenu->SetNewItem ( 1, 1, "%i", iFat );
	}
};

void SkinInit ()
{
	g_pMenuManager = new CMenuManager ();

	if ( !g_pMenuManager )
		return;

	if ( FAILED ( g_pMenuManager->InitializeDeviceObjs ( *( IDirect3DDevice9** ) 0x00C97C28 ) ) )
		return;

	for ( size_t i = 0; i < 24; i++ )
	{
		g_pMenuManager->AddMenu ( i );

		// Check for a valid menu
		if ( !g_pMenuManager->GetMenu ( i ) )
			return;
	}

	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->SetPos ( D3DXVECTOR2 ( 200.f, 200.f ) );
	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->SetHeight ( 20 );

	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddColumn ( 0, "Main menu", 200 );

	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, "New Skins" );
	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, "Ped Skins" );
	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, "Special Skins" );
	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, "Change Player Clothes" );
	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, "Change Stat" );

	// Add Submenus
	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddSubMenu ( g_pMenuManager->GetMenu ( MENU_ID_NEW_SKINS ), 0 );
	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddSubMenu ( g_pMenuManager->GetMenu ( MENU_ID_PED_SKINS ), 1 );
	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddSubMenu ( g_pMenuManager->GetMenu ( MENU_ID_SPECIAL_SKINS ), 2 );
	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddSubMenu ( g_pMenuManager->GetMenu ( MENU_ID_CHANGE_CLOTHES ), 3 );
	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddSubMenu ( g_pMenuManager->GetMenu ( MENU_ID_CHANGE_STAT ), 4 );

	// Add Callback's
	g_pMenuManager->GetMenu ( MENU_ID_NEW_SKINS )->SetEventHandler ( CallbackHandlers::MenuNewSkins );
	g_pMenuManager->GetMenu ( MENU_ID_PED_SKINS )->SetEventHandler ( CallbackHandlers::MenuPedSkins );
	g_pMenuManager->GetMenu ( MENU_ID_SPECIAL_SKINS )->SetEventHandler ( CallbackHandlers::MenuSpecialSkins );

	for ( size_t i = MENU_ID_CLOTHES_TORSO; i < MENU_ID_CLOTHES_SPECIAL + 1; i++ )
		g_pMenuManager->GetMenu ( i )->SetEventHandler ( CallbackHandlers::MenuClothes );

	g_pMenuManager->GetMenu ( MENU_ID_CHANGE_STAT )->SetEventHandler ( CallbackHandlers::MenuStat );

	//----------- New Skins
	g_pMenuManager->GetMenu ( MENU_ID_NEW_SKINS )->AddColumn ( 0, "New Skins", 200 );

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA ( "SkinsAddon/*.dff", &FindFileData );
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
			{
				auto FileName = FindFileData.cFileName;

				// Remove '.dff' from string
				ZeroMemory ( FileName + strlen ( FileName ) - 4, 4 );
				g_pMenuManager->GetMenu ( MENU_ID_NEW_SKINS )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, FileName );

			}
		}
		while ( FindNextFileA ( hFind, &FindFileData ) );
		FindClose ( hFind );
	}
	//-----------

	//-----------  Ped Skins
	g_pMenuManager->GetMenu ( MENU_ID_PED_SKINS )->AddColumn ( 0, "Ped Skins", 200 );
	g_pMenuManager->GetMenu ( MENU_ID_PED_SKINS )->AddColumn ( 1, "ID", 50 );

	for ( size_t i = 0; i < PED_MODEL_MAX; i++ )
	{
		g_pMenuManager->GetMenu ( MENU_ID_PED_SKINS )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, sPedModel [ i ].szPedName );
		g_pMenuManager->GetMenu ( MENU_ID_PED_SKINS )->AddColumnItem ( 1, D3DCOLOR_RGBA ( 255, 0, 0, 255 ), false, "%i", sPedModel [ i ].uiPedID );
	}
	//-----------

	//-----------  Special Skins
	g_pMenuManager->GetMenu ( MENU_ID_SPECIAL_SKINS )->AddColumn ( 0, "Special Skins", 200 );
	g_pMenuManager->GetMenu ( MENU_ID_SPECIAL_SKINS )->AddColumn ( 1, "Model", 100 );

	for ( size_t i = 0; i < SPECIAL_MODEL_MAX; i++ )
	{
		g_pMenuManager->GetMenu ( MENU_ID_SPECIAL_SKINS )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, sSpecialModel [ i ].szName );
		g_pMenuManager->GetMenu ( MENU_ID_SPECIAL_SKINS )->AddColumnItem ( 1, D3DCOLOR_RGBA ( 255, 0, 0, 255 ), false, sSpecialModel [ i ].szModel );
	}
	//-----------

	//----------- Clothes
	g_pMenuManager->GetMenu ( MENU_ID_CHANGE_CLOTHES )->AddColumn ( 0, "Choose bodypart type", 200 );

	for ( size_t i = 0; i < PLAYER_CLOTHING_SLOTS; i++ )
	{
		int iMenuClothesID = i + MENU_ID_CLOTHES_TORSO;

		g_pMenuManager->GetMenu ( MENU_ID_CHANGE_CLOTHES )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, sClothesNames [ i ].szName );
		g_pMenuManager->GetMenu ( iMenuClothesID )->AddColumn ( 0, sClothesNames [ i ].szName, 200 );

		const SPlayerClothing *ClothesInfo = CClothes::GetClothingGroup ( i );
		for ( size_t j = 0; j < CClothes::GetClothingGroupMax ( i ); j++ )
			g_pMenuManager->GetMenu ( iMenuClothesID )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, ClothesInfo [ j ].szTexture );

		g_pMenuManager->GetMenu ( MENU_ID_CHANGE_CLOTHES )->AddSubMenu ( g_pMenuManager->GetMenu ( iMenuClothesID ), i );

	}
	//-----------

	//----------- Stat
	g_pMenuManager->GetMenu ( MENU_ID_CHANGE_STAT )->AddColumn ( 0, "Stat", 200 );
	g_pMenuManager->GetMenu ( MENU_ID_CHANGE_STAT )->AddColumn ( 1, "Value", 200 );

	g_pMenuManager->GetMenu ( MENU_ID_CHANGE_STAT )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, "Fat" );
	g_pMenuManager->GetMenu ( MENU_ID_CHANGE_STAT )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, "Muscle" );

	g_pMenuManager->GetMenu ( MENU_ID_CHANGE_STAT )->AddColumnItem ( 1, D3DCOLOR_RGBA ( 255, 0, 0, 255 ), true, " " );
	g_pMenuManager->GetMenu ( MENU_ID_CHANGE_STAT )->AddColumnItem ( 1, D3DCOLOR_RGBA ( 255, 0, 0, 255 ), true, " " );

	gameProc = ( WNDPROC ) SetWindowLong ( *( HWND* ) 0x00C8CF88, GWL_WNDPROC, ( LONG ) DefWndProc );

}


void Draw ()
{
	if ( g_pMenuManager )
		g_pMenuManager->Draw ();
}

void OnLostDevice ()
{
	if ( g_pMenuManager )
		g_pMenuManager->OnLostDevice ();
}

void OnResetDevice ()
{
	if ( g_pMenuManager )
		g_pMenuManager->OnResetDevice ();
}
