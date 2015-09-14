#pragma once

#include <tchar.h>
#include "define.h"


// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004

// Font rendering flags
#define D3DFONT_CENTERED_X  0x0001
#define D3DFONT_CENTERED_Y  0x0002
#define D3DFONT_TWOSIDED    0x0004
#define D3DFONT_FILTERED    0x0008
#define D3DFONT_BORDER		0x0010
#define D3DFONT_COLORTABLE	0x0020
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

//-----------------------------------------------------------------------------
// Name: class CD3DFont
// Desc: Texture-based font class for doing text in a 3D scene.
//-----------------------------------------------------------------------------
class CD3DFont : public CD3DState
{
	CHAR   m_strFontName [ 80 ];            // Font properties
	DWORD   m_dwFontHeight;
	DWORD   m_dwFontFlags;


	LPDIRECT3DTEXTURE9      m_pTexture;   // The d3d texture for this font
	LPDIRECT3DVERTEXBUFFER9 m_pVB;        // VertexBuffer for rendering text
	DWORD   m_dwTexWidth;                 // Texture dimensions
	DWORD   m_dwTexHeight;
	FLOAT   m_fTextScale;
	FLOAT   m_fTexCoords [ 128 - 32 ] [ 4 ];
	DWORD   m_dwSpacing;                  // Character pixel spacing per side

										  // Stateblocks for setting and restoring render states

	float m_fWidth;
public:
	// 2D and 3D text drawing functions
	HRESULT DrawText ( FLOAT x, FLOAT y, DWORD dwColor, const CHAR* strText, DWORD dwFlags = 0L );

	// Function to get extent of text
	HRESULT GetTextExtent ( const CHAR* strText, SIZE* pSize );

	// Initializing and destroying device-dependent objects
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects ();
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

	void SetWidth ( float fWidth ) { m_fWidth = fWidth };

	LPDIRECT3DDEVICE9 getDevice () const { return m_pd3dDevice; }
	// Constructor / destructor
	CD3DFont ( const CHAR* strFontName, DWORD dwHeight, DWORD dwFlags = 0L );
	~CD3DFont ();
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