#pragma once

#include "CGraphics.h"

class CMenu;

typedef void ( __cdecl *tEventHandler )( CMenu*, int );

struct SMenuAddon
{
	int ID = -1;
	CMenu *pMenu = NULL;
	bool bDraw;
};

class CMenuManager
{
public:
	CMenuManager ( void );
	~CMenuManager ( void );

	HRESULT InitializeDeviceObjs ( IDirect3DDevice9 *pDevice );

	void AddMenu ( int ID );
	void RemoveMenu ( int ID );
	void RemoveAllMenus ( void );

	CMenu *GetMenu ( int ID ) const;

	CD3DRender *GetRenderer ( void );
	CD3DFont *GetFont ( void );

	void Draw ( void );
	void SetDrawable ( int ID, bool bDraw );

	void HandleMessage ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void OnLostDevice ( void );
	void OnResetDevice ( void );

private:
	CD3DFont *m_pFont;
	CD3DRender *m_pRender;
	IDirect3DDevice9 *m_pDevice;

	std::vector<SMenuAddon> m_MenuAddon;
};

struct SMenuColor
{
	D3DCOLOR d3dBackGround;
	D3DCOLOR d3dEnabledItem;
	D3DCOLOR d3dSelecItemBar;
	D3DCOLOR d3dCurItemTex;
	D3DCOLOR d3dLine;
	D3DCOLOR d3dScrollBar;
	D3DCOLOR d3dScrollBarBackGround;
};

class CMenu
{
public:

	CMenu ( void );
	~CMenu ( void );

	void Initialize ( CMenuManager *pMenuManager );

	void SetColor ( SMenuColor sColor );
	SMenuColor GetColor ( void );

	void AddColumn ( int iColumnID, const char *szName, int iWidth = 0, D3DCOLOR d3dColor = D3DCOLOR_RGBA ( 255, 255, 255, 255 ) );
	void AddColumnItem ( int iColumnID, D3DCOLOR d3dColor, const char *szItem, ... );

	void RemoveItem ( int iColumnID, int iRow );
	void RemoveItemByName ( int iColumnID, const char *szItem );

	void RemoveColumn ( int iColumnID );
	void RemoveColumnByName ( const char *szName );

	void ClearAllColumns ( void );
	void ClearItemsFromColumn ( int iColumnID );

	int GetSelectedRow ( void );
	char *GetSelectedRowByName ( int iColumnID );

	void SetSelectedRowByName ( int iColumnID, const char *szItem );
	void SetSelectedRow ( int iRow );

	D3DXVECTOR2 GetPos ( void );
	D3DXVECTOR2 GetSize ( void );

	void SetPos ( const D3DXVECTOR2 &vVector );
	void SetPos ( float fX, float fY );

	void SetPageSize ( int iSize );
	void SetEventHandler ( tEventHandler pAction );

	void ShowLines ( bool bShow );
	void ShowScrollbar ( bool bShow );

	void SetMaxItems ( int iSize );
	size_t GetNumOfItems ( void );

	void SetTitleBarHeight ( int iWidth );
	void SetTextSpace ( int iSpace );

	void SetHeight ( int iHeight );

	void SetEnabledRowByName ( int iColumnID, const char *szItem, bool bActivated );
	void SetEnabledRow ( int iRow, bool bActivate );

	void Draw ( void );

	bool OnKeyPressed ( int iRow );

	void LockControls ( bool bLock );

	void HandleMessage ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void AddSubMenu ( CMenu *pMenu, int iRow );
	void RemoveSubMenu ( CMenu* );
	void RemoveAllSubMenus ( void );
private:

	struct SColumn
	{
		struct SColumnItem
		{
			std::vector<std::string> sItemName;
			std::vector<D3DCOLOR> d3dColor;
		};

		int iWidth;
		char *szName;
		int iID;
		D3DCOLOR d3dColor;
		SColumnItem ItemAddon;

		SColumn ( int iWidth, const char *szName, D3DCOLOR d3dColor, int iID )
		{
			this->iWidth = iWidth;
			this->szName = ( char* ) szName;
			this->iID = iID;
			this->d3dColor = d3dColor;
		}
	};

	std::vector<SColumn*> m_ColumnAddon;
	std::map<int, bool> m_RowStatus;

	SMenuColor m_sColor;
	CMenuManager *m_pMenuManager;
	tEventHandler m_pEventHandler;

	D3DXVECTOR2 m_vSize;
	D3DXVECTOR2 m_vPos;

	int m_iOldRow;
	int m_iPageSize;
	int m_iCurrentRow;
	int m_iNexSpace;
	int m_iOldRowCount;
	int m_iHeight;
	int m_iMaxItems;
	int m_iCountItemsEnabled;

	int m_iTitleScale;
	int m_iTextSpace;

	bool m_bShowLines;
	bool m_bShowScrollbar;
	bool m_bAtBegin;
	bool m_bAtEnd;
	bool m_bLockControls;

	std::map<int, bool> m_bKeys;

	void _UpdateItemsUp ( void );
	void _UpdateItemsDown ( void );
};