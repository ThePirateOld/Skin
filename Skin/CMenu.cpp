#include "CMenu.h"

struct SSubMenuAddon
{
	struct SInvokeMenu
	{
		bool bStat;
		bool bAdded;
	};

	int iCount = 0;

	bool bPrevMenu;
	bool bNextMenu;

	std::map<CMenu*, SInvokeMenu> InvokeMenu;
	std::map<CMenu*, std::vector<SMenuAddon>> Addon;
	std::map<int, CMenu*> NextMenu;
}SubMenu;

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
	m_pFont = new CD3DFont ( "Arial", 10, FCR_BOLD );

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
}

void CMenuManager::OnResetDevice ( void )
{
	if ( m_pFont )
		m_pFont->Initialize ( m_pDevice );

	if ( m_pRender )
		m_pRender->Initialize ( m_pDevice );
}

CMenu::CMenu ( void ) : m_iPageSize ( 10 ), m_iMaxItems ( 255 )
{
	ZeroMemory ( &m_vPos, sizeof ( POINT ) );
	ZeroMemory ( &m_sColor, sizeof ( SMenuColor ) );

	m_pMenuManager = NULL;

	m_bShowLines = m_bShowScrollbar = true;
	m_bLockControls = false;

	m_iCurrentRow = m_iNexSpace = 0;
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
	m_sColor.d3dSelecItemBar		= D3DCOLOR_RGBA ( 255, 255, 255, 255 );
	m_sColor.d3dLine				= D3DCOLOR_RGBA ( 180, 180, 180, 255 );
	m_sColor.d3dEnabledItem			= D3DCOLOR_RGBA ( 180, 180, 180, 255 );
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
			pRender->D3DBox ( ( float ) m_vPos.x, float ( m_vPos.y + m_iTitleScale ) + float ( m_iTextSpace * float ( m_iCurrentRow - m_iNexSpace ) ), m_bShowScrollbar && m_RowStatus.size () > m_iPageSize ? m_vSize.x - 15.f : m_vSize.x, m_iTextSpace, 0.f, m_sColor.d3dSelecItemBar );
		}

		for ( size_t i = 0; i < m_ColumnAddon.size (); i++ )
		{
			int iIncPosX = m_vPos.x,
				iIncPosY = m_vPos.y + m_iTitleScale;

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

		if ( m_pEventHandler )
			m_pEventHandler ( this, m_iCurrentRow );

		for ( size_t i = 0; i < SubMenu.Addon [ this ].size (); i++ )
		{
			if ( GetAsyncKeyState ( VK_SPACE ) )
			{
				if ( SubMenu.bNextMenu )
				{
					SubMenu.bNextMenu = false;

					if ( SubMenu.Addon [ this ] [ i ].ID == m_iCurrentRow )
					{
						SubMenu.Addon [ this ] [ i ].pMenu->SetSelectedRow ( 0 );

						SubMenu.InvokeMenu [ this ].bStat = false;
						SubMenu.InvokeMenu [ SubMenu.Addon [ this ] [ i ].pMenu ].bStat = true;

						SubMenu.iCount++;
						SubMenu.NextMenu [ SubMenu.iCount ] = SubMenu.Addon [ this ] [ i ].pMenu;
						break;
					}
				}
			}
			else
			{
				SubMenu.bNextMenu = true;
			}
		}
	}
	else
	{
		for ( size_t i = 0; i<SubMenu.Addon [ this ].size (); i++ )
		{	
			if ( GetAsyncKeyState ( VK_RETURN ) )
			{
				if ( SubMenu.bPrevMenu )
				{
					SubMenu.bPrevMenu = false;

					if ( SubMenu.NextMenu [ SubMenu.iCount ] == SubMenu.Addon [ this ] [ i ].pMenu )
					{
						SubMenu.iCount--;

						SubMenu.InvokeMenu [ this ].bStat = true;
						SubMenu.InvokeMenu [ SubMenu.Addon [ this ] [ i ].pMenu ].bStat = false;
						break;
					}
				}
			}
			else
			{
				SubMenu.bPrevMenu = true;
			}
		}
	}
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

void CMenu::AddColumnItem ( int iColumnID, D3DCOLOR d3dColor, const char *szItem, ... )
{
	if ( iColumnID < -1 ||
		 iColumnID > m_ColumnAddon.size () )
	{
		return;
	}

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

		if ( GetNumOfItems () > m_RowStatus.size () )
		{
			m_RowStatus [ GetNumOfItems () - 1 ] = true;
			m_iCountItemsEnabled++;
		}
	}

}

void CMenu::RemoveItemByName ( int iColumnID, const char *szItem )
{
	if ( iColumnID < -1 || 
		 iColumnID > m_ColumnAddon.size () )
	{
		return;
	}

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
	if ( iColumnID > -1 && 
		 iColumnID < m_ColumnAddon.size () && 
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
	if ( iColumnID < -1 || 
		 iColumnID > m_ColumnAddon.size () )
	{
		return;
	}

	ClearItemsFromColumn ( iColumnID );
	SAFE_DELETE ( m_ColumnAddon [ iColumnID ] );

	m_ColumnAddon.erase ( m_ColumnAddon.begin () + iColumnID );
}

void CMenu::ClearItemsFromColumn ( int iColumnID )
{
	if ( iColumnID < -1 || 
		 iColumnID > m_ColumnAddon.size () )
	{
		return;
	}

	if ( m_ColumnAddon [ iColumnID ] )
	{
		m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName.clear ();
		m_ColumnAddon [ iColumnID ]->ItemAddon.d3dColor.clear ();
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
	if ( iColumnID < -1 || 
		 iColumnID > m_ColumnAddon.size () )
	{
		return NULL;
	}

	if ( m_ColumnAddon [ iColumnID ] )
	{
		return ( char* ) m_ColumnAddon [ iColumnID ]->ItemAddon.sItemName [ m_iCurrentRow ].c_str ();
	}

	return NULL;
}

void CMenu::SetSelectedRowByName ( int iColumnID, const char *szItem )
{
	if ( iColumnID < -1 || 
		 iColumnID > m_ColumnAddon.size () )
	{
		return;
	}

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
	if ( m_iCurrentRow == iRow )
		return;

	if ( GetNumOfItems () > iRow )
	{
		if ( iRow > m_iPageSize )
		{
			m_iNexSpace = iRow - m_iPageSize + 1;
		}

		m_iCurrentRow = iRow;
	}
}

void CMenu::SetEnabledRowByName ( int iColumnID, const char *szItem, bool bActivated )
{
	if ( iColumnID < -1 || 
		 iColumnID > m_ColumnAddon.size () )
	{
		return;
	}

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
	if ( GetNumOfItems () > iRow )
	{
		if ( m_iOldRowCount != iRow )
		{
			if ( bActivate && 
				 !m_RowStatus [ iRow ] )
			{
				m_iCountItemsEnabled++;
			}
			else if ( m_iCountItemsEnabled && 
					  m_RowStatus [ iRow ] )
			{
				m_iCountItemsEnabled--;
			}
			m_iOldRowCount = iRow;
		}
		m_RowStatus [ iRow ] = bActivate;
	}
}


bool CMenu::OnKeyPressed ( int iRow )
{
	if ( GetAsyncKeyState ( VK_SPACE ) )
	{
		if ( m_bKeys [ iRow ] )
		{
			m_bKeys [ iRow ] = false;

			if ( m_iCurrentRow == iRow )
			{
				return true;
			}
		}
	}
	else
	{
		m_bKeys [ iRow ] = true;
	}

	return false;
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

	return D3DXVECTOR2 ( m_iHeight * m_iTextSpace > height ? m_iHeight * m_iTextSpace : height, 
						 iAddWidth );
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
	if ( !SubMenu.InvokeMenu [ this ].bStat || m_bLockControls )
		return;

	switch ( uMsg )
	{
		case WM_KEYDOWN:
		{
			switch ( wParam )
			{
				case VK_UP:
				case 0x57:
				{
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

