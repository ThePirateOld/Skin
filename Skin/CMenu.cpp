#include "CMenu.h"

struct SSubMenuAddon
{
	struct SInvokeMenu
	{
		bool bStat;
		bool bAdded;
	};

	int iCount = 0;

	std::map<CMenu*, SInvokeMenu> InvokeMenu;
	std::map<CMenu*, std::vector<SMenuAddon>> Addon;
	std::map<int, CMenu*> NextMenu;
}SubMenu;


LPD3DXFONT g_pFont = NULL;
LPD3DXSPRITE g_pSprite = NULL;

LPD3DXFONT LoadD3Font ( IDirect3DDevice9 *pDevice )
{
	LPD3DXFONT font;
	D3DXCreateSprite ( pDevice, &g_pSprite );

	D3DXCreateFontA ( pDevice, -MulDiv ( 10, GetDeviceCaps ( GetDC ( 0 ), LOGPIXELSY ), 72 ), 0, FW_BLACK,
					  0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &font );
	return font;
}


 signed char hex_to_dec ( signed char ch )
{
	if ( ch >= '0' && ch <= '9' )
		return ch - '0';
	if ( ch >= 'A' && ch <= 'F' )
		return ch - 'A' + 10;
	if ( ch >= 'a' && ch <= 'f' )
		return ch - 'A' + 10;

	return -1;
}

 bool hex_is_valid ( std::string hex )
 {
	 if ( hex.empty () )
		 return false;
	 for ( size_t i = 0; i < hex.length (); i++ )
	 {
		 if ( hex_to_dec ( hex [ i ] ) == -1 )
			 return false;
	 }
	 return true;
 }

 CONST D3DCOLOR Red = D3DCOLOR_ARGB ( 255, 255, 000, 000 );
 CONST D3DCOLOR Green = D3DCOLOR_ARGB ( 255, 127, 255, 000 );
 CONST D3DCOLOR Orange = D3DCOLOR_ARGB ( 255, 255, 140, 000 );
 CONST D3DCOLOR Blue = D3DCOLOR_ARGB ( 255, 000, 000, 255 );
 CONST D3DCOLOR Yellow = D3DCOLOR_ARGB ( 255, 255, 255, 51 );
 CONST D3DCOLOR Black = D3DCOLOR_ARGB ( 255, 000, 000, 000 );
 CONST D3DCOLOR Grey = D3DCOLOR_ARGB ( 255, 112, 112, 112 );
 CONST D3DCOLOR Gold = D3DCOLOR_ARGB ( 255, 255, 215, 000 );
 CONST D3DCOLOR Pink = D3DCOLOR_ARGB ( 255, 255, 192, 203 );
 CONST D3DCOLOR Purple = D3DCOLOR_ARGB ( 255, 128, 000, 128 );
 CONST D3DCOLOR White = D3DCOLOR_ARGB ( 255, 255, 255, 249 );
 CONST D3DCOLOR Cyan = D3DCOLOR_ARGB ( 255, 000, 255, 255 );
 CONST D3DCOLOR Magenta = D3DCOLOR_ARGB ( 255, 255, 000, 255 );

 struct s
 {
	  char sKey[4];
	  long color;
 }cs [] =
 { {"red",Red }, { "blu",Blue }, { "pur",Purple }, { "whi",White },{ "mag",Magenta }, { "cya",Cyan }, { "ora",Orange }, { "pin",Pink },
 { "gol",Gold }, { "gry",Grey } ,{ "bla",Black }, { "yel",Yellow } , { "grn",Green }
 };


 long GetColor ( const char* key )
 {
	 for ( size_t i = 0; i < sizeof(cs); i++ )
	 {
		 if ( !strncmp ( cs [ i ].sKey, key, 3 ) )
			 return cs [ i ].color;
	 }

	 return 0;
 }

 void DrawFont ( LPD3DXFONT pFont, RECT &r, DWORD dwFormat, D3DCOLOR d3dColor, CHAR *szStr )
 {
	 g_pSprite->Begin ( D3DXSPRITE_ALPHABLEND );
	 g_pSprite->Flush ();

	 DWORD dwColor = d3dColor;

	 int iTagCount = 0;
	 std::map<int, long> color;
	 std::string str ( szStr );
	 RECT rctA = r;
	 long inc = 0;
	 for ( size_t i = 0; i < str.size (); i++ )
	 {
		 if ( str [ i ] == '>' )
		 {
			 if ( str [ i + 4 ] == '<' )
			 {
				 auto pszStr = str.c_str ();
				 color [ iTagCount ] = GetColor ( &pszStr [ i + 1 ] );
				 iTagCount++;
				 str.erase ( i, 5 );
			 }
		 }
		
		// if ( !( dwFormat & DT_CALCRECT ) )
		// {
			 if ( iTagCount )
				 dwColor = color [ iTagCount - 1 ];

			 HDC dc = pFont->GetDC ();

			 SIZE size;
			 GetTextExtentPoint32A ( dc, &str [ i ], 1, &size );

			 auto pszStr = str.c_str ();
			 pFont->DrawTextA ( g_pSprite, &pszStr[i], -1, &r, dwFormat, dwColor );
			 r.left = r.left + size.cx; // offset for next character.

			 if ( inc > rctA.right - rctA.left - 15 )
			 {
				 inc = 0;
				 r.left = rctA.left;
				 r.top += size.cy;
				 r.bottom = r.top + size.cy;
			 }
			 else
				 inc += size.cx;
		// }
	 }

	// if ( dwFormat & DT_CALCRECT )
	//	 pFont->DrawTextA ( NULL, str.c_str (), -1, &r, dwFormat, NULL );

	 g_pSprite->End ();
 }

CMenuManager::CMenuManager ( void ) :
	m_pDevice ( NULL ),
	m_pFont ( NULL ),
	m_pRender ( NULL )
{}

CMenuManager::~CMenuManager ( void )
{
	SAFE_DELETE ( m_pRender );
	SAFE_DELETE ( m_pFont );

	RemoveAllMenus ();
}

HRESULT CMenuManager::InitializeDeviceObjs ( IDirect3DDevice9 *pDevice )
{
	m_pFont = new CD3DFont ( "Tahoma", 10, FW_EXTRABOLD );

	g_pFont = LoadD3Font ( pDevice );

	if ( !m_pFont )
		return E_FAIL;

	if ( FAILED ( m_pFont->Initialize ( pDevice ) ) )
		return E_FAIL;

	m_pRender = new CD3DRender ( 128 );

	if ( !m_pRender )
		return E_FAIL;

	if ( FAILED ( m_pRender->Initialize ( pDevice ) ) )
		return E_FAIL;

	m_pDevice = pDevice;

	return S_OK;
}

CD3DRender *CMenuManager::GetRenderer ( void )
{
	return m_pRender;
}

CD3DFont *CMenuManager::GetFont ( void )
{
	return m_pFont;
}

void CMenuManager::AddMenu ( int ID )
{
	auto pMenu = new CMenu ();
	if ( pMenu )
	{
		pMenu->Initialize ( this );
	}

	SMenuAddon sAddon;
	sAddon.ID = ID;
	sAddon.pMenu = pMenu;
	sAddon.bDraw = true;

	m_MenuAddon.push_back ( sAddon );
}

void CMenuManager::SetDrawable ( int ID, bool bDraw )
{
	for ( size_t i = 0; i < m_MenuAddon.size (); i++ )
	{
		if ( m_MenuAddon [ i ].ID == ID &&
			 !SubMenu.InvokeMenu [ m_MenuAddon [ i ].pMenu ].bAdded )
		{
			m_MenuAddon [ i ].bDraw = bDraw;
			break;
		}
	}
}

void CMenuManager::Draw ( void )
{
	for ( int i = int ( m_MenuAddon.size () ) - 1; i >= 0; i-- )
	{
		if ( m_MenuAddon [ i ].pMenu && 
			 m_MenuAddon [ i ].bDraw )
		{
			m_MenuAddon [ i ].pMenu->Draw ();
		}
	}
}

void CMenuManager::RemoveAllMenus ( void )
{
	for ( size_t i = 0; i < m_MenuAddon.size (); i++ )
		SAFE_DELETE ( m_MenuAddon [ i ].pMenu );

	m_MenuAddon.clear ();
}

void CMenuManager::RemoveMenu ( int ID )
{
	for ( size_t i = 0; i < m_MenuAddon.size (); i++ )
	{
		if ( m_MenuAddon [ i ].pMenu )
		{
			SAFE_DELETE ( m_MenuAddon [ i ].pMenu );
			m_MenuAddon.erase ( m_MenuAddon.begin () + ID );
			break;
		}
	}
}

void CMenuManager::HandleMessage ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	for ( size_t i = 0; i < m_MenuAddon.size (); i++ )
	{
		if ( m_MenuAddon [ i ].pMenu )
		{
			m_MenuAddon [ i ].pMenu->HandleMessage ( uMsg, wParam, lParam );
		}
	}
}

CMenu *CMenuManager::GetMenu ( int ID ) const
{
	for ( size_t i = 0; i < m_MenuAddon.size (); i++ )
	{
		if ( m_MenuAddon [ i ].ID == ID )
		{
			return m_MenuAddon [ i ].pMenu;
		}
	}

	return NULL;
}

void CMenuManager::OnLostDevice ( void )
{
	if ( m_pFont )
		m_pFont->Invalidate ();

	if ( m_pRender )
		m_pRender->Invalidate ();

	if ( g_pFont )
		g_pFont->OnLostDevice ();

	if ( g_pSprite )
		g_pSprite->OnLostDevice ();
}

void CMenuManager::OnResetDevice ( void )
{
	if ( m_pFont )
		m_pFont->Initialize ( m_pDevice );

	if ( m_pRender )
		m_pRender->Initialize ( m_pDevice );

	if ( g_pFont )
		g_pFont->OnResetDevice ();

	if ( g_pSprite )
		g_pSprite->OnResetDevice ();
}

CMenu::CMenu ( void ) : m_iPageSize ( 19 ), m_iMaxItems ( 255 )
{
	ZeroMemory ( &m_vPos, sizeof ( POINT ) );
	ZeroMemory ( &m_sColor, sizeof ( SMenuColor ) );

	m_pMenuManager = NULL;

	m_bShowLines = m_bShowScrollbar = true;
	m_bLockControls = false;

	m_iCurrentRow = m_iNexSpace = m_iCountItemsEnabled = m_dwTime=0;
	m_iOldRow = m_iOldRowCount = -1;

	SSubMenuAddon::SInvokeMenu sInvoke;
	sInvoke.bAdded = false;
	sInvoke.bStat = true;

	SubMenu.InvokeMenu [ this ] = sInvoke;
}

CMenu::~CMenu ( void )
{
	ClearAllColumns ();
}

void CMenu::Initialize ( CMenuManager *pMenuManager )
{
	m_sColor.d3dBackGround			= D3DCOLOR_RGBA ( 0, 0, 0, 200 );
	m_sColor.d3dScrollBar			= D3DCOLOR_RGBA ( 180, 180, 180, 200 );
	m_sColor.d3dScrollBarBackGround = D3DCOLOR_RGBA ( 0, 0, 0, 100 );
	m_sColor.d3dCurItemBar			= D3DCOLOR_RGBA ( 255, 255, 255, 255 );
	m_sColor.d3dLine				= D3DCOLOR_RGBA ( 180, 180, 180, 255 );
	m_sColor.d3dEnabledItem			= D3DCOLOR_RGBA ( 80, 80, 80, 255 );
	m_sColor.d3dCurItemTex			= D3DCOLOR_RGBA ( 0, 0, 0, 255 );

	m_pMenuManager = pMenuManager;

	this->SetTextSpace ( 5 );
	this->SetTitleBarHeight ( 10 );
}

void CMenu::Draw ( void )
{
	if ( !m_ColumnAddon.size () || !m_pMenuManager )
		return;

	auto pRender = m_pMenuManager->GetRenderer ();
	auto pFont = m_pMenuManager->GetFont ();

	if ( !pFont || !pRender )
		return;

	
	if ( SubMenu.InvokeMenu [ this ].bStat )

	{
		if ( m_pEventHandler )
			m_pEventHandler ( this, m_iCurrentRow );

		if ( m_iPageSize > m_RowStatus.size () )
		{
			m_iPageSize = m_RowStatus.size ();
		}

		if ( m_iOldRow != m_iCurrentRow && m_iCountItemsEnabled )
		{
			if ( m_iCurrentRow > m_iOldRow )
			{
				this->_UpdateItemsUp ();

				if ( m_iCurrentRow >= m_RowStatus.size () )
				{
					m_iCurrentRow = m_iNexSpace = 0;
					this->_UpdateItemsUp ();
				}
			}
			else
			{
				this->_UpdateItemsDown ();

				if ( m_iNexSpace < 0 )
				{
					m_iNexSpace = m_RowStatus.size () - m_iPageSize;
					m_iCurrentRow = m_RowStatus.size () - 1;
					this->_UpdateItemsDown ();
				}
			}
			m_iOldRow = m_iCurrentRow;
		}

		if ( ( m_vPos.y + m_iTitleScale ) + ( m_iTextSpace * float ( m_iCurrentRow - m_iNexSpace ) ) <= ( m_vPos.y + m_iTitleScale ) )
		{
			m_bAtBegin = true;

			if ( m_iNexSpace < 0 )
			{
				m_iNexSpace = m_RowStatus.size () - m_iPageSize;
				m_iCurrentRow = m_RowStatus.size () - 1;
			}
		}
		else
		{
			m_bAtBegin = false;
		}

		if ( ( m_vPos.y + m_iTitleScale ) + ( m_iTextSpace *  float ( m_iCurrentRow - m_iNexSpace ) ) >= ( m_vPos.y + m_iTitleScale ) + ( float ( m_iPageSize * m_iTextSpace ) - m_iTextSpace ) )
		{
			m_bAtEnd = true;

			if ( m_iCurrentRow > m_RowStatus.size () || m_iNexSpace > ( m_RowStatus.size () - m_iPageSize ) )
			{
				m_iNexSpace = m_iCurrentRow = 0;
			}
		}
		else
		{
			m_bAtEnd = false;
		}

		m_vSize = this->GetSize ();
		static DWORD time = 0;
		if ( m_dwTime && 
			 m_szTextBox )
		{
			if(!time )
			time = ( 1.f * m_dwTime ) + timeGetTime ();

			if ( timeGetTime () < time )
			{
				RECT rctA, rctB;

				rctA.left = m_vPos.x + 4;
				rctA.top = m_vPos.y + m_vSize.y + m_iTitleScale + 6;
				rctA.right = rctA.left + m_vSize.x;
				rctA.bottom = rctA.top + ( m_vSize.y / 2 ) + m_iTitleScale + 6;

				rctB = rctA;

				DrawFont ( g_pFont, rctA,  DT_WORDBREAK| DT_CALCRECT, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), m_szTextBox );
				pRender->D3DBox ( m_vPos.x, m_vPos.y + m_vSize.y + m_iTitleScale + 4, m_vSize.x, rctA.bottom - rctA.top + 4, 0.f, m_sColor.d3dBackGround );
				DrawFont ( g_pFont, rctB, 0, D3DCOLOR_RGBA ( 255, 255, 255, 255 ), m_szTextBox );
			}
			else
			{
				time = 0;
				m_dwTime = 0;
			}
		}
	
		pRender->D3DBox ( m_vPos.x, m_vPos.y, m_vSize.x, m_vSize.y + m_iTitleScale, 0.f, m_sColor.d3dBackGround );
		pRender->D3DLine ( m_vPos.x, m_vPos.y + m_iTitleScale - 2.f, m_vPos.x + m_vSize.x, m_vPos.y + m_iTitleScale - 2.f, m_sColor.d3dLine );

		if ( m_RowStatus.size () > m_iPageSize && m_bShowScrollbar )
		{
			auto fScrollbarHeight = m_vSize.y / ( float ) m_RowStatus.size () * ( float ) m_iPageSize;

			if ( fScrollbarHeight <= 2.f )
			{
				fScrollbarHeight = 2.f;
			}

			auto fScrollbarPosY = ( m_vPos.y + m_iTitleScale ) + m_vSize.y / ( float ) m_RowStatus.size () * ( float ) m_iNexSpace;

			pRender->D3DBox ( m_vPos.x + m_vSize.x - 15.f, m_vPos.y + m_iTitleScale, 15.f, m_iTextSpace * m_iPageSize, 0.f, m_sColor.d3dScrollBarBackGround );
			pRender->D3DBox ( m_vPos.x + m_vSize.x - 15.f, fScrollbarPosY, 15.f, fScrollbarHeight, 0.f, m_sColor.d3dScrollBar );
		}

		if ( m_iCountItemsEnabled )
		{
			pRender->D3DBox ( ( float ) m_vPos.x, float ( m_vPos.y + m_iTitleScale ) + float ( m_iTextSpace * float ( m_iCurrentRow - m_iNexSpace ) ), m_bShowScrollbar && m_RowStatus.size () > m_iPageSize ? m_vSize.x - 15.f : m_vSize.x, m_iTextSpace, 0.f, m_sColor.d3dCurItemBar );
		}

		int iIncPosX = m_vPos.x;
		for ( size_t i = 0; i < m_ColumnAddon.size (); i++ )
		{
			int iIncPosY = m_vPos.y + m_iTitleScale;

			if ( m_ColumnAddon [ i ] )
			{
				if ( m_ColumnAddon [ i ]->szName )
				{
					if ( i )
					{
						iIncPosX += m_ColumnAddon [ i - 1 ]->iWidth;
					}

					std::string sTitleName;
					sTitleName = m_ColumnAddon [ i ]->szName;

					pFont->CutString ( m_ColumnAddon [ i ]->iWidth, sTitleName );
					pFont->PrintShadow ( ( float ) iIncPosX + 5.f, ( float ) m_vPos.y + ( ( float ) m_iTitleScale / 2.f ) - ( ( float ) pFont->GetHeight () / 2.f ), m_ColumnAddon [ i ]->d3dColor, FT_NONE | FT_VCENTER, sTitleName.c_str () );
				}

				size_t MaxItemsCount = m_ColumnAddon [ i ]->ItemAddon.sItemName.size () > m_iPageSize ? m_iNexSpace + m_iPageSize : m_ColumnAddon [ i ]->ItemAddon.sItemName.size ();

				for ( size_t j = m_iNexSpace; j < MaxItemsCount; j++, iIncPosY += m_iTextSpace )
				{
					if ( j >= m_ColumnAddon [ i ]->ItemAddon.sItemName.size () )
						break;

					bool bLastColumn = true;
					if ( m_RowStatus.size () > m_iPageSize && i == m_ColumnAddon.size () - 1
						 && m_bShowScrollbar )
						bLastColumn = false;

					if ( m_ColumnAddon [ i ]->ItemAddon.sItemName [ j ].c_str () )
					{
						pFont->CutString ( bLastColumn ? m_ColumnAddon [ i ]->iWidth - 2 : m_ColumnAddon [ i ]->iWidth - 15.f, m_ColumnAddon [ i ]->ItemAddon.sItemName [ j ] );
						pFont->Print ( ( float ) iIncPosX + 5.f, ( float ) iIncPosY + ( ( float ) m_iTextSpace / 2.f ) - ( ( float ) pFont->GetHeight () / 2.f ), !m_RowStatus [ j ] ? m_sColor.d3dEnabledItem : m_iCurrentRow != j ? m_ColumnAddon [ i ]->ItemAddon.d3dColor [ j ] : m_sColor.d3dCurItemTex, m_ColumnAddon [ i ]->ItemAddon.sItemName [ j ].c_str (), FT_NONE );
					}
				}

				if ( i )
				{
					pRender->D3DLine ( iIncPosX, m_vPos.y, iIncPosX, m_bShowLines ? ( m_vPos.y + m_iTitleScale ) + m_vSize.y : m_vPos.y + m_iTitleScale - 2.f, m_sColor.d3dLine );
				}
			}
		}

		static bool bKey;
		for ( size_t i = 0; i < SubMenu.Addon [ this ].size (); i++ )
		{
			if ( SubMenu.Addon [ this ] [ i ].ID == m_iCurrentRow )
			{
				if ( GetAsyncKeyState ( VK_SPACE ) )
				{
					if ( bKey )
					{
						bKey = false;

						SubMenu.Addon [ this ] [ i ].pMenu->SetSelectedRow ( 0 );

						SubMenu.InvokeMenu [ this ].bStat = false;
						SubMenu.InvokeMenu [ SubMenu.Addon [ this ] [ i ].pMenu ].bStat = true;

						SubMenu.iCount++;
						SubMenu.NextMenu [ SubMenu.iCount ] = SubMenu.Addon [ this ] [ i ].pMenu;
						break;
					}
				}
				else
				{
					bKey = true;
				}
			}

		}
	}
	else
	{
		static bool bKey;
		for ( size_t i = 0; i < SubMenu.Addon [ this ].size (); i++ )
		{
			if ( SubMenu.NextMenu [ SubMenu.iCount ] == SubMenu.Addon [ this ] [ i ].pMenu )
			{
				if ( GetAsyncKeyState ( VK_RETURN ) )
				{
					if ( bKey )
					{
						bKey = false;
						SubMenu.iCount--;

						SubMenu.InvokeMenu [ this ].bStat = true;
						SubMenu.InvokeMenu [ SubMenu.Addon [ this ] [ i ].pMenu ].bStat = false;
						break;
					}
				}
				else
				{
					bKey = true;
				}
			}
		}
	}
	iOldcu = m_iCurrentRow;
}


void CMenu::AddSubMenu ( CMenu *pMenu, int iRow )
{
	if ( pMenu == this || 
		 !pMenu || 
		 iRow > GetNumOfItems () )
		return;

	pMenu->SetPos ( this->GetPos () );

	SMenuAddon sAddon;
	sAddon.ID = iRow;
	sAddon.pMenu = pMenu;

	SubMenu.Addon [ this ].push_back ( sAddon );

	SSubMenuAddon::SInvokeMenu sInvoke;
	sInvoke.bAdded = true;
	sInvoke.bStat = false;

	SubMenu.InvokeMenu [ pMenu ] = sInvoke;
}

void CMenu::RemoveSubMenu ( CMenu *pMenu )
{
	if ( pMenu == this || 
		 !pMenu )
		return;

	for ( size_t i = 0; i < SubMenu.Addon [ this ].size (); i++ )
	{
		if ( SubMenu.Addon [ this ] [ i ].pMenu == pMenu )
		{
			SubMenu.InvokeMenu.erase ( pMenu );
			SubMenu.Addon [ this ].erase ( SubMenu.Addon [ this ].begin () + i );
			break;
		}
	}
}

void CMenu::RemoveAllSubMenus ( void )
{
	SubMenu.InvokeMenu.clear ();
	SubMenu.Addon [ this ].clear ();
}

void CMenu::AddColumn ( int iColumnID, const char *szName, int iWidth, D3DCOLOR d3dColor )
{
	if ( !iWidth )
	{
		int iSize = 0;

		if ( !m_pMenuManager )
			return;

		auto pFont = m_pMenuManager->GetFont ();
		if ( pFont )
		{
			iSize = pFont->GetWidth ( szName );
		}

		iWidth = iSize;
	}

	bool bExistColumn = false;
	if ( m_ColumnAddon.size () )
	{
		for ( size_t i = 0; i < m_ColumnAddon.size (); i++ )
		{
			if ( m_ColumnAddon [ i ]->iID == iColumnID )
			{
				m_ColumnAddon [ i ]->szName = ( char* ) szName;
				m_ColumnAddon [ i ]->iWidth = iWidth;
				m_ColumnAddon [ i ]->d3dColor = d3dColor;
				bExistColumn = true;
				break;
			}
		}
	}

	if ( !bExistColumn )
	{
		m_ColumnAddon.push_back ( new SColumn ( iWidth, szName, d3dColor, iColumnID ) );
	}
}

void CMenu::AddColumnItem ( int iColumnID, D3DCOLOR d3dColor, bool bSetValue, const char *szItem, ... )
{


	if ( szItem && m_ColumnAddon [ iColumnID ] )
	{
		if ( m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName.size () >= m_iMaxItems )
			return;

		char szBuffer [ 1024 ];
		va_list ap;
		va_start ( ap, szItem );
		vsnprintf ( szBuffer, 1024, szItem, ap );
		va_end ( ap );

		m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName.push_back ( szBuffer );
		m_ColumnAddon [ iColumnID ]->ItemAddon.d3dColor.push_back ( d3dColor );

		auto numItems = GetNumOfItems () ;
		if ( numItems > m_RowStatus.size () )
		{
			m_bKeys.push_back ( false );
			m_RowStatus [ numItems - 1 ] = true;
			m_iCountItemsEnabled++;
		}
	}
}

void CMenu::RemoveItemByName ( int iColumnID, const char *szItem )
{
	
	if ( m_ColumnAddon [ iColumnID ] )
	{
		for ( size_t i = 0; i < m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName.size (); i++ )
		{
			if ( !strcmp ( szItem, m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName [ i ].c_str () ) )
			{
				RemoveItem ( iColumnID, i );
				break;
			}
		}
	}
}

void CMenu::RemoveItem ( int iColumnID, int iRow )
{
	if (  
		 iRow > -1 && 
		 iRow < m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName.size () )
	{
		if ( m_ColumnAddon [ iColumnID ] )
			m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName.erase ( m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName.begin () + iRow );
	}
}

void CMenu::RemoveColumnByName ( const char *szName )
{
	for ( size_t i = 0; i < m_ColumnAddon.size (); i++ )
	{
		if ( !strcmp ( szName, m_ColumnAddon [ i ]->szName ) )
		{
			RemoveColumn ( i );
			break;
		}
	}
}

void CMenu::RemoveColumn ( int iColumnID )
{
	

	ClearItemsFromColumn ( iColumnID );
	SAFE_DELETE ( m_ColumnAddon [ iColumnID ] );

	m_ColumnAddon.erase ( m_ColumnAddon.begin () + iColumnID );
}

char *CMenu::GetColumnName ( int iColumnID )
{
	if ( m_ColumnAddon [ iColumnID ] )
		return m_ColumnAddon [ iColumnID ]->szName;
}

void CMenu::ClearItemsFromColumn ( int iColumnID )
{
	

	if ( m_ColumnAddon [ iColumnID ] )
	{
		m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName.clear ();
		m_ColumnAddon [ iColumnID ]->ItemAddon.d3dColor.clear ();
	}

	
	if ( m_ColumnAddon.size() <= 1)
	{
		m_iCountItemsEnabled = 0;
		//m_iOldRow = m_iOldRowCount = -1;

		m_RowStatus.clear ();
	}
}

void CMenu::ClearAllColumns ( void )
{
	for ( size_t i = 0; i < m_ColumnAddon.size (); i++ )
		RemoveColumn ( i );

	m_iCountItemsEnabled = 0;
	m_iCurrentRow = m_iNexSpace = 0;
	m_iOldRow = m_iOldRowCount = -1;

	m_RowStatus.clear ();
}

char *CMenu::GetSelectedRowByName ( int iColumnID )
{

	if ( m_ColumnAddon [ iColumnID ] )
	{
		return ( char* ) m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName [ m_iCurrentRow ].c_str ();
	}

	return NULL;
}

void CMenu::SetSelectedRowByName ( int iColumnID, const char *szItem )
{
	

	if ( m_ColumnAddon [ iColumnID ] )
	{
		for ( size_t i = 0; i < m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName.size (); i++ )
		{
			if ( !strcmp ( szItem, m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName [ i ].c_str () ) )
			{
				SetSelectedRow ( i );
				break;
			}
		}
	}
}

void CMenu::SetSelectedRow ( int iRow )
{
	if ( GetNumOfItems () > iRow )
	{
		m_iNexSpace = 0;

		if ( iRow > m_iPageSize )
		{
			m_iNexSpace = iRow - m_iPageSize + 1;
		}
	
		m_iCurrentRow = iRow;
		m_iOldRow = - 1;
	}
}

void CMenu::SetEnabledRowByName ( int iColumnID, const char *szItem, bool bActivated )
{
	

	if ( m_ColumnAddon [ iColumnID ] )
	{
		for ( size_t i = 0; i < m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName.size (); i++ )
		{
			if ( !strcmp ( szItem, m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName [ i ].c_str () ) )
			{
				SetEnabledRow ( i, bActivated );
				break;
			}
		}
	}
}

void CMenu::SetEnabledRow ( int iRow, bool bActivate )
{
	//static std::map<int, int> iOldRowCount;
	//static bool bOldState = false;
	if ( GetNumOfItems () > iRow )
	{
		if ( iOldRowCount [ iRow ] != iRow || bOldState != bActivate)
		{
			if ( bActivate )
			{
				m_iCountItemsEnabled++;
			}
			else if ( m_iCountItemsEnabled )
			{
				m_iCountItemsEnabled--;
			}
			bOldState = bActivate;
			iOldRowCount [ iRow ] = iRow;		
		
			
		}

		m_RowStatus [ iRow ] = bActivate;		
	}
}


bool CMenu::OnKeyPressed ( int iRow )
{
	static  bool bKey;

	static int iLasRow; 

	if ( m_iCurrentRow == iRow &&
		 SubMenu.InvokeMenu [ this ].bStat )
	{
		if ( GetAsyncKeyState ( VK_SPACE ) )
		{
			if ( bKey  && bKey )
			{
				bKey = false;
				return true;
			}
		}
		else
		{
			if ( GetAsyncKeyState ( VK_RETURN ) )
				bKey= false;
			else
				bKey  = true;
		}
	}

	return false;
}

void CMenu::SetNewItem ( int iColumnID, int iRow, const char *szItem, ... )
{
	if ( szItem )
	{
		char szBuffer [ 1024 ];
		va_list ap;
		va_start ( ap, szItem );
		vsnprintf ( szBuffer, 1024, szItem, ap );
		va_end ( ap );


		m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName [ iRow ] = szBuffer;
	}
	else m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName [ iRow ] = szItem ;
}

void CMenu::AddTextBox ( DWORD dwTime, const char *szText, ... )
{
	if ( szText )
	{
		char szBuffer [ 1024 ];
		va_list ap;
		va_start ( ap, szText );
		vsnprintf ( szBuffer, 1024, szText, ap );
		va_end ( ap );

		m_dwTime = dwTime;
		strcpy ( m_szTextBox, szBuffer );
	}
}

void CMenu::SetColor ( SMenuColor sColor )
{
	m_sColor = sColor;
}

SMenuColor CMenu::GetColor ( void )
{
	return m_sColor;
}

void CMenu::LockControls ( bool bLock )
{
	m_bLockControls = bLock;
}

D3DXVECTOR2 CMenu::GetPos ( void )
{
	return m_vPos;
}

void CMenu::SetPos ( const D3DXVECTOR2 &vVect )
{
	m_vPos = vVect;
}

void CMenu::SetPos ( float fX, float fY )
{
	m_vPos.x = fX;
	m_vPos.y = fY;
}

void CMenu::SetPageSize ( int iSize )
{
	if ( iSize > -1 )
		m_iPageSize = iSize;
}

void CMenu::SetEventHandler ( tEventHandler pEvent )
{
	m_pEventHandler = pEvent;
}

void CMenu::ShowLines ( bool bShow )
{
	m_bShowLines = bShow;
}

void CMenu::ShowScrollbar ( bool bShow )
{
	m_bShowScrollbar = bShow;
}

int CMenu::GetSelectedRow ( void )
{
	return m_iCurrentRow;
}

void CMenu::SetTitleBarHeight ( int iHeight )
{
	int iTextHeight = 0;

	if ( !m_pMenuManager )
		return;

	auto pFont = m_pMenuManager->GetFont ();
	if ( pFont )
	{
		iTextHeight = pFont->GetHeight ();
	}

	m_iTitleScale = iTextHeight + iHeight;
}

void CMenu::SetTextSpace ( int iSpace )
{
	int iTextHeight = 0;

	if ( !m_pMenuManager )
		return;

	auto pFont = m_pMenuManager->GetFont ();
	if ( pFont )
	{
		iTextHeight = pFont->GetHeight ();
	}

	m_iTextSpace = iTextHeight + iSpace;
}

void CMenu::SetMaxItems ( int iSize )
{
	if ( iSize > -1 )
	{
		m_iMaxItems = iSize;
	}
}

void CMenu::SetHeight ( int iHeight )
{
	if ( iHeight < -1 )
		return;

	m_iHeight = iHeight;

	if ( iHeight >= m_iPageSize )
	{
		m_iPageSize = iHeight;
	}
}

D3DXVECTOR2 CMenu::GetSize ( void )
{
	int iAddWidth = 0.f;
	size_t size = 0;

	for ( size_t i = 0; i < m_ColumnAddon.size (); i++ )
	{
		if ( m_ColumnAddon [ i ] )
		{
			iAddWidth = iAddWidth + m_ColumnAddon [ i ]->iWidth;
			if ( i )
			{
				size = max ( size, m_ColumnAddon [ i ]->ItemAddon.sItemName.size () );
			}
			else
			{
				size = m_ColumnAddon [ i ]->ItemAddon.sItemName.size ();
			}
		}
	}

	auto height = size > m_iPageSize ?
		m_iTextSpace * m_iPageSize :
		m_iTextSpace * size;

	return D3DXVECTOR2 ( iAddWidth,
						 m_iHeight * m_iTextSpace > height ? m_iHeight * m_iTextSpace : height );
}

size_t CMenu::GetNumOfItems ( void )
{
	size_t size = 0;
	for ( size_t i = 0; i < m_ColumnAddon.size (); i++ )
	{
		if ( m_ColumnAddon [ i ] )
		{
			if ( i )
			{
				size = max ( size, m_ColumnAddon [ i ]->ItemAddon.sItemName.size () );
			}
			else
			{
				size = m_ColumnAddon [ i ]->ItemAddon.sItemName.size ();
			}
		}
	}
	return size;
}

void CMenu::HandleMessage ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	
	switch ( uMsg )
	{
		case WM_KEYDOWN:
		{
			switch ( wParam )
			{
				case VK_UP:
				case 0x57:
				{
					if ( !SubMenu.InvokeMenu [ this ].bStat ||
						 m_bLockControls )
						return;

					if ( m_iCountItemsEnabled < 2 )
						break;
					
					m_iCurrentRow--;
					if ( m_bAtBegin )
						m_iNexSpace--;
					break;
				}
				case VK_DOWN:
				case 0x53:
				{
					if ( !SubMenu.InvokeMenu [ this ].bStat ||
						 m_bLockControls )
						return;

					if ( m_iCountItemsEnabled < 2 )
						break;
					
					m_iCurrentRow++;
					if ( m_bAtEnd )
						m_iNexSpace++;

					break;
				}
			}
			break;
		}
	}
}

void CMenu::_UpdateItemsUp ( void )
{
	int iCountItemsEnabled = 0;

	for ( size_t i = 0; i < m_RowStatus.size (); i++ )
	{
		if ( m_iCurrentRow == i || iCountItemsEnabled )
		{
			if ( m_RowStatus [ i ] )
				break;

			iCountItemsEnabled++;
		}
	}

	if ( iCountItemsEnabled )
	{
		m_iCurrentRow = m_iCurrentRow + iCountItemsEnabled;
		if ( ( m_vPos.y + m_iTitleScale ) + ( m_iTextSpace * ( m_iCurrentRow - m_iNexSpace ) ) >= ( m_vPos.y + m_iTitleScale ) + ( ( m_iPageSize * m_iTextSpace ) - m_iTextSpace ) &&
			 GetNumOfItems () > m_iPageSize )
		{
			m_iNexSpace = m_iCurrentRow - m_iPageSize + 1;
		}
	}
}

void CMenu::_UpdateItemsDown ( void )
{
	int iCountItemsEnabled = 0;

	for ( int i = m_iCurrentRow; i >= 0; i-- )
	{
		if ( m_iCurrentRow == i ||
			 iCountItemsEnabled )
		{
			if ( m_RowStatus [ i ] )
				break;

			iCountItemsEnabled++;
		}
	}

	if ( iCountItemsEnabled )
	{
		m_iCurrentRow = m_iCurrentRow - iCountItemsEnabled;
		if ( ( m_vPos.y + m_iTitleScale ) + ( m_iTextSpace * ( m_iCurrentRow - m_iNexSpace ) ) < ( m_vPos.y + m_iTitleScale ) )
		{
			m_iNexSpace = m_iCurrentRow;
		}
	}
}

