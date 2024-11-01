/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// hud_redraw.cpp
//
#include <math.h>
#include "hud.h"
#include "cl_util.h"
#include "triangleapi.h"

#include <string>
#include <cstdarg>
#include <draw_util.h>

#define MAX_LOGO_FRAMES 56

#define DHN_DRAWZERO 1
#define DHN_2DIGITS 2
#define DHN_3DIGITS 4
#define MIN_ALPHA 100

int grgLogoFrame[MAX_LOGO_FRAMES] = 
{
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13, 13, 13, 13, 13, 12, 11, 10, 9, 8, 14, 15,
	16, 17, 18, 19, 20, 20, 20, 20, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
	29, 29, 29, 29, 29, 28, 27, 26, 25, 24, 30, 31 
};



extern int g_iVisibleMouse;

float HUD_GetFOV( void );

// Think
void CHud::Think(void)
{
	int newfov;

	for( HUDLIST *pList = m_pHudList; pList; pList = pList->pNext )
	{
		if( pList->p->m_iFlags & HUD_THINK )
			pList->p->Think();
	}

	newfov = HUD_GetFOV();
	m_iFOV = newfov ? newfov : default_fov->value;

	// the clients fov is actually set in the client data update section of the hud

	// Set a new sensitivity
	if ( m_iFOV == default_fov->value )
	{  
		// reset to saved sensitivity
		m_flMouseSensitivity = 0;
	}
	else
	{  
		// set a new sensitivity that is proportional to the change from the FOV default
		m_flMouseSensitivity = sensitivity->value * ((float)newfov / (float)default_fov->value) * zoom_sens_ratio->value;
	}

	// think about default fov
	if ( m_iFOV == 0 )
	{  // only let players adjust up in fov,  and only if they are not overriden by something else
		m_iFOV = max( default_fov->value, 90 );
	}

}

// Redraw
// step through the local data,  placing the appropriate graphics & text as appropriate
// returns 1 if they've changed, 0 otherwise
int CHud :: Redraw( float flTime, int intermission )
{
	m_fOldTime = m_flTime;	// save time of previous redraw
	m_flTime = flTime;
	m_flTimeDelta = (double)m_flTime - m_fOldTime;
	static int m_flShotTime = 0;

	m_WinImage.Think( flTime );
	m_SoundManager.Update( flTime );
#ifdef __ANDROID__
	if( cl_android_force_defaults && cl_android_force_defaults->value )
	{
		if( cl_lw && !cl_lw->value )
		{
			ConsolePrint( "cl_lw is forced to 1. Set cl_android_force_defaults to 0, if you want to disable this behaviour" );
			gEngfuncs.Cvar_SetValue( "cl_lw", 1.0f );
		}
		if( cl_predict && !cl_predict->value )
		{
			ConsolePrint( "cl_predict is forced to 1. Set cl_android_force_defaults to 0, if you want to disable this behaviour" );
			gEngfuncs.Cvar_SetValue( "cl_predict", 1.0f );
		}
		if( sv_skipshield && !sv_skipshield->value )
		{
			ConsolePrint( "sv_skipshield is forced to 1. Set cl_android_force_defaults to 0, if you want to disable this behaviour" );
			gEngfuncs.Cvar_SetValue( "sv_skipshield", 1.0f );
		}
	}
#endif
	
	// Clock was reset, reset delta
	if ( m_flTimeDelta < 0 )
		m_flTimeDelta = 0;


	if (m_flShotTime && m_flShotTime < flTime)
	{
		gEngfuncs.pfnClientCmd("snapshot\n");
		m_flShotTime = 0;
	}

	m_iIntermission = intermission;

	if ( m_pCvarDraw->value && (intermission || !(m_iHideHUDDisplay & HIDEHUD_ALL) ) )
	{
		for( HUDLIST *pList = m_pHudList; pList; pList = pList->pNext )
		{
			if( pList->p->m_iFlags & HUD_DRAW )
			{
				if( intermission && !(pList->p->m_iFlags & HUD_INTERMISSION) )
					continue; // skip no-intermission during intermission

				pList->p->Draw( flTime );
			}
		}
	}

	// are we in demo mode? do we need to draw the logo in the top corner?
	if (m_iLogo)
	{
		int x, y, i;

		if (m_hsprLogo == 0)
			m_hsprLogo = LoadSprite("sprites/%d_logo.spr");

		SPR_Set(m_hsprLogo, 250, 250, 250 );
		
		x = SPR_Width(m_hsprLogo, 0);
		x = ScreenWidth - x;
		y = SPR_Height(m_hsprLogo, 0)/2;

		// Draw the logo at 20 fps
		int iFrame = (int)(flTime * 20) % MAX_LOGO_FRAMES;
		i = grgLogoFrame[iFrame] - 1;

		SPR_DrawAdditive(i, x, y, NULL);

	}


	return 1;
}




void ScaleColors( int &r, int &g, int &b, int a )
{
	float x = (float)a / 255;
	r       = (int)( r * x );
	g       = (int)( g * x );
	b       = (int)( b * x );
}

//// OpenAG
//void CHud::UpdateDefaultHUDColor( )
//{
//	int r, g, b;
//
//	if ( sscanf( m_pCvarColor->string, "%d %d %d", &r, &g, &b ) == 3 )
//	{
//		r = Q_max( r, 0 );
//		g = Q_max( g, 0 );
//		b = Q_max( b, 0 );
//
//		r = Q_min( r, 255 );
//		g = Q_min( g, 255 );
//		b = Q_min( b, 255 );
//
//		m_iDefaultHUDColor = ( r << 16 ) | ( g << 8 ) | b;
//	}
//	else
//	{
//		m_iDefaultHUDColor = RGB_YELLOWISH;
//	}
//}

const unsigned char colors[8][3] =
    {
        { 127, 127, 127 }, // additive cannot be black
        { 255, 0, 0 },
        { 0, 255, 0 },
        { 255, 255, 0 },
        { 0, 0, 255 },
        { 0, 255, 255 },
        { 255, 0, 255 },
        { 240, 180, 24 } };

int CHud::DrawHudString( int xpos, int ypos, int iMaxX, const char *szIt, int r, int g, int b )
{
	return xpos + gEngfuncs.pfnDrawString( xpos, ypos, szIt, r, g, b );
}


void CHud::DrawStringConsole( int x, int y, int r, int g, int b, const char *fmt, ... )
{
	va_list va_alist;
	char buf[256];
	va_start( va_alist, fmt );
	vsnprintf( buf, sizeof( buf ), fmt, va_alist );
	va_end( va_alist );
	gEngfuncs.pfnDrawSetTextColor( (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f );
	gEngfuncs.pfnDrawConsoleString( x, y, buf );
}

int CHud::DrawChar( int xpos, int ypos, int iMaxX, const char *szIt, int r, int g, int b )
{
	// xash3d: reset unicode state
	gEngfuncs.pfnDrawCharacter( 0, 0, 0, 0, 0, 0 );

	// draw the string until we hit the null character or a newline character
	for ( ; *szIt != 0 && *szIt != '\n'; szIt++ )
	{
		int w = gHUD.m_scrinfo.charWidths['M'];
		if ( xpos + w > iMaxX )
			return xpos;
		if ( ( *szIt == '^' ) && ( *( szIt + 1 ) >= '0' ) && ( *( szIt + 1 ) <= '7' ) )
		{
			szIt++;
			r = colors[*szIt - '0'][0];
			g = colors[*szIt - '0'][1];
			b = colors[*szIt - '0'][2];
			if ( !*( ++szIt ) )
				return xpos;
		}
		int c = (unsigned int)(unsigned char)*szIt;


		xpos += gEngfuncs.pfnDrawCharacter( xpos, ypos, c, r, g, b );
	}

	return xpos;
}

int CHud::DrawCharReverse( int xpos, int ypos, int iMinX, const char *szString, int r, int g, int b )
{
	// find the end of the string
	for ( const char *szIt = szString; *szIt != 0; szIt++ )
		xpos -= gHUD.m_scrinfo.charWidths[(unsigned char)*szIt];
	if ( xpos < iMinX )
		xpos = iMinX;
	DrawChar( xpos, ypos, gHUD.m_scrinfo.iWidth, szString, r, g, b );
	return xpos;
}

int CHud::DrawHudStringLen( const char *szIt )
{
	int l = 0;
	for ( ; *szIt != 0 && *szIt != '\n'; szIt++ )
	{
		l += gHUD.m_scrinfo.charWidths[(unsigned char)*szIt];
	}
	return l;
}

int CHud::DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b )
{
	char szString[32];
	sprintf( szString, "%d", iNumber );
	return DrawHudStringReverse( xpos, ypos, iMinX, szString, r, g, b );
}

// draws a string from right to left (right-aligned)
int CHud::DrawHudStringReverse( int xpos, int ypos, int iMinX, const char *szString, int r, int g, int b )
{
	// find the end of the string
	for ( const char *szIt = szString; *szIt != 0; szIt++ )
		xpos -= gHUD.m_scrinfo.charWidths[(unsigned char)*szIt];
	if ( xpos < iMinX )
		xpos = iMinX;
	DrawHudString( xpos, ypos, gHUD.m_scrinfo.iWidth, szString, r, g, b );
	return xpos;
}

int CHud::DrawHudNumber( int x, int y, int iFlags, int iNumber, int r, int g, int b )
{
	int iWidth = GetSpriteRect( m_HUD_number_0 ).right - GetSpriteRect( m_HUD_number_0 ).left;
	int k;

	if ( iNumber > 0 )
	{
		// SPR_Draw 100's
		if ( iNumber >= 100 )
		{
			k = iNumber / 100;
			SPR_Set( GetSprite( m_HUD_number_0 + k ), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect( m_HUD_number_0 + k ) );
			x += iWidth;
		}
		else if ( iFlags & ( DHN_3DIGITS ) )
		{
			// SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw 10's
		if ( iNumber >= 10 )
		{
			k = ( iNumber % 100 ) / 10;
			SPR_Set( GetSprite( m_HUD_number_0 + k ), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect( m_HUD_number_0 + k ) );
			x += iWidth;
		}
		else if ( iFlags & ( DHN_3DIGITS | DHN_2DIGITS ) )
		{
			// SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		k = iNumber % 10;
		SPR_Set( GetSprite( m_HUD_number_0 + k ), r, g, b );
		SPR_DrawAdditive( 0, x, y, &GetSpriteRect( m_HUD_number_0 + k ) );
		x += iWidth;
	}
	else if ( iFlags & DHN_DRAWZERO )
	{
		SPR_Set( GetSprite( m_HUD_number_0 ), r, g, b );

		// SPR_Draw 100's
		if ( iFlags & ( DHN_3DIGITS ) )
		{
			// SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		if ( iFlags & ( DHN_3DIGITS | DHN_2DIGITS ) )
		{
			// SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		SPR_DrawAdditive( 0, x, y, &GetSpriteRect( m_HUD_number_0 ) );
		x += iWidth;
	}

	return x;
}

static constexpr int s_TenPowers[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000 };

int CHud::DrawHudNumber( int x, int y, int iNumber, int r, int g, int b )
{
	int iWidth = GetSpriteRect( m_HUD_number_0 ).right - GetSpriteRect( m_HUD_number_0 ).left;
	int iCount = iNumber > 9 ? (int)log10( (double)iNumber ) + 1 : 1;

	for ( int i = iCount; i > 0; --i )
	{
		int digit = iNumber / s_TenPowers[i - 1];

		SPR_Set( GetSprite( m_HUD_number_0 + digit ), r, g, b );
		SPR_DrawAdditive( 0, x, y, &GetSpriteRect( m_HUD_number_0 + digit ) );
		x += iWidth;

		iNumber -= digit * s_TenPowers[i - 1];
	}

	return x;
}

int CHud::DrawHudNumberCentered( int x, int y, int iNumber, int r, int g, int b )
{
	int iWidth = GetSpriteRect( m_HUD_number_0 ).right - GetSpriteRect( m_HUD_number_0 ).left;
	int iCount = iNumber > 9 ? (int)log10( (double)iNumber ) + 1 : 1;
	int totalWidth = iWidth * iCount;

	int startX = x - ( totalWidth / 2 );

	return DrawHudNumber( startX - ( iWidth * iCount ) / 2, y, iNumber, r, g, b );
}

int CHud::GetNumWidth( int iNumber, int iFlags )
{
	if ( iFlags & ( DHN_3DIGITS ) )
		return 3;

	if ( iFlags & ( DHN_2DIGITS ) )
		return 2;

	if ( iNumber <= 0 )
	{
		if ( iFlags & ( DHN_DRAWZERO ) )
			return 1;
		else
			return 0;
	}

	if ( iNumber < 10 )
		return 1;

	if ( iNumber < 100 )
		return 2;

	return 3;
}

int CHud::GetHudStringWidth( const char *string )
{
	return gEngfuncs.pfnDrawString( 0, 0, string, 0, 0, 0 );
}

void CHud::DrawDarkRectangle( int x, int y, int wide, int tall )
{
	// gEngfuncs.pTriAPI->RenderMode( kRenderTransTexture );
	gEngfuncs.pfnFillRGBABlend( x, y, wide, tall, 0, 0, 0, 255 * 0.6 );
	FillRGBA( x + 1, y, wide - 1, 1, 255, 140, 0, 255 );
	FillRGBA( x, y, 1, tall - 1, 255, 140, 0, 255 );
	FillRGBA( x + wide - 1, y + 1, 1, tall - 1, 255, 140, 0, 255 );
	FillRGBA( x, y + tall - 1, wide - 1, 1, 255, 140, 0, 255 );
}

void CHud::HUEtoRGB( float hue, int &R, int &G, int &B )
{
	hue     = ( hue < 0 ) ? 0 : ( hue > 255 ) ? 255
	                                          : hue;
	float h = hue / 255.0f;
	float r, g, b;

	if ( h < 1.0f / 6.0f )
	{
		r = 1.0f;
		g = h * 6.0f;
		b = 0.0f;
	}
	else if ( h < 2.0f / 6.0f )
	{
		r = 1.0f - ( h - 1.0f / 6.0f ) * 6.0f;
		g = 1.0f;
		b = 0.0f;
	}
	else if ( h < 3.0f / 6.0f )
	{
		r = 0.0f;
		g = 1.0f;
		b = ( h - 2.0f / 6.0f ) * 6.0f;
	}
	else if ( h < 4.0f / 6.0f )
	{
		r = 0.0f;
		g = 1.0f - ( h - 3.0f / 6.0f ) * 6.0f;
		b = 1.0f;
	}
	else if ( h < 5.0f / 6.0f )
	{
		r = ( h - 4.0f / 6.0f ) * 6.0f;
		g = 0.0f;
		b = 1.0f;
	}
	else
	{
		r = 1.0f;
		g = 0.0f;
		b = 1.0f - ( h - 5.0f / 6.0f ) * 6.0f;
	}

	R = static_cast< int >( r * 255 );
	G = static_cast< int >( g * 255 );
	B = static_cast< int >( b * 255 );
}

struct
{
	int r;
	int g;
	int b;
} top, bottom;

void CHud::DrawHudModelName( int x, int y, float topcolor, float bottomcolor, const char *model )
{
	std::string firstcolor;
	std::string secondcolor;
	std::string modelname = model;

	size_t mid = modelname.length( ) / 2;

	firstcolor  = modelname.substr( 0, mid );
	secondcolor = modelname.substr( mid );

	HUEtoRGB( topcolor, top.r, top.g, top.b );
	HUEtoRGB( bottomcolor, bottom.r, bottom.g, bottom.b );

	int Width = GetHudStringWidth( firstcolor.c_str( ) );

	DrawHudString( x, y, ScreenWidth, firstcolor.c_str( ), top.r, top.g, top.b );
	DrawHudString( x + Width, y, ScreenWidth, secondcolor.c_str( ), bottom.r, bottom.g, bottom.b );
}

int CHud::DrawText( int x, int y, const char *szString, int r, int g, int b )
{
	if ( !szString )
		return 1;

	gEngfuncs.pfnDrawSetTextColor( r / 255.0, g / 255.0, b / 255.0 );
	return gEngfuncs.pfnDrawConsoleString( x, y, (char *)szString );
}

int CHud::DrawTextCentered( int x, int y, const char *szString, int r, int g, int b )
{
	if ( !szString )
		return 1;

	int width = GetHudStringWidth( szString );
	x += DrawText( x - width / 2, y, szString, r, g, b );
	return x;
}

