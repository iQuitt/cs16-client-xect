
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "kbutton.h"
#include "triangleapi.h"
#include <string.h>
#include <stdio.h>
#include "draw_util.h"



int m_TeamWIN[4];
bool m_ZombieMaps;

int CHudWinImage::Init( )
{
	gHUD.AddHudElem( this );
	m_iFlags |= HUD_DRAW;

	hud_winimage = gEngfuncs.pfnRegisterVariable( "cl_winimage", "1", FCVAR_ARCHIVE );

	m_TeamWIN[0] = gRenderAPI.GL_LoadTexture( "gfx/xgclient/trwin.tga", NULL, 0, TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP );
	m_TeamWIN[1] = gRenderAPI.GL_LoadTexture( "gfx/xgclient/ctwin.tga", NULL, 0, TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP );
	m_TeamWIN[2] = gRenderAPI.GL_LoadTexture( "gfx/xgclient/hmwin.tga", NULL, 0, TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP );// human win for zombie server (not Ready)
	m_TeamWIN[3] = gRenderAPI.GL_LoadTexture( "gfx/xgclient/zmwin.tga", NULL, 0, TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP );// zombie win for zombie server (not Ready)

	return 1;
}

int CHudWinImage::VidInit( )
{
	m_iFlags |= HUD_DRAW;
	m_flEndTime = 0.0f;
	bWinningTeam = WIN_TEAM_NONE;
	return 1;
}

void CHudWinImage::Think( float flTime)
{

	if ( m_flEndTime && m_flEndTime < flTime )
	{
		bWinningTeam = WIN_TEAM_NONE;
		m_flEndTime  = 0.0f;
	}
}

int CHudWinImage::Draw( float flTime )
{
	if ( !hud_winimage->value || ( !bWinningTeam ) )
		return 0;

	int x = ScreenWidth / 2;
	int y = ScreenHeight / 2;

	int iX, iY;
	int	wide   = 330;
	int	height = 181;
	

	iX = x - wide / 2;
	iY = y - height / 2;

	gRenderAPI.GL_SelectTexture( 0 );
	gEngfuncs.pTriAPI->RenderMode( kRenderTransColor );
	gEngfuncs.pTriAPI->Brightness( 1.0 );
	gEngfuncs.pTriAPI->Color4ub( 255, 255, 255, 255 );
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );

	if ( bWinningTeam == WIN_TEAM_TR )
	{
		if ( m_TeamWIN )
		{

			gRenderAPI.GL_Bind( 0, m_TeamWIN[0] );

			DrawUtils::Draw2DQuadScaled( iX, iY * 0.35, iX + wide, iY * 0.35 + height );
		}
	}
	else if ( bWinningTeam == WIN_TEAM_CT )
	{
		if ( m_TeamWIN )
		{

			gRenderAPI.GL_Bind( 0, m_TeamWIN[1] );

			DrawUtils::Draw2DQuadScaled( iX, iY * 0.35, iX + wide, iY * 0.35 + height );
		}
	}
	return 1;
}