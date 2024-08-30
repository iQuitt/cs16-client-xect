#include "hud.h"

#include "cl_util.h"
#include "draw_util.h"
#include "parsemsg.h"
#include "triangleapi.h"
#include "vgui_parser.h"
#include <cmath>
#include <string>
#define TEXTURE_DRAW_DELAY 3.0f
int CHudAnnouncerIcon::Init( )
{
	gHUD.AddHudElem( this );
	m_iFlags |= HUD_DRAW;

	return 1;
}
int CHudAnnouncerIcon::VidInit( )
{
	m_iFlags |= HUD_DRAW;
	return 1;
}
int CHudAnnouncerIcon::Draw( float flTime )
{
	if ( !hud_killfx->value )
	{
		return 0;
	}

	textureManager.Update( flTime );

	if ( flTime < gHUD.m_DeathNotice.m_killEffectTime )
	{
		if ( m_iHeadshot )
		{
			textureManager.AddTexture( announceTextures[ALARM_HEADSHOT], flTime );

			m_iHeadshot = false; // Reset after adding
		}

		if ( gHUD.m_DeathNotice.m_multiKills > 0 && gHUD.m_DeathNotice.m_multiKills <= 4 )
		{
			int alarmType = 0;
			switch ( gHUD.m_DeathNotice.m_multiKills )
			{
			case 1:
				alarmType = announceTextures[ALARM_FIRSTBLOOD];
				textureManager.AddTexture( alarmType, flTime );

				break;
			case 2:
				alarmType = announceTextures[ALARM_2KILL];
				textureManager.AddTexture( alarmType, flTime );

				break;
			case 3:
				alarmType = announceTextures[ALARM_3KILL];
				textureManager.AddTexture( alarmType, flTime );

				break;
			case 4:
				alarmType = announceTextures[ALARM_4KILL];
				textureManager.AddTexture( alarmType, flTime );

				break;
			}
		}
		else if ( gHUD.m_DeathNotice.m_killNums >= 5 )
		{
			int alarmType = 0;
			if ( gHUD.m_DeathNotice.m_killNums >= 25 )
				alarmType = announceTextures[ALARM_OUTOFWORLD];
			else if ( gHUD.m_DeathNotice.m_killNums >= 20 )
				alarmType = announceTextures[ALARM_CANTBELIEVE];
			else if ( gHUD.m_DeathNotice.m_killNums >= 15 )
				alarmType = announceTextures[ALARM_CRAZY];
			else if ( gHUD.m_DeathNotice.m_killNums >= 10 )
				alarmType = announceTextures[ALARM_INCREDIBLE];
			else
				alarmType = announceTextures[ALARM_EXCELLENT];

			textureManager.AddTexture( alarmType, flTime );
		}

		if ( textureManager.HasTextures( ) )
		{
			auto currentTexture = textureManager.GetCurrentTexture( );
			int textureWidth    = 94;
			int textureHeight   = 94;
			float startX        = textureWidth / 2.0f;
			float startY        = textureHeight / 2.0f;

			DrawUtils::Draw2DQuad2( startX, startY, textureWidth, textureHeight, 0.0f, 0.0f, 1.0f, 1.0f, currentTexture.textureID, 255, 255, 255, currentTexture.alpha * 255 );
		}
	}

	return 1;
}
