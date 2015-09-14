#include "CGraphics.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>

#include <tchar.h>
#include <math.h>
#include <assert.h>

#pragma warning (push)
#pragma warning (disable : 4996)


//=========================================================================

//-------------------------------------------------------------------------
// Custom vertex types for rendering text
//-------------------------------------------------------------------------

#define D3DFVF_BITMAPFONT ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define D3DFVF_FONT3DVERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL| D3DFVF_TEX1 )

#define D3DFVF_PRIMITIVES	( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

inline LVERTEX Init2DVertex ( float x, float y,
							  DWORD color, float tu, float tv )
{
	return { x, y, 1.0f, 1.0f, color, tu, tv };
}

inline FONTVERTEX InitFont2DVertex ( float x, float y,
									 DWORD colour, float tu, float tv )
{
	return { x, y, 1.0f, 1.0f, colour, tu, tv };
}


//-----------------------------------------------------------------------------
// Custom vertex types for rendering text
//-----------------------------------------------------------------------------
#define MAX_NUM_VERTICES 50*6

struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };
struct FONT3DVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

inline FONT2DVERTEX InitFont2DVertex ( const D3DXVECTOR4& p, D3DCOLOR color,
									   FLOAT tu, FLOAT tv )
{
	FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
	return v;
}

inline FONT3DVERTEX InitFont3DVertex ( const D3DXVECTOR3& p, const D3DXVECTOR3& n,
									   FLOAT tu, FLOAT tv )
{
	FONT3DVERTEX v;   v.p = p;   v.n = n;   v.tu = tu;   v.tv = tv;
	return v;
}

// Helper function
void RotateVerts ( D3DXVECTOR2 *pVector, int iVertexCount, float fX, float fY, float fAngle )
{
	float fOldPosX;
	float fOldPosY;

	for ( size_t i = 0; i < iVertexCount; i++ )
	{
		fOldPosX = pVector [ i ].x;
		fOldPosY = pVector [ i ].y;
		pVector [ i ].x = fX + ( fOldPosX - fX ) * cos ( fAngle / ( 180.f / D3DX_PI ) ) + ( fOldPosY - fY ) * sin ( fAngle / ( 180.f / D3DX_PI ) );
		pVector [ i ].y = fX - ( fOldPosX - fX ) * sin ( fAngle / ( 180.f / D3DX_PI ) ) + ( fOldPosY - fY ) * cos ( fAngle / ( 180.f / D3DX_PI ) );
	}
}

CGraphics::CGraphics ( IDirect3DDevice9 *pDevice )
{
	m_pd3dDevice = pDevice;
}

CGraphics::~CGraphics ( void )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		SAFE_DELETE ( m_ItemAddon [ i ].pFont );
		SAFE_DELETE ( m_ItemAddon [ i ].pTexture );
		SAFE_DELETE ( m_ItemAddon [ i ].pRender );
	}
}

void CGraphics::CreateD3DFont ( int ID, const char *pszFontName, int iFontHeight, DWORD dwCreateFlags )
{
	CD3DFont *pFont = new CD3DFont ( pszFontName, iFontHeight, dwCreateFlags );
	if ( pFont )
		pFont->Initialize ( m_pd3dDevice );

	SItem sItem;
	sItem.pFont = pFont;
	sItem.ID = ID;

	m_ItemAddon.push_back ( sItem );
}

void CGraphics::CreateD3DRender ( int ID, int iNumVertices )
{
	CD3DRender *pRender = new CD3DRender ( iNumVertices );
	if ( pRender )
		pRender->Initialize ( m_pd3dDevice );

	SItem sItem;
	sItem.pRender = pRender;
	sItem.ID = ID;

	m_ItemAddon.push_back ( sItem );
}

void CGraphics::CreateD3DTexture ( int ID, const char *pszPath )
{
	auto pTexture = new CD3DTexture ( m_pd3dDevice, pszPath );

	SItem sItem;
	sItem.pTexture = pTexture;
	sItem.ID = ID;

	m_ItemAddon.push_back ( sItem );
}

CD3DFont *CGraphics::GetFontAt ( int ID )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pFont &&
			 m_ItemAddon [ i ].ID == ID )
		{
			return m_ItemAddon [ i ].pFont;
		}
	}
	
	return NULL;
}

CD3DRender *CGraphics::GetRendererAt ( int ID )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pRender &&
			 m_ItemAddon [ i ].ID == ID )
		{
			return m_ItemAddon [ i ].pRender;
		}
	}
	
	return NULL;
}

CD3DTexture *CGraphics::GetTextureAt ( int ID )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pTexture &&
			 m_ItemAddon [ i ].ID == ID )
		{
			return m_ItemAddon [ i ].pTexture;
		}
	}
	
	return NULL;
}

void CGraphics::RemoveItem ( HANDLE hItem )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pFont == ( CD3DFont* ) hItem )
			SAFE_DELETE ( m_ItemAddon [ i ].pFont )
		else if ( m_ItemAddon [ i ].pTexture == ( CD3DTexture* ) hItem )
			SAFE_DELETE ( m_ItemAddon [ i ].pTexture )
		else if ( m_ItemAddon [ i ].pRender == ( CD3DRender* ) hItem )
			SAFE_DELETE ( m_ItemAddon [ i ].pRender )
	}
}

void CGraphics::OnResetDevice ( void )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pFont )
			m_ItemAddon [ i ].pFont->Initialize ( m_pd3dDevice );
		else if ( m_ItemAddon [ i ].pRender )
			m_ItemAddon [ i ].pRender->Initialize ( m_pd3dDevice );
		else if ( m_ItemAddon [ i ].pTexture )
			m_ItemAddon [ i ].pTexture->OnLostDevice ();
	}
}

void CGraphics::OnLostDevice ( void )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pFont )
			m_ItemAddon [ i ].pFont->Invalidate ();
		else if ( m_ItemAddon [ i ].pRender )
			m_ItemAddon [ i ].pRender->Invalidate ();
		else if ( m_ItemAddon [ i ].pTexture )
			m_ItemAddon [ i ].pTexture->OnResetDevice ();
	}
}

void CD3DState::Initialize ( IDirect3DDevice9 *pDevice )
{
	if ( !m_pStateBlock )
		pDevice->CreateStateBlock ( D3DSBT_ALL, &m_pStateBlock );

	m_pd3dDevice = pDevice;
}

void CD3DState::Invalidate ( void )
{
	SAFE_RELEASE ( m_pStateBlock );
}

HRESULT CD3DState::BeginState ( void )
{
	if ( !m_pd3dDevice )
		return E_FAIL;

	if ( m_pStateBlock )
		m_pStateBlock->Capture ( );

	m_pd3dDevice->SetPixelShader ( NULL );
	m_pd3dDevice->SetVertexShader ( NULL );

	m_pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	m_pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, TRUE );
	m_pd3dDevice->SetRenderState ( D3DRS_ALPHAREF, 0x08 );
	m_pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	m_pd3dDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pd3dDevice->SetRenderState ( D3DRS_STENCILENABLE, FALSE );
	m_pd3dDevice->SetRenderState ( D3DRS_CLIPPING, TRUE );
	m_pd3dDevice->SetRenderState ( D3DRS_CLIPPLANEENABLE, FALSE );
	m_pd3dDevice->SetRenderState ( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
	m_pd3dDevice->SetRenderState ( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );
	m_pd3dDevice->SetRenderState ( D3DRS_COLORWRITEENABLE, 
								   D3DCOLORWRITEENABLE_RED | 
								   D3DCOLORWRITEENABLE_GREEN | 
								   D3DCOLORWRITEENABLE_BLUE | 
								   D3DCOLORWRITEENABLE_ALPHA );

	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	
	m_pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	return S_OK;
}

HRESULT CD3DState::EndState ( void )
{
	if ( m_pStateBlock )
		m_pStateBlock->Apply ( );

	return S_OK;
}
//-----------------------------------------------------------------------------
// Name: CD3DFont()
// Desc: Font class constructor
//-----------------------------------------------------------------------------
CD3DFont::CD3DFont ( const CHAR* strFontName, DWORD dwHeight, DWORD dwFlags )
{
	strncpy_s ( m_strFontName, strFontName, sizeof ( m_strFontName ) / sizeof ( CHAR ) );
	m_strFontName [ sizeof ( m_strFontName ) / sizeof ( CHAR ) - 1 ] = _T ( '\0' );
	m_dwFontHeight = dwHeight;
	m_dwFontFlags = dwFlags;
	m_dwSpacing = 0;

	m_pd3dDevice = NULL;
	m_pTexture = NULL;
	m_pVB = NULL;

}




//-----------------------------------------------------------------------------
// Name: ~CD3DFont()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CD3DFont::~CD3DFont ()
{
	InvalidateDeviceObjects ();
	DeleteDeviceObjects ();
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InitDeviceObjects ( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;

	CD3DState::Initialize ( pd3dDevice );

	// Establish the font and texture size
	m_fTextScale = 1.0f; // Draw fonts into texture without scaling

						 // Large fonts need larger textures
	if ( m_dwFontHeight > 60 )
		m_dwTexWidth = m_dwTexHeight = 2048;
	else if ( m_dwFontHeight > 30 )
		m_dwTexWidth = m_dwTexHeight = 1024;
	else if ( m_dwFontHeight > 15 )
		m_dwTexWidth = m_dwTexHeight = 512;
	else
		m_dwTexWidth = m_dwTexHeight = 256;

	// If requested texture is too big, use a smaller texture and smaller font,
	// and scale up when rendering.
	D3DCAPS9 d3dCaps;
	m_pd3dDevice->GetDeviceCaps ( &d3dCaps );

	if ( m_dwTexWidth > d3dCaps.MaxTextureWidth )
	{
		m_fTextScale = ( FLOAT ) d3dCaps.MaxTextureWidth / ( FLOAT ) m_dwTexWidth;
		m_dwTexWidth = m_dwTexHeight = d3dCaps.MaxTextureWidth;
	}

	// Create a new texture for the font
	hr = m_pd3dDevice->CreateTexture ( m_dwTexWidth, m_dwTexHeight, 1,
									   0, D3DFMT_A4R4G4B4,
									   D3DPOOL_MANAGED, &m_pTexture, NULL );
	if ( FAILED ( hr ) )
		return hr;

	// Prepare to create a bitmap
	DWORD*      pBitmapBits;
	BITMAPINFO bmi;
	ZeroMemory ( &bmi.bmiHeader, sizeof ( BITMAPINFOHEADER ) );
	bmi.bmiHeader.biSize = sizeof ( BITMAPINFOHEADER );
	bmi.bmiHeader.biWidth = ( int ) m_dwTexWidth;
	bmi.bmiHeader.biHeight = -( int ) m_dwTexHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 32;

	// Create a DC and a bitmap for the font
	HDC     hDC = CreateCompatibleDC ( NULL );
	HBITMAP hbmBitmap = CreateDIBSection ( hDC, &bmi, DIB_RGB_COLORS,
										   ( void** ) &pBitmapBits, NULL, 0 );
	SetMapMode ( hDC, MM_TEXT );

	// Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
	// antialiased font, but this is not guaranteed.
	INT nHeight = -MulDiv ( m_dwFontHeight,
							( INT ) ( GetDeviceCaps ( hDC, LOGPIXELSY ) * m_fTextScale ), 72 );
	DWORD dwBold = ( m_dwFontFlags&D3DFONT_BOLD ) ? FW_BOLD : FW_NORMAL;
	DWORD dwItalic = ( m_dwFontFlags&D3DFONT_ITALIC ) ? TRUE : FALSE;
	HFONT hFont = CreateFontA ( nHeight, 0, 0, 0, dwBold, dwItalic,
							   FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
							   CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							   VARIABLE_PITCH, m_strFontName );
	if ( NULL == hFont )
		return E_FAIL;

	SelectObject ( hDC, hbmBitmap );
	SelectObject ( hDC, hFont );

	// Set text properties
	SetTextColor ( hDC, RGB ( 255, 255, 255 ) );
	SetBkColor ( hDC, 0x00000000 );
	SetTextAlign ( hDC, TA_TOP );

	// Loop through all printable character and output them to the bitmap..
	// Meanwhile, keep track of the corresponding tex coords for each character.
	DWORD x = 0;
	DWORD y = 0;
	CHAR str [ 2 ] = "x" ;
	SIZE size;

	// Calculate the spacing between characters based on line height
	GetTextExtentPoint32A ( hDC,  " " , 1, &size );
	x = m_dwSpacing = ( DWORD ) ceil ( size.cy * 0.3f );

	for ( CHAR c = 32; c < 127; c++ )
	{
		str [ 0 ] = c;
		GetTextExtentPoint32A ( hDC, str, 1, &size );

		if ( ( DWORD ) ( x + size.cx + m_dwSpacing ) > m_dwTexWidth )
		{
			x = m_dwSpacing;
			y += size.cy + 1;
		}

		ExtTextOutA ( hDC, x + 0, y + 0, ETO_OPAQUE, NULL, str, 1, NULL );

		m_fTexCoords [ c - 32 ] [ 0 ] = ( ( FLOAT ) ( x + 0 - m_dwSpacing ) ) / m_dwTexWidth;
		m_fTexCoords [ c - 32 ] [ 1 ] = ( ( FLOAT ) ( y + 0 + 0 ) ) / m_dwTexHeight;
		m_fTexCoords [ c - 32 ] [ 2 ] = ( ( FLOAT ) ( x + size.cx + m_dwSpacing ) ) / m_dwTexWidth;
		m_fTexCoords [ c - 32 ] [ 3 ] = ( ( FLOAT ) ( y + size.cy + 0 ) ) / m_dwTexHeight;

		x += size.cx + ( 2 * m_dwSpacing );
	}

	// Lock the surface and write the alpha values for the set pixels
	D3DLOCKED_RECT d3dlr;
	m_pTexture->LockRect ( 0, &d3dlr, 0, 0 );
	BYTE* pDstRow = ( BYTE* ) d3dlr.pBits;
	WORD* pDst16;
	BYTE bAlpha; // 4-bit measure of pixel intensity

	for ( y = 0; y < m_dwTexHeight; y++ )
	{
		pDst16 = ( WORD* ) pDstRow;
		for ( x = 0; x < m_dwTexWidth; x++ )
		{
			bAlpha = ( BYTE ) ( ( pBitmapBits [ m_dwTexWidth*y + x ] & 0xff ) >> 4 );
			if ( bAlpha > 0 )
			{
				*pDst16++ = ( WORD ) ( ( bAlpha << 12 ) | 0x0fff );
			}
			else
			{
				*pDst16++ = 0x0000;
			}
		}
		pDstRow += d3dlr.Pitch;
	}

	// Done updating texture, so clean up used objects
	m_pTexture->UnlockRect ( 0 );
	DeleteObject ( hbmBitmap );
	DeleteDC ( hDC );
	DeleteObject ( hFont );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DFont::RestoreDeviceObjects ()
{
	HRESULT hr;

	// Create vertex buffer for the letters
	int vertexSize = max ( sizeof ( FONT2DVERTEX ), sizeof ( FONT3DVERTEX ) );
	if ( FAILED ( hr = m_pd3dDevice->CreateVertexBuffer ( MAX_NUM_VERTICES * vertexSize,
		 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
		 D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
	{
		return hr;
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InvalidateDeviceObjects ()
{
	SAFE_RELEASE ( m_pVB );

	CD3DState::Invalidate ();

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DeleteDeviceObjects ()
{
	SAFE_RELEASE ( m_pTexture );
	m_pd3dDevice = NULL;

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//-----------------------------------------------------------------------------
HRESULT CD3DFont::GetTextExtent ( const CHAR* strText, SIZE* pSize )
{
	if ( NULL == strText || NULL == pSize )
		return E_FAIL;

	FLOAT fRowWidth = 0.0f;
	FLOAT fRowHeight = ( m_fTexCoords [ 0 ] [ 3 ] - m_fTexCoords [ 0 ] [ 1 ] )*m_dwTexHeight;
	FLOAT fWidth = 0.0f;
	FLOAT fHeight = fRowHeight;

	while ( *strText )
	{
		CHAR c = *strText++;

		if ( c == '{' )
		{
			std::vector<char> _costumColor;
			while ( *strText != '}' )
			{
				char ch = toupper ( *strText );
				bool numeric = ( ch >= '0' && ch <= '9' ) || ( ch >= 'A' && ch <= 'F' );
				if ( numeric )
				{
					_costumColor.push_back ( ch );
				}
				strText++;
			}
			c = *strText++;
			continue;
		}

		if ( c ==  '\n' || 
			 fRowWidth >= m_fWidth )
		{
			fRowWidth = 0.0f;
			fHeight += fRowHeight;
		}

		if ( ( c - 32 ) < 0 || ( c - 32 ) >= 128 - 32 )
			continue;

		FLOAT tx1 = m_fTexCoords [ c - 32 ] [ 0 ];
		FLOAT tx2 = m_fTexCoords [ c - 32 ] [ 2 ];

		fRowWidth += ( tx2 - tx1 )*m_dwTexWidth - 2 * m_dwSpacing;

		if ( fRowWidth > fWidth )
			fWidth = fRowWidth;
	}

	pSize->cx = ( int ) fWidth;
	pSize->cy = ( int ) fHeight;

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws 2D text. Note that sx and sy are in pixels
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DrawText ( FLOAT sx, FLOAT sy, DWORD dwColor,
							 const CHAR* strText, DWORD dwFlags )
{
	if ( m_pd3dDevice == NULL )
		return E_FAIL;

	// Setup renderstate
	if(FAILED(CD3DState::BeginState ()))
		return E_FAIL;

	m_pd3dDevice->SetFVF ( D3DFVF_FONT2DVERTEX );
	m_pd3dDevice->SetPixelShader ( NULL );
	m_pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof ( FONT2DVERTEX ) );

	// Set filter states
	if ( dwFlags & D3DFONT_FILTERED )
	{
		m_pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	}

	// Center the text block in the viewport
	if ( dwFlags & D3DFONT_CENTERED_X )
	{
		SIZE pSize;
		GetTextExtent ( strText, &pSize );
		sx -= ( FLOAT ) pSize.cx / 2.0f;
	}
	if ( dwFlags & D3DFONT_CENTERED_Y )
	{
		SIZE sz;
		GetTextExtent ( strText, &sz );
		sy -= ( FLOAT ) sz.cy / 2.0f;
	}

	// Adjust for character spacing
	sx -= m_dwSpacing;
	FLOAT fStartX = sx;

	// Fill vertex buffer
	FONT2DVERTEX* pVertices = NULL;
	DWORD         dwNumTriangles = 0;
	m_pVB->Lock ( 0, 0, ( void** ) &pVertices, D3DLOCK_DISCARD );

	int len = strlen ( strText );
	DWORD strStart = ( DWORD ) strText;
	DWORD dwCustomColor = dwColor;

	while ( *strText )
	{
		CHAR c = *strText++;
		DWORD strCur = ( DWORD ) strText;

		if ( c == '{' )
		{
			std::vector<char> _costumColor;
			while ( *strText != '}' )
			{
				char ch = toupper ( *strText );
				bool numeric = ( ch >= '0' && ch <= '9' ) || ( ch >= 'A' && ch <= 'F' );
				if ( numeric )
				{
					_costumColor.push_back ( ch );
				}
				strText++;
			}
			sscanf_s ( _costumColor.data (), "%X", &dwCustomColor );
			dwCustomColor |= ( dwColor >> 24 ) << 24;
			c = *strText++;
			continue;
		}

		if ( c == '\n' || 
			 sx >= m_fWidth )
		{
			sx = fStartX;
			sy += ( m_fTexCoords [ 0 ] [ 3 ] - m_fTexCoords [ 0 ] [ 1 ] )*m_dwTexHeight;
		}

		if ( ( c - 32 ) < 0 || ( c - 32 ) >= 128 - 32 )
			continue;

		FLOAT tx1 = m_fTexCoords [ c - 32 ] [ 0 ];
		FLOAT ty1 = m_fTexCoords [ c - 32 ] [ 1 ];
		FLOAT tx2 = m_fTexCoords [ c - 32 ] [ 2 ];
		FLOAT ty2 = m_fTexCoords [ c - 32 ] [ 3 ];

		FLOAT w = ( tx2 - tx1 ) *  m_dwTexWidth / m_fTextScale;
		FLOAT h = ( ty2 - ty1 ) * m_dwTexHeight / m_fTextScale;

		if ( c != ' ' )
		{
			if ( dwFlags & D3DFONT_COLORTABLE )
				dwColor = dwCustomColor;

			if ( dwFlags & D3DFONT_BORDER )
			{
				float _x = sx, _y = sy;
				float addy = 0.5f;
				float fHeight = h, fWidth = w;

				DWORD dwShadow = 0xFF000000;

				_x = sx - addy, _y = sy - addy;
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx1, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.0f, 0.0f ), dwShadow, tx1, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y, 0.0f, 0.0f ), dwShadow, tx2, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.0f, 0.0f ), dwShadow, tx1, ty1 );


				_x = sx + addy, _y = sy + addy;
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx1, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.0f, 0.0f ), dwShadow, tx1, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y, 0.0f, 0.0f ), dwShadow, tx2, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.0f, 0.0f ), dwShadow, tx1, ty1 );

				_x = sx - addy, _y = sy + addy;
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx1, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.0f, 0.0f ), dwShadow, tx1, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y, 0.0f, 0.0f ), dwShadow, tx2, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.0f, 0.0f ), dwShadow, tx1, ty1 );

				_x = sx + addy, _y = sy - addy;
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx1, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.0f, 0.0f ), dwShadow, tx1, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y, 0.0f, 0.0f ), dwShadow, tx2, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.0f, 0.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.0f, 0.0f ), dwShadow, tx1, ty1 );


				dwNumTriangles += 8;
			}
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + 0 - 0.5f, sy + h - 0.5f, 0.9f, 1.0f ), dwColor, tx1, ty2 );
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + 0 - 0.5f, sy + 0 - 0.5f, 0.9f, 1.0f ), dwColor, tx1, ty1 );
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + w - 0.5f, sy + h - 0.5f, 0.9f, 1.0f ), dwColor, tx2, ty2 );
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + w - 0.5f, sy + 0 - 0.5f, 0.9f, 1.0f ), dwColor, tx2, ty1 );
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + w - 0.5f, sy + h - 0.5f, 0.9f, 1.0f ), dwColor, tx2, ty2 );
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + 0 - 0.5f, sy + 0 - 0.5f, 0.9f, 1.0f ), dwColor, tx1, ty1 );
			dwNumTriangles += 2;

			if ( dwNumTriangles * 3 > ( MAX_NUM_VERTICES - 6 ) )
			{
				// Unlock, render, and relock the vertex buffer
				m_pVB->Unlock ();
				m_pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
				pVertices = NULL;
				m_pVB->Lock ( 0, 0, ( void** ) &pVertices, D3DLOCK_DISCARD );
				dwNumTriangles = 0L;
			}
		}

		sx += w - ( 2 * m_dwSpacing );
	}

	// Unlock and render the vertex buffer
	m_pVB->Unlock ();
	if ( dwNumTriangles > 0 )
		m_pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

	// Restore the modified renderstates
	CD3DState::EndState ();

	return S_OK;
}

CD3DTexture::CD3DTexture ( IDirect3DDevice9* pd3dDevice, const char *szPath )
{
	D3DXCreateSprite ( pd3dDevice, &m_pSprite );
	D3DXCreateTextureFromFileA ( pd3dDevice, szPath, &m_pTexture );

	m_szPath = new char [ strlen ( szPath ) ];
	strcpy ( m_szPath, szPath );

	m_pDevice = pd3dDevice;
}

CD3DTexture::~CD3DTexture ( void )
{
	SAFE_RELEASE ( m_pSprite );
	SAFE_RELEASE ( m_pTexture );

	delete [] m_szPath;
	m_szPath = NULL;
}

void CD3DTexture::Draw ( float fX, float fY,
						 float fScaleX, float fScaleY, float fRotation, D3DCOLOR d3dColor )
{
	if ( !m_pTexture ||
		 !m_pSprite )
	{
		return;
	}

	D3DSURFACE_DESC dTextureDesc;
	m_pTexture->GetLevelDesc ( 0, &dTextureDesc );

	D3DXVECTOR2 vScaling ( 1.f, 1.f );

	if ( fScaleX >= 0.f &&
		 dTextureDesc.Width > 0.f )
	{
		vScaling.x = vScaling.x * fScaleX / dTextureDesc.Width;
	}

	if ( fScaleY >= 0.f &&
		 dTextureDesc.Height > 0.f )
	{
		vScaling.y = vScaling.y * fScaleY / dTextureDesc.Height;
	}

	D3DXMATRIX mMatrix;

	D3DXVECTOR2 vTrans = D3DXVECTOR2 ( fX, fY );
	D3DXVECTOR2 vCenter = D3DXVECTOR2 ( vScaling.x * fScaleX / dTextureDesc.Width, vScaling.y * fScaleY / dTextureDesc.Height );
	D3DXMatrixTransformation2D ( &mMatrix, NULL, NULL, &vScaling, &vCenter, fRotation / ( 180.f / D3DX_PI ), &vTrans );

	m_pSprite->Begin ( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE );
	m_pSprite->SetTransform ( &mMatrix );
	m_pSprite->Draw ( m_pTexture, NULL, NULL, NULL, d3dColor );
	m_pSprite->End ( );
}

void CD3DTexture::OnLostDevice ( )
{
	if ( m_pSprite )
		m_pSprite->OnLostDevice ( );

	SAFE_RELEASE ( m_pTexture );
}

void CD3DTexture::OnResetDevice ( )
{
	if ( m_pSprite )
		m_pSprite->OnResetDevice ( );

	if ( m_pDevice &&
		 !m_pTexture )
	{
		D3DXCreateTextureFromFileA ( m_pDevice, m_szPath, &m_pTexture );
	}
}

CD3DRender::CD3DRender ( int numVertices )
{
	m_canRender = false;

	m_pD3Dbuf = NULL;
	m_pVertex = NULL;

	m_color = 0;
	m_tu = 0.0f;
	m_tv = 0.0f;

	m_maxVertex = numVertices;
	m_curVertex = 0;
}

CD3DRender::~CD3DRender ( )
{
	this->Invalidate ( ); 
	CD3DState::Invalidate ();
}

HRESULT CD3DRender::Initialize ( IDirect3DDevice9 *pD3Ddevevice )
{
	if ( !m_canRender )
	{
		CD3DState::Initialize ( pD3Ddevevice );

		if ( FAILED ( m_pd3dDevice->CreateVertexBuffer ( m_maxVertex * sizeof ( LVERTEX ),
														 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pD3Dbuf, NULL ) ) )
			return E_FAIL;

		m_canRender = true;
	}

	return S_OK;
}

HRESULT CD3DRender::Invalidate ( )
{
	SAFE_RELEASE ( m_pD3Dbuf );
	m_pVertex = NULL;
	
	m_canRender = false;

	return S_OK;
}

HRESULT CD3DRender::BeginRender ( D3DPRIMITIVETYPE primType )
{
	if ( !m_canRender )
		return E_FAIL;

	if ( m_pVertex )
		return E_FAIL;

	if ( FAILED ( m_pD3Dbuf->Lock ( 0, 0, ( void ** ) &m_pVertex, D3DLOCK_DISCARD ) ) )
		return E_FAIL;

	m_primType = primType;

	if ( FAILED ( CD3DState::BeginState ( ) ) )
		return E_FAIL;

	return S_OK;
}

HRESULT CD3DRender::EndRender ()
{
	int numPrims;

	m_pVertex = NULL;

	if ( !m_canRender )
	{
		m_curVertex = 0;
		return E_FAIL;
	}

	switch ( m_primType )
	{
		case D3DPT_POINTLIST:
			numPrims = m_curVertex;
			break;

		case D3DPT_LINELIST:
			numPrims = m_curVertex / 2;
			break;

		case D3DPT_LINESTRIP:
			numPrims = m_curVertex - 1;
			break;

		case D3DPT_TRIANGLELIST:
			numPrims = m_curVertex / 3;
			break;

		case D3DPT_TRIANGLESTRIP:
		case D3DPT_TRIANGLEFAN:
			numPrims = m_curVertex - 2;
			break;

		default:
			numPrims = 0;
			break;
	}

	m_curVertex = 0;

	if ( numPrims > 0 )
	{
		m_pD3Dbuf->Unlock ();

		m_pd3dDevice->SetFVF ( D3DFVF_PRIMITIVES );
		m_pd3dDevice->SetTexture ( 0, NULL );

		m_pd3dDevice->SetStreamSource ( 0, m_pD3Dbuf, 0, sizeof ( LVERTEX ) );
		m_pd3dDevice->DrawPrimitive ( m_primType, 0, numPrims );
	}

	CD3DState::EndState ();

	return S_OK;
}

void CD3DRender::D3DColor ( DWORD color )
{
	m_color = color;
}

void CD3DRender::D3DTexCoord2f ( float u, float v )
{
	m_tu = u;
	m_tv = v;
}

HRESULT CD3DRender::D3DVertex2f ( float x, float y )
{
	if ( m_canRender &&
		 m_pVertex &&
		 ( ++m_curVertex < m_maxVertex ) )
	{
		*m_pVertex++ = Init2DVertex ( x, y, m_color, m_tu, m_tv );
	}
	else
		return E_FAIL;

	return S_OK;
}

void CD3DRender::D3DTriangle ( float fX, float fY, float fSize, float fAngle,
							   D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor, bool bAntAlias )
{
	const int iVertexSize = 4;
	D3DXVECTOR2 vVector [ iVertexSize ];

	vVector [ 0 ].x = fX;
	vVector [ 0 ].y = fY - fSize;
	vVector [ 1 ].x = fX + fSize;
	vVector [ 1 ].y = fY;
	vVector [ 2 ].x = fX - fSize;
	vVector [ 2 ].y = fY;
	vVector [ 3 ].x = fX;
	vVector [ 3 ].y = fY - fSize;

	RotateVerts ( vVector, iVertexSize, fX, fY, fAngle );

	if ( SUCCEEDED ( BeginRender ( D3DPT_TRIANGLEFAN ) ) )
	{
		D3DColor ( d3dColor );

		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		for ( size_t i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ( );
	}

	if ( SUCCEEDED ( BeginRender ( D3DPT_LINESTRIP ) ) )
	{
		D3DColor ( d3dOutlineColor );

		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		for ( size_t i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ( );
	}
}

void CD3DRender::D3DCircle ( float fX, float fY, float fSize,
							 D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor, bool bAntAlias )
{
	const int iVertexSize = 100;
	D3DXVECTOR2 vVector [ iVertexSize ];

	for ( int i = 0; i < iVertexSize; i++ )
	{
		vVector [ i ].x = fX + cos ( D3DX_PI / 180 ) * ( ( fX - fSize * cos ( D3DX_PI * ( i / ( iVertexSize / 2.0f ) ) ) ) - fX ) - sin ( D3DX_PI / 180 ) * ( ( fY - fSize * sin ( D3DX_PI * ( i / ( iVertexSize / 2.0f ) ) ) ) - fY );
		vVector [ i ].y = fY + sin ( D3DX_PI / 180 ) * ( ( fX - fSize * cos ( D3DX_PI * ( i / ( iVertexSize / 2.0f ) ) ) ) - fX ) + cos ( D3DX_PI / 180 ) * ( ( fY - fSize * sin ( D3DX_PI * ( i / ( iVertexSize / 2.0f ) ) ) ) - fY );
	}

	if ( SUCCEEDED ( BeginRender ( D3DPT_TRIANGLEFAN ) ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		D3DColor ( d3dColor );
		for ( int i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ( );
	}

	if ( SUCCEEDED ( BeginRender ( D3DPT_LINESTRIP ) ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		D3DColor ( d3dOutlineColor );
		for ( int i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ( );
	}
}

void CD3DRender::D3DBox ( float fX, float fY,
						  float fWidth, float fHeight, float fAngle,
						  D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor, bool bAntAlias )
{
	const int iVertexSize = 6;
	D3DXVECTOR2 vVector [ iVertexSize ];

	vVector [ 0 ].x = fX;
	vVector [ 0 ].y = fY;
	vVector [ 1 ].x = fX + fWidth;
	vVector [ 1 ].y = fY;
	vVector [ 2 ].x = fX + fWidth;
	vVector [ 2 ].y = fY + fHeight;
	vVector [ 3 ].x = fX;
	vVector [ 3 ].y = fY + fHeight;
	vVector [ 4 ].x = fX;
	vVector [ 4 ].y = fY;
	vVector [ 5 ].x = fX;
	vVector [ 5 ].y = fY + fHeight;

	if ( fAngle > 0.f )
		RotateVerts ( vVector, iVertexSize, fX, fY, fAngle );

	if ( SUCCEEDED ( BeginRender ( D3DPT_TRIANGLEFAN ) ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );
		
		D3DColor ( d3dColor );
		for ( size_t i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ( );
	}

	if ( SUCCEEDED ( BeginRender ( D3DPT_LINESTRIP ) ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		D3DColor ( d3dOutlineColor );
		for ( size_t i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ( );
	}
}

void CD3DRender::D3DLine ( float fStartX, float fStartY, float fEndX, float fEndY,
						   D3DCOLOR d3dColor, bool bAntAlias )
{
	if ( SUCCEEDED ( BeginRender ( D3DPT_LINELIST ) ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		D3DColor ( d3dColor );
		D3DVertex2f ( fStartX, fStartY );
		D3DVertex2f ( fEndX, fEndY );
		EndRender ( );
	}
}