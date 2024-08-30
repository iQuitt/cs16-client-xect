
#include <cmath>

#include "hud.h"
#include "cl_util.h"

#include "parsemsg.h"

#include "draw_util.h"
#include "triangleapi.h"
#include "vgui_parser.h"

char path[128];
int CHudSpeedometer::Init( )
{
	gHUD.AddHudElem( this );
	m_iFlags |= HUD_DRAW;

	hud_speedometer                = gEngfuncs.pfnRegisterVariable( "hud_speedometer", "0", FCVAR_ARCHIVE );
	hud_speedometer_x              = gEngfuncs.pfnRegisterVariable( "hud_speedometer_x", "0", FCVAR_ARCHIVE );
	hud_speedometer_y              = gEngfuncs.pfnRegisterVariable( "hud_speedometer_y", "300", FCVAR_ARCHIVE );
	hud_speedometer_red            = gEngfuncs.pfnRegisterVariable( "hud_speedometer_red", "255", FCVAR_ARCHIVE );
	hud_speedometer_green          = gEngfuncs.pfnRegisterVariable( "hud_speedometer_green", "255", FCVAR_ARCHIVE );
	hud_speedometer_blue           = gEngfuncs.pfnRegisterVariable( "hud_speedometer_blue", "255", FCVAR_ARCHIVE );
	hud_speedometer_texture_height = gEngfuncs.pfnRegisterVariable( "hud_speedometer_texture_height", "-10", FCVAR_ARCHIVE );
	hud_speedometer_texture_width  = gEngfuncs.pfnRegisterVariable( "hud_speedometer_texture_width", "-10", FCVAR_ARCHIVE );
	hud_speedometer_texture_type   = gEngfuncs.pfnRegisterVariable( "hud_speedometer_texture_type", "2", FCVAR_ARCHIVE );

	return 1;
}

int CHudSpeedometer::VidInit( )
{
	m_iFlags |= HUD_DRAW;

	return 1;
}
void CHudSpeedometer::DrawNumber( int number, float x, float y, int r, int g, int b, int a,int textureID, int w, int h)
{
	int textureWidth  = 512;
	int textureHeight = 64;
	int digitWidth    = textureWidth / 10;
	int digitHeight   = textureHeight;
	char numberStr[20];
	snprintf( numberStr, sizeof( numberStr ), "%d", number );
	int len = strlen( numberStr );
	float widthScale  = 1.0f;
	float heightScale = 1.0f;
	int scaledSpacing = (int)( w * widthScale );
	int totalWidth    = len * ( digitWidth + scaledSpacing ) - scaledSpacing;

	float startX = x - totalWidth / 2.0f;
	float startY = y - ( digitHeight + h ) / 2.0f;


	for ( int i = 0; i < len; ++i )
	{
		int digit      = numberStr[i] - '0';
		int digitIndex = digit;
		float digitX   = startX + i * ( digitWidth + scaledSpacing );
		float digitY   = startY;
		DrawUtils::Draw2DQuad2( digitX, digitY, digitWidth + w, digitHeight + h, (float)digitIndex / 10.0f, 0.0f, (float)( digitIndex + 1 ) / 10.0f, 1.0f, textureID, r, g, b, a );
	}
}
int CHudSpeedometer::Draw( float time )
{
	if ( !hud_speedometer->value )
		return 0;

	if ( current_texture_type != (int)hud_speedometer_texture_type->value )
	{
		current_texture_type = (int)hud_speedometer_texture_type->value;
		sprintf( path, "resource/floatingdamage/num_nomal_skin%d.tga", current_texture_type );
		textureID = gRenderAPI.GL_LoadTexture( path, NULL, 0, TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP );
	}

	int r, g, b;
	DrawUtils::UnpackRGB( r, g, b, RGB_YELLOWISH );

	if ( hud_speedometer->value == 1 )
	{
		gHUD.DrawStringConsole( ScreenWidth / 2 + hud_speedometer_x->value, ScreenHeight / 2 + hud_speedometer_y->value, hud_speedometer_red->value, hud_speedometer_green->value, hud_speedometer_blue->value, "%d", m_iSpeed );
	}
	else if ( hud_speedometer->value == 2 )
	{
		DrawNumber(m_iSpeed,ScreenWidth / 2 + hud_speedometer_x->value,ScreenHeight / 2 + hud_speedometer_y->value, hud_speedometer_red->value,hud_speedometer_green->value,hud_speedometer_blue->value,255, textureID, hud_speedometer_texture_width->value,hud_speedometer_texture_height->value);
	}
	else
	{
		gHUD.DrawHudNumberCentered( ScreenWidth / 2 + hud_speedometer_x->value, ScreenHeight / 2 + hud_speedometer_y->value, m_iSpeed, hud_speedometer_red->value, hud_speedometer_green->value, hud_speedometer_blue->value );
	}

	return 0;
}

void CHudSpeedometer::UpdateSpeed( const float velocity[2] )
{
	m_iSpeed = std::round( std::hypot( velocity[0], velocity[1] ) );
}