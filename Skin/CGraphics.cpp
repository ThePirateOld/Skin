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

CD3DFont::CD3DFont ( const char *pszFontName, int iFontHeight, DWORD dwCreateFlags )
{
	strcpy_s ( m_szFontName, 32, ( pszFontName ? pszFontName : "Arial" ) );

	m_fontHeight = iFontHeight;
	m_dwCreateFlags = dwCreateFlags;

	m_isReady = m_statesOK = false;

	m_pD3Dtex = NULL;
	m_pD3Dbuf = NULL;

	m_maxTriangles = 255 * 2;
}

CD3DFont::~CD3DFont ( )
{
	Invalidate ( );
}

HRESULT CD3DFont::Initialize ( IDirect3DDevice9 * pD3Ddev )
{
	CD3DState::Initialize ( pD3Ddev );

	m_texWidth = m_texHeight = 1024;

	if ( FAILED ( m_pd3dDevice->CreateTexture ( m_texWidth, m_texHeight, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &m_pD3Dtex, NULL ) ) )
	{
		return E_FAIL;
	}

	if ( FAILED ( m_pd3dDevice->CreateVertexBuffer ( m_maxTriangles * 3 * sizeof ( FONTVERTEX ), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pD3Dbuf, NULL ) ) )
	{
		SAFE_RELEASE ( m_pD3Dtex );
		return E_FAIL;
	}

	DWORD *pBitmapBits = 0;
	BITMAPINFO bmi;

	ZeroMemory ( &bmi.bmiHeader, sizeof ( BITMAPINFOHEADER ) );
	bmi.bmiHeader.biSize = sizeof ( BITMAPINFOHEADER );
	bmi.bmiHeader.biWidth = m_texWidth;
	bmi.bmiHeader.biHeight = -m_texHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 32;

	HDC hDC = CreateCompatibleDC ( 0 );
	HBITMAP hbmBitmap = CreateDIBSection ( hDC, &bmi, DIB_RGB_COLORS, ( void** ) &pBitmapBits, NULL, 0 );
	SetMapMode ( hDC, MM_TEXT );

	HFONT hFont = CreateFontA ( -MulDiv ( m_fontHeight, GetDeviceCaps ( hDC, LOGPIXELSY ), 72 ), 0, 0, 0,  m_dwCreateFlags, m_dwCreateFlags&FCR_ITALICS, false, false,
								ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
								VARIABLE_PITCH, m_szFontName );

	if ( hFont == NULL )
	{
		return E_FAIL;
	}

	SelectObject ( hDC, hbmBitmap );
	SelectObject ( hDC, hFont );

	SetTextColor ( hDC, 0x00FFFFFF );
	SetBkColor ( hDC, 0 );
	SetTextAlign ( hDC, TA_TOP );

	int x, y = 0;
	char szStr [ 2 ] = { ' ' , 0 };
	SIZE size;

	GetTextExtentPoint32A ( hDC, szStr, 1, &size );
	x = m_chrSpacing = static_cast<int>( ceil ( size.cx / 4.0f ) );
	m_fChrHeight = static_cast<float>( size.cy - 1 );

	for ( int c = 32; c < 255; c++ )
	{
		szStr [ 0 ] = btowc ( c );

		if ( !GetTextExtentPoint32A ( hDC, szStr, 1, &size ) )
			MessageBoxA ( 0, szStr, "GetTextExtentPoint32", 0 );

		if ( x + size.cx + m_chrSpacing > m_texWidth )
		{
			x = m_chrSpacing;
			y += size.cy + 1;
		}

		RECT rect = { x, y, x + size.cx, y + size.cy };
		if ( !ExtTextOutA ( hDC, x, y, ETO_CLIPPED, &rect, szStr, 1, 0 ) )
			MessageBoxA ( 0, szStr, "ExtTextOut", 0 );

		//tu src + dst
		m_fTexCoords [ c - 32 ] [ 0 ] = static_cast<float>( ( x + 0 - m_chrSpacing ) ) / static_cast<float>( m_texWidth );
		m_fTexCoords [ c - 32 ] [ 2 ] = static_cast<float>( ( x + size.cx + m_chrSpacing ) ) / static_cast<float>( m_texWidth );

		//tv src + dst
		m_fTexCoords [ c - 32 ] [ 1 ] = static_cast<float>( ( y + 0 + 0 ) ) / static_cast<float>( m_texHeight );
		m_fTexCoords [ c - 32 ] [ 3 ] = static_cast<float>( ( y + size.cy + 0 ) ) / static_cast<float>( m_texHeight );

		x += size.cx + ( 2 * m_chrSpacing );
	}

	D3DLOCKED_RECT d3dlr;
	m_pD3Dtex->LockRect ( 0, &d3dlr, 0, 0 );

	BYTE * pDstRow = reinterpret_cast<BYTE*>( d3dlr.pBits );
	WORD * pDst16 = 0;
	BYTE bAlpha = 0;

	for ( y = 0; y < m_texHeight; y++ )
	{
		pDst16 = reinterpret_cast<WORD*>( pDstRow );

		for ( x = 0; x < m_texWidth; x++ )
		{
			bAlpha = static_cast<BYTE>( ( pBitmapBits [ m_texWidth * y + x ] >> 8 ) ) & 0xFF;
			*pDst16 = static_cast<WORD>( ( bAlpha << 8 ) ) | 0x0FFF;
			pDst16++;
		}

		pDstRow += d3dlr.Pitch;
	}
	m_pD3Dtex->UnlockRect ( 0 );

	DeleteObject ( hbmBitmap );
	DeleteDC ( hDC );
	DeleteObject ( hFont );

	m_isReady = true;

	return S_OK;
}

HRESULT CD3DFont::Invalidate ( )
{
	CD3DState::Invalidate ( );
	m_isReady = false;

	SAFE_RELEASE ( m_pD3Dtex );
	SAFE_RELEASE ( m_pD3Dbuf );

	return S_OK;
}

void CD3DFont::CutString ( int iMaxWidth, std::string & sString )
{
	int iIndex = 0, iLength = sString.length ( );

	for ( int iWidth = 0; iIndex < iLength && iWidth + 10 < iMaxWidth; )
	{
		char szCurrent [ 2 ] = { sString.c_str ( ) [ iIndex ], 0 };
		iWidth += GetWidth ( szCurrent );
		iIndex++;
	}

	if ( iIndex < iLength )
		sString [ iIndex - 1 ] = '\0';
}

HRESULT CD3DFont::PrintShadow ( float x, float y,
								DWORD colour, DWORD dwFlags, const char *szText, ... )
{
	char szBuffer [ 1024 ];
	va_list ap;
	va_start ( ap, szText );
	vsnprintf ( szBuffer, 1024, szText, ap );
	va_end ( ap );

	HRESULT hr;
	hr |= Print ( x + 1.f, y + 1.f, D3DCOLOR_RGBA ( 0, 0, 0, 255 ), szBuffer, 0 );
	hr |= Print ( x, y, colour, szBuffer, 0 );

	return hr;
}

HRESULT CD3DFont::Print ( float x, float y,
						  DWORD colour, const char *szText, DWORD dwFlags )
{
	if ( !m_isReady )
	{
		return E_FAIL;
	}

	int strWidth = GetWidth ( szText );

	if ( dwFlags & FT_CENTER )
		x -= strWidth / 2.0f;

	if ( dwFlags & FT_VCENTER )
		y -= GetHeight ( ) / 2;

	x -= static_cast< float >( m_chrSpacing );

	if ( FAILED ( CD3DState::BeginState ( ) ) )
	{
		return E_FAIL;
	}

	m_pd3dDevice->SetFVF ( D3DFVF_BITMAPFONT );
	m_pd3dDevice->SetTexture ( 0, m_pD3Dtex );
	m_pd3dDevice->SetStreamSource ( 0, m_pD3Dbuf, 0, sizeof ( FONTVERTEX ) );

	if ( *szText != '\0' )
	{
		UINT usedTriangles = 0;
		FONTVERTEX *pVertex;

		if ( FAILED ( m_pD3Dbuf->Lock ( 0, 0, reinterpret_cast< void** >( &pVertex ), D3DLOCK_DISCARD ) ) )
		{
			return E_FAIL;
		}

		int iLen = static_cast<int>( strlen ( szText ) );
		for ( int i = 0; i < iLen; i++ )
		{
			int c = ( szText [ i ] & 0xFF ) - 32;

			float tx1 = m_fTexCoords [ c ] [ 0 ];
			float tx2 = m_fTexCoords [ c ] [ 2 ];
			float ty1 = m_fTexCoords [ c ] [ 1 ];
			float ty2 = m_fTexCoords [ c ] [ 3 ];

			float w = ( tx2 - tx1 ) * m_texWidth;
			float h = ( ty2 - ty1 ) * m_texHeight;

			*pVertex++ = InitFont2DVertex ( x - 0.5f, y - 0.5f, colour, tx1, ty1 ); //topleft
			*pVertex++ = InitFont2DVertex ( x + w - 0.5f, y - 0.5f, colour, tx2, ty1 ); //topright
			*pVertex++ = InitFont2DVertex ( x - 0.5f, y + h - 0.5f, colour, tx1, ty2 ); //bottomleft

			*pVertex++ = InitFont2DVertex ( x + w - 0.5f, y - 0.5f, colour, tx2, ty1 ); //topright
			*pVertex++ = InitFont2DVertex ( x + w - 0.5f, y + h - 0.5f, colour, tx2, ty2 ); //bottomright
			*pVertex++ = InitFont2DVertex ( x - 0.5f, y + h - 0.5f, colour, tx1, ty2 ); //bottomleft

			x += w - ( m_chrSpacing * 2 );

			usedTriangles += 2;
			if ( usedTriangles >= m_maxTriangles )
			{
				break;
			}
		}

		if ( usedTriangles > 0 )
		{
			m_pD3Dbuf->Unlock ( );
			m_pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLELIST, 0, usedTriangles );
		}
	}

	CD3DState::EndState ( );

	return S_OK;
}

int CD3DFont::GetWidth ( const char *szText )
{
	float fLen = 0.0f;
	for ( int i = strlen ( szText ) - 1; i >= 0; i-- )
	{
		int iChar = ( szText [ i ] & 0xFF ) - 32;
		if ( iChar >= 0 && iChar < 255 )
			fLen += ( ( m_fTexCoords [ iChar ] [ 2 ] - m_fTexCoords [ iChar ] [ 0 ] ) * m_texWidth ) - m_chrSpacing * 2;
	}

	return static_cast< int >( ceilf ( fLen ) );
}

int CD3DFont::GetHeight ( void )
{
	if ( m_fChrHeight <= 0.f )
		Print ( 0, 0, 0, "Y", 0L );

	return static_cast< int >( m_fChrHeight );
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