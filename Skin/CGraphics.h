#pragma once

#include <tchar.h>
#include "define.h"

#define FCR_NONE	0x0
#define FCR_BOLD 	0x1
#define FCR_ITALICS 0x2

#define FT_NONE		0x0
#define FT_CENTER	0x1
#define FT_BORDER	0x2
#define FT_VCENTER	0x4
#define FT_SINGLELINE 0x8

//Vertex Types
struct FONTVERTEX
{
	float x, y, z, rhw; 
	DWORD colour; 
	float tu, tv;
};

struct LVERTEX
{
	float	x, y, z, rhw;
	DWORD	color;
	float	tu, tv;
};

class CD3DFont;
class CD3DRender;
class CD3DTexture;

class CGraphics
{
public:
	CGraphics ( IDirect3DDevice9 *pDevice );
	~CGraphics ( void );

	void CreateD3DFont ( int ID, const char *pszFontName, int iFontHeight, DWORD dwCreateFlags = 0 );
	void CreateD3DRender ( int ID, int iNumVertices );
	void CreateD3DTexture ( int ID, const char *pszPath );

	CD3DFont *GetFontAt ( int ID );
	CD3DRender *GetRendererAt ( int ID );
	CD3DTexture *GetTextureAt ( int ID );

	void RemoveItem ( HANDLE hItem );

	void OnLostDevice ( void );
	void OnResetDevice ( void );

	IDirect3DDevice9 *GetDevice ( void ) { return m_pd3dDevice; }

private:

	struct SItem
	{
		int ID = -1;
		CD3DFont *pFont			= NULL;
		CD3DRender *pRender		= NULL;
		CD3DTexture *pTexture	= NULL;
	};

	std::vector<SItem> m_ItemAddon;
	IDirect3DDevice9 *m_pd3dDevice= NULL;
};

class CD3DState
{
protected:
	void Initialize ( IDirect3DDevice9 *pDevice );
	void Invalidate ( void );

public:
	HRESULT BeginState ( void );
	HRESULT EndState ( void );

private:
	IDirect3DStateBlock9* m_pStateBlock = NULL;

protected:
	IDirect3DDevice9 *m_pd3dDevice = NULL;
};

class CD3DRender : public CD3DState
{
public:
	CD3DRender ( int numVertices );
	~CD3DRender ( );

	HRESULT Initialize ( IDirect3DDevice9 *pD3Ddev );
	HRESULT Invalidate ( );

	HRESULT BeginRender ( D3DPRIMITIVETYPE primType );
	HRESULT EndRender ( );

	void D3DColor ( DWORD color );

	void D3DTexCoord2f ( float u, float v );
	HRESULT D3DVertex2f ( float x, float y );

	void D3DTriangle ( float fX, float fY, float fSize, float fAngle, D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor = D3DCOLOR_RGBA ( 0, 0, 0, 255 ), bool bAntAlias = false );
	void D3DCircle ( float fX, float fY, float fSize, D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor = D3DCOLOR_RGBA ( 0, 0, 0, 255 ), bool bAntAlias = false );
	void D3DBox ( float fX, float fY, float fWidth, float fHeight, float fAngle, D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor = D3DCOLOR_RGBA ( 0, 0, 0, 255 ), bool bAntAlias = false );
	void D3DLine ( float fStartX, float fStartY, float fEndX, float fEndY, D3DCOLOR d3dColor, bool bAntAlias = true );
private:
	D3DPRIMITIVETYPE		m_primType;
	IDirect3DVertexBuffer9	*m_pD3Dbuf;
	LVERTEX					*m_pVertex;

	DWORD					m_color;
	float					m_tu, m_tv;
	int						m_maxVertex;
	int						m_curVertex;

	bool					m_canRender;
};

class CD3DFont : public CD3DState
{
public:
	CD3DFont ( const char *szFontName, int fontHeight, DWORD dwCreateFlags = 0 );
	~CD3DFont ( );

	HRESULT Initialize ( IDirect3DDevice9 *pD3Ddev );
	HRESULT Invalidate ( );

	HRESULT Print ( float x, float y, DWORD colour, const char *szText ,DWORD dwFlags = 0 );
	HRESULT PrintShadow ( float x, float y, DWORD colour, DWORD dwFlags, const char *szText,... );

	void CutString ( int iWidth, std::string &sString );

	int GetWidth ( const char* );
	int GetHeight ( void );

private:
	char 	m_szFontName [ 31 + 1 ];
	int  	m_fontHeight;
	DWORD 	m_dwCreateFlags;

	bool 	m_isReady;
	bool	m_statesOK;

	IDirect3DTexture9 		*m_pD3Dtex;
	IDirect3DVertexBuffer9 	*m_pD3Dbuf;

	DWORD 	m_maxTriangles;

	int 	m_texWidth, m_texHeight;
	int 	m_chrSpacing;
	float 	m_fTexCoords [ 256 ] [ 4 ];
	float 	m_fChrHeight;
};

class CD3DTexture
{
public:
	CD3DTexture ( IDirect3DDevice9* pd3dDevice, const char *szPath );
	~CD3DTexture ( void );

	void Draw ( float fX, float fY, float fScaleX, float fScaleY, float fRotation = 0.f, D3DCOLOR d3dColor = D3DCOLOR_XRGB ( 255, 255, 255 ) );

	void OnLostDevice ( void );
	void OnResetDevice ( void );

private:
	IDirect3DTexture9	*m_pTexture = NULL;

	ID3DXSprite			*m_pSprite = NULL;
	IDirect3DDevice9	*m_pDevice = NULL;
	char				*m_szPath = NULL;
};