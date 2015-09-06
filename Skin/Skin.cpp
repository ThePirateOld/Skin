#include "SKin.h"
#include "CSkinsName.h"
#include "Clothes.h"

#include "CMenu.h"
#include "CKey.h"

#include "injector\utility.hpp"
#include "internal\CallbackResetDevice.hpp"
#include "injector\calling.hpp"

static CallbackResetDevice ResetDevice;
static CallbackManager1C<0x53E4FF> DrawHUDFunc;
static CallbackManager1C<0x46937B> InitialiseTheScriptsFunc;
static CallbackManager1C<0x53BC21> TerminateGame;
static CallbackManager1C<0x748CFB> InitGameFunc;
static CallbackManager2C<0x748E09, 0x748E48> ReInitGameFunc;



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

BOOL HasModelLoaded ( int iModelID )
{
	DWORD dwFunc = 0x4044C0;
	DWORD ModelID = iModelID;
	BOOL bReturn = 0;

	_asm
	{
		push    ModelID
		call    dwFunc
		movzx   eax, al
		mov     bReturn, eax
		pop     eax
	}

	return bReturn;
}

void RequestModel ( int iModelID,int iLoadingStream )
{
	_asm push iLoadingStream
	_asm push iModelID
	_asm mov edx, 0x4087E0
	_asm call edx
	_asm pop edx
	_asm pop edx
}

void ReleaseModel ( int iModelID )
{
	_asm push iModelID
	_asm mov edx, 0x4089A0
	_asm call edx
	_asm pop edx
}

void RequestAllModels ( bool bOnlyPriorityModels )
{
	DWORD dwOnlyPriorityModels = bOnlyPriorityModels;
	__asm push  dwOnlyPriorityModels;
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
		mov	ecx, [ edi + 0x4 + 0x4 ] // (CPed) + (CPlayerData) + (CClothes)
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

#define HOOKPOS_CClothes_RebuildPlayer  0x5A82C0
DWORD RETURN_CClothes_RebuildPlayera = 0x5A82C8;
DWORD RETURN_CClothes_RebuildPlayerb = 0x5A837F;

// Only allow rebuild player on CJ - Stops other models getting corrupted (spider CJ)
// hooked at 5A82C0 8 bytes
void _declspec( naked ) HOOK_CClothes_RebuildPlayer ()
{
	_asm
	{
		push    esi
		mov     esi, [ esp + 8 ]
		movsx   eax, word ptr [ esi + 34 ]
		cmp     eax, 0
		jne     cont  // Not CJ, so skip

		// continue standard path
		mov     eax, [ esi + 18h ]
		jmp     RETURN_CClothes_RebuildPlayera  // 005A82C8

		cont:
		jmp     RETURN_CClothes_RebuildPlayerb  // 005A837F
	}
}


template < class T, class U >
void MemPut ( U ptr, const T value )
{
	if ( *( T* ) ptr != value )
		memcpy ( ( void* ) ptr, &value, sizeof ( T ) );
}
////////////////////////////////////////////////////////////////////

BYTE * CreateJump ( DWORD dwFrom, DWORD dwTo, BYTE * ByteArray )
{
	ByteArray [ 0 ] = 0xE9;
	MemPut < DWORD > ( &ByteArray [ 1 ], dwTo - ( dwFrom + 5 ) );
	return ByteArray;
}

#define MAX_JUMPCODE_SIZE 50
BOOL HookInstall ( DWORD dwInstallAddress,
				   DWORD dwHookHandler,
				   int iJmpCodeSize )
{
	BYTE JumpBytes [ MAX_JUMPCODE_SIZE ];
	memset ( JumpBytes, 0x90, MAX_JUMPCODE_SIZE );
	if ( CreateJump ( dwInstallAddress, dwHookHandler, JumpBytes ) )
	{

		memcpy ( ( PVOID ) dwInstallAddress, JumpBytes, iJmpCodeSize );

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//
// Skip loading the directory data from player.img if it has already been loaded.
// Speeds up clothes a bit, but is only part of a solution - The actual files from inside player.img are still loaded each time
//
bool _cdecl IsPlayerImgDirLoaded ( void )
{
	// When player.img dir is loaded, it looks this this:
	// 0x00BC12C0  00bbcdc8 00000226
	DWORD* ptr1 = ( DWORD* ) 0x00BC12C0;
	if ( ptr1 [ 0 ] == 0x00BBCDC8 && 
		 ptr1 [ 1 ] == 0x0000226 )
	{
		return true;
	}

	return false;
}

// Hook info
#define HOOKPOS_LoadingPlayerImgDir                     0x5A69E3
#define HOOKSIZE_LoadingPlayerImgDir                    5
DWORD RETURN_LoadingPlayerImgDirA = 0x5A69E8;
DWORD RETURN_LoadingPlayerImgDirB = 0x5A6A06;
void _declspec( naked ) HOOK_LoadingPlayerImgDir ()
{
	// hook from 005A69E3 5 bytes
	_asm
	{
		pushad
		call    IsPlayerImgDirLoaded
		cmp     al, 0
		jnz     skip
		popad

		// Standard code to load img directory
		push    0BBCDC8h
		jmp     RETURN_LoadingPlayerImgDirA

		// Skip loading img directory
		skip :
		popad
		jmp     RETURN_LoadingPlayerImgDirB
	}
}


////////////////////////////////////////////////
//
// Hook CStreaming::RequestFile
//
//
//
////////////////////////////////////////////////

	struct CStreamingInfo
	{
		USHORT    usNext;
		USHORT    usPrev;

		USHORT  usNextOnce;         
		UCHAR   ucFlags;         // 0x12 when loading, 0x02 when finished loading
		UCHAR   ucImgId;

		int     iBlockOffset;
		int     iBlockCount;
		UINT    uiLoadflag;         // 0-not loaded  2-requested  3-loaded  1-processed
	};

	int     iReturnFileId;
	char*   pReturnBuffer;



bool FileLoad ( const std::string& strFilename, std::vector < char >& buffer )
{
	buffer.clear ();
	// Open
	FILE* fh = fopen ( strFilename.c_str(), "rb" );
	if ( !fh )
		return false;
	// Get size
	fseek ( fh, 0, SEEK_END );
	int size = ftell ( fh );
	rewind ( fh );

	// Set offset
	fseek ( fh, 0, SEEK_SET );

	int bytesRead = 0;
	if ( size > 0 && size < 1e9 )   // 1GB limit
	{
		size = min ( size, INT_MAX );
		// Allocate space
		buffer.assign ( size, 0 );
		// Read into buffer
		bytesRead = fread ( &buffer.at ( 0 ), 1, size, fh );
	}
	// Close
	fclose ( fh );
	return bytesRead == size;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// If request is for a file inside player.img (imgId 5) and uiLoadflag is 0 or 1
// then force use of our cached player.img data
//
DWORD FUNC_CStreamingInfoAddToList = 0x407480;
DWORD FUNC_CStreamingConvertBufferToObject = 0x40C6B0;

static char *PlayerImgCachePtr = NULL;

bool _cdecl OnCallCStreamingInfoAddToList ( int flags, CStreamingInfo* pImgGTAInfo )
{
	static bool bLoadedFile = false;
	if ( !bLoadedFile )
	{
		std::vector<char> m_PlayerImgCache;

		MEMORYSTATUSEX statex;
		ZeroMemory ( &statex, sizeof ( MEMORYSTATUSEX ) );

		GlobalMemoryStatusEx ( &statex );

		// Check for more than 512 installed ram
		if ( statex.ullTotalPhys > 0 && statex.ullTotalPhys > 512 * 1024 )
		{
			FILE* fh = fopen ( "models/player.img", "rb" );
			if ( !fh )
				return false;
	
			fseek ( fh, 0, SEEK_END );
			int size = ftell ( fh );
			rewind ( fh );
			fseek ( fh, 0, SEEK_SET );

			int bytesRead = 0;
			if ( size > 0 && size < 1e9 ) // 1GB limit
			{
				size = min ( size, INT_MAX );
				// Allocate space
				m_PlayerImgCache.assign ( size, 0 );
				// Read into buffer
				bytesRead = fread ( &m_PlayerImgCache.at ( 0 ), 1, size, fh );
			}
			fclose ( fh );
		}

		// Update the cache pointer
		if ( !m_PlayerImgCache.empty () )
			PlayerImgCachePtr = &m_PlayerImgCache [ 0 ];
		else
			return false;

		bLoadedFile = true;
	}

	if ( pImgGTAInfo->ucImgId == 5 )
	{
		// If bLoadingBigModel is set, try to get it unset
		#define VAR_CStreaming_bLoadingBigModel     0x08E4A58
		BYTE& bLoadingBigModel = *( BYTE* ) VAR_CStreaming_bLoadingBigModel;
		if ( bLoadingBigModel )
		{
			RequestAllModels ( true );

			if ( bLoadingBigModel )
				RequestAllModels ( false );
		}

		int iFileId = ( ( int ) pImgGTAInfo - 0x08E4CC0 ) / 20;

		iReturnFileId = iFileId;
		pReturnBuffer = PlayerImgCachePtr + pImgGTAInfo->iBlockOffset * 2048;

		// Update flags
		pImgGTAInfo->uiLoadflag = 3;

		// Remove priorty flag, as not counted in ms_numPriorityRequests
		pImgGTAInfo->ucFlags &= ~0x10;

		return true;
	}

	return false;
}

// Hook info
#define HOOKPOS_CallCStreamingInfoAddToList             0x408962
#define HOOKSIZE_CallCStreamingInfoAddToList            5
DWORD RETURN_CallCStreamingInfoAddToListA = 0x408967;
DWORD RETURN_CallCStreamingInfoAddToListB = 0x408990;
void _declspec( naked ) HOOK_CallCStreamingInfoAddToList ()
{
	_asm
	{
		pushad
		push    ecx
			push    ebx
			call    OnCallCStreamingInfoAddToList
			add     esp, 4 * 2
			cmp     al, 0
			jnz     skip

			// Continue with standard code
			popad
			call    FUNC_CStreamingInfoAddToList
			jmp     RETURN_CallCStreamingInfoAddToListA


			// Handle load here
			skip :
		popad
			pushad

			mov     eax, 0
			push    eax
			mov     eax, iReturnFileId
			push    eax
			mov     eax, pReturnBuffer
			push    eax
			call    FUNC_CStreamingConvertBufferToObject
			add     esp, 4 * 3

			popad
			add     esp, 4 * 1
			jmp     RETURN_CallCStreamingInfoAddToListB
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Return true to skip
bool _cdecl ShouldSkipLoadRequestedModels ( DWORD calledFrom )
{
	if ( !PlayerImgCachePtr )
		return false;

	// Skip LoadRequestedModels if called from:
	//      CClothesBuilder::ConstructGeometryArray      5A55A0 - 5A56B6
	//      CClothesBuilder::LoadAndPutOnClothes         5A5F70 - 5A6039
	//      CClothesBuilder::ConstructTextures           5A6040 - 5A6520
	if ( calledFrom > 0x5A55A0 && 
		 calledFrom < 0x5A6520 )
		return true;

	return false;
}

// Hook info
#define HOOKPOS_CStreamingLoadRequestedModels  0x15670A0
DWORD RETURN_CStreamingLoadRequestedModels = 0x15670A5;
DWORD RETURN_CStreamingLoadRequestedModelsB = 0x156711B;

void _declspec( naked ) HOOK_CStreamingLoadRequestedModels ()
{
	_asm
	{
		pushad
		push [ esp + 32 + 4 * 0 ]
		call    ShouldSkipLoadRequestedModels
		add     esp, 4 * 1
		cmp     al, 0
		jnz     skip

		// Continue with standard code
		popad
		mov     al, byte ptr ds : [008E4A58h]
		jmp     RETURN_CStreamingLoadRequestedModels

		// Skip LoadRequestedModels
		skip :
		popad
		jmp     RETURN_CStreamingLoadRequestedModelsB
	}
}


#include <stdlib.h>
namespace CallbackHandlers
{
	void MenuNewSkins ( CMenu *pMenu, int iRow )
	{

	}

	void MenuPedSkins ( CMenu *pMenu, int iRow )
	{
		int iModel = sPedModel [ iRow ].uiPedID;

		if ( pMenu->OnKeyPressed ( iRow ) )
		{
			// These models are always loaded
			if ( iModel != 0 && iModel != 7 )
			{
				if ( !HasModelLoaded ( iModel ) )
				{
					RequestModel ( iModel, 0x6 );
					RequestAllModels ( false );
				}

				SetModelIndex ( iModel );
				if ( HasModelLoaded ( iModel ) )
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
				RequestAllModels ( false );
			}

			SetModelIndex ( 290 );
			ReleaseSpecialModel ( 290 );
		}
	}

	void MenuClothes ( CMenu *pMenu, int iRow )
	{
		if ( pMenu->OnKeyPressed ( iRow ) )
		{


			auto ptr = ( DWORD* ) 0xB6F612;
			int *as = ( int * ) ( *ptr + 0x22 );
			
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

void RegisterFuncs ( void )
{
	// Spider CJ fix
	HookInstall ( HOOKPOS_CClothes_RebuildPlayer, ( DWORD ) HOOK_CClothes_RebuildPlayer, 8 );

	HookInstall ( HOOKPOS_CStreamingLoadRequestedModels, ( DWORD ) HOOK_CStreamingLoadRequestedModels, 5 );
	HookInstall ( HOOKPOS_LoadingPlayerImgDir, ( DWORD ) HOOK_LoadingPlayerImgDir, 5 );
	HookInstall ( HOOKPOS_CallCStreamingInfoAddToList, ( DWORD ) HOOK_CallCStreamingInfoAddToList, 5 );
	
	InitGameFunc.RegisterFuncAfter ( SkinInit );
	//ReInitGameFunc.RegisterFuncAfter ( ReInit );
	DrawHUDFunc.RegisterFuncAfter ( Draw );
	ResetDevice.RegisterFuncBefore ( OnLostDevice );
	ResetDevice.RegisterFuncAfter ( OnResetDevice );
	//TerminateGame.RegisterFuncAfter ( Remove );
}

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
		auto ptr = ( DWORD* ) 0x00B6F5F0;
		int *as = ( int * ) ( *ptr + 0x598 );
		*as = 1;

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
