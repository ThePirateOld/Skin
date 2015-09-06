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

void RequestSpecialModel ( DWORD dwModelID, const char *szModel, DWORD dwChannel )
{
	if ( dwModelID < 0 )
		return;

	__asm push	dwChannel
	__asm push	szModel
	__asm push	dwModelID
	__asm mov	eax, 00409D10h
	__asm call  eax
	__asm add	esp, 0xC
}

bool HasSpecialModelLoaded ( DWORD dwModelID )
{
	if ( dwModelID < 0 )
		return false;

	BOOL bReturn = 0;

	__asm push   dwModelID
	__asm mov	 eax, 00407F00h
	__asm call   eax
	__asm movzx  eax, al
	__asm mov    bReturn, eax
	__asm pop    eax

	return bReturn;
}

void ReleaseSpecialModel ( DWORD dwModelID )
{
	if ( dwModelID < 0 )
		return;

	__asm push dwModelID
	__asm mov eax, 00409C90h
	__asm call eax
	__asm add esp, 0x4
}

bool HasModelLoaded ( DWORD dwModelID )
{
	if ( dwModelID < 0 )
		return false;

	BOOL bReturn = 0;

	__asm push   dwModelID
	__asm mov	 eax, 004044C0h
	__asm call   eax
	__asm movzx  eax, al
	__asm mov    bReturn, eax
	__asm pop    eax

	return bReturn;
}

void RequestModel ( DWORD dwModelID )
{
	if ( dwModelID < 0 )
		return;

	__asm push  0
	__asm push  dwModelID
	__asm mov	eax, 004087E0h
	__asm call  eax
	__asm add   esp, 0x8
}

void ReleaseModel ( DWORD dwModelID )
{
	if ( dwModelID < 0 )
		return;

	__asm push dwModelID
	__asm mov eax, 004089A0h
	__asm call eax
	__asm add esp, 0x4
}

void RequestAllModels ( void )
{
	__asm push  0
	__asm mov	eax, 0040EA10h
	__asm call  eax
	__asm add   esp, 0x4
}

void SetModelIndex ( DWORD dwModelID )
{
	if ( dwModelID < 0 )
		return;

	__asm mov	edi, 00B7CD98h
	__asm mov	ecx, [ edi ]
	__asm push  dwModelID
	__asm mov	eax, 005E4880h
	__asm call	eax
}

void SetClothes ( const char* szTexture, const char* szModel, int textureType )
{
	DWORD dwFunc = 0x5A8080;

	_asm
	{
		mov	edi, 00B7CD98h
		mov	ecx, [ edi + 0x4 + 0x4 ]
		push    textureType
		push    szModel
		push    szTexture
		call    dwFunc
	}
}

void RebuildChar ( void )
{
	DWORD dwFunc = 0x5A82C0;
	DWORD dwThis = 0xB7CD98;

	_asm
	{
		push    0
		mov	edi, 00B7CD98h
		mov esi, [ edi ]
		push    esi
		call    dwFunc
		add     esp, 8
	}
}

namespace CallbackHandlers
{
	void MenuNewSkins ( CMenu *pMenu, int iRow )
	{

	}

	void MenuPedSkins ( CMenu *pMenu, int iRow )
	{
		int iModel = sPedModel [ iRow ]. uiPedID;

		if ( pMenu->OnKeyPressed ( iRow ) )
		{
			if ( iModel != NULL )
			{
				if ( !HasModelLoaded ( iModel ) )

				{
					RequestModel ( iModel );
					RequestAllModels ();

				}
				SetModelIndex ( iModel );
				ReleaseModel ( iModel );
			}
			else
			{
				SetModelIndex ( iModel );
			}
		}
	}

	void MenuSpecialSkins ( CMenu *pMenu, int iRow )
	{
		if ( pMenu->OnKeyPressed ( iRow ) )
		{
			if ( !HasSpecialModelLoaded ( 290 ) )
			{
				RequestSpecialModel ( 290, pMenu->GetSelectedRowByName ( 1 ), 0 );
				RequestAllModels ();
			}

			SetModelIndex ( 290 );
			ReleaseSpecialModel ( 290 );
		}
	}

	void MenuClothes ( CMenu *pMenu, int iRow )
	{
		if ( pMenu->OnKeyPressed ( iRow ) )
		{
			const SPlayerClothing *ClothesInfo = CClothes::GetClothingGroupByName ( pMenu->GetColumnName ( 0 ) );
			SetClothes ( ClothesInfo [ iRow ].szTexture, ClothesInfo [ iRow ].szModel, ClothesInfo [ iRow ].uiBodyPart );
			RebuildChar ();
		}
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

	g_pMenuManager->GetMenu ( MENU_ID_MAIN_MENU )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, "New Skins");
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

		g_pMenuManager->GetMenu ( MENU_ID_CHANGE_CLOTHES )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, sClothingType [ i ].szName );
		g_pMenuManager->GetMenu ( iMenuClothesID )->AddColumn ( 0, sClothingType [ i ].szName, 200 );

		const SPlayerClothing *ClothesInfo = CClothes::GetClothingGroup ( i );
		for ( size_t j = 0; j < CClothes::GetClothingGroupMax ( i ); j++ )
			g_pMenuManager->GetMenu ( iMenuClothesID )->AddColumnItem ( 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), false, ClothesInfo [ j ].szName );

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

	{
		g_pMenuManager->Draw ();
	}
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
