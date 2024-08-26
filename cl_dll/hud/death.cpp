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
// death notice
//
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>
#include "draw_util.h"

#define KILLICON_DISPLAY_TIME hud_killicon_display_time->value
#define KILLEFFECT_DISPLAY_TIME hud_killeffect_display_time->value
float color[3];

DECLARE_MESSAGE( m_DeathNotice, DeathMsg )

struct DeathNoticeItem {
	char szKiller[MAX_PLAYER_NAME_LENGTH*2];
	char szVictim[MAX_PLAYER_NAME_LENGTH*2];
	int iId;	// the index number of the associated sprite
	bool bSuicide;
	bool bTeamKill;
	bool bNonPlayerKill;
	float flDisplayTime;
	float *KillerColor;
	float *VictimColor;
	int iHeadShotId;
};

#define MAX_DEATHNOTICES	4
static int DEATHNOTICE_DISPLAY_TIME = 6;

#define DEATHNOTICE_TOP		32

DeathNoticeItem rgDeathNoticeList[ MAX_DEATHNOTICES + 1 ];

int CHudDeathNotice :: Init( void )
{
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( DeathMsg );

	hud_deathnotice_time = CVAR_CREATE( "hud_deathnotice_time", "6", 0 );
	hud_killicon_display_time = CVAR_CREATE( "hud_killicon_display_time", "1.5", FCVAR_ARCHIVE );
	hud_killeffect_display_time = CVAR_CREATE( "hud_killeffect_display_time", "3", FCVAR_ARCHIVE );
	m_iFlags = 0;

	return 1;
}


void CHudDeathNotice :: Reset( void )
{
	m_killNums       = 0;
	m_multiKills     = 0;
	m_showIcon       = false;
	m_showKill       = false;
	m_iconIndex      = 0;
	m_killEffectTime = 0;
	m_killIconTime   = 0;
}


void CHudDeathNotice :: InitHUDData( void )
{
	memset( rgDeathNoticeList, 0, sizeof(rgDeathNoticeList) );
}


int CHudDeathNotice :: VidInit( void )
{
	m_HUD_d_skull = gHUD.GetSpriteIndex( "d_skull" );
	m_HUD_d_headshot = gHUD.GetSpriteIndex( "d_headshot" );
	m_KM_Number0     = gHUD.GetSpriteIndex( "KM_Number0" );
	m_KM_Number1     = gHUD.GetSpriteIndex( "KM_Number1" );
	m_KM_Number2     = gHUD.GetSpriteIndex( "KM_Number2" );
	m_KM_Number3     = gHUD.GetSpriteIndex( "KM_Number3" );
	m_KM_KillText    = gHUD.GetSpriteIndex( "KM_KillText" );
	m_KM_Icon_Head   = gHUD.GetSpriteIndex( "KM_Icon_Head" );
	m_KM_Icon_Knife  = gHUD.GetSpriteIndex( "KM_Icon_knife" );
	m_KM_Icon_Frag   = gHUD.GetSpriteIndex( "KM_Icon_Frag" );

	return 1;
}

int CHudDeathNotice :: Draw( float flTime )
{
	int x, y, r, g, b, i;

	for( i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( rgDeathNoticeList[i].iId == 0 )
			break;  // we've gone through them all

		if ( rgDeathNoticeList[i].flDisplayTime < flTime )
		{ // display time has expired
			// remove the current item from the list
			memmove( &rgDeathNoticeList[i], &rgDeathNoticeList[i+1], sizeof(DeathNoticeItem) * (MAX_DEATHNOTICES - i) );
			i--;  // continue on the next item;  stop the counter getting incremented
			continue;
		}

		rgDeathNoticeList[i].flDisplayTime = min( rgDeathNoticeList[i].flDisplayTime, flTime + DEATHNOTICE_DISPLAY_TIME );

		// Hide when scoreboard drawing. It will break triapi
		//if ( gViewPort && gViewPort->AllowedToPrintText() )
		//if ( !gHUD.m_iNoConsolePrint )
		{
			// Draw the death notice
			if( !g_iUser1 )
			{
				y = YRES(DEATHNOTICE_TOP) + 2 + (20 * i);  //!!!
			}
			else
			{
				y = ScreenHeight / 5 + 2 + (20 * i);
			}

			int id = (rgDeathNoticeList[i].iId == -1) ? m_HUD_d_skull : rgDeathNoticeList[i].iId;
			x = ScreenWidth - DrawUtils::ConsoleStringLen(rgDeathNoticeList[i].szVictim) - (gHUD.GetSpriteRect(id).right - gHUD.GetSpriteRect(id).left);
			if( rgDeathNoticeList[i].iHeadShotId )
				x -= gHUD.GetSpriteRect(m_HUD_d_headshot).right - gHUD.GetSpriteRect(m_HUD_d_headshot).left;

			if ( !rgDeathNoticeList[i].bSuicide )
			{
				x -= (5 + DrawUtils::ConsoleStringLen( rgDeathNoticeList[i].szKiller ) );

				// Draw killers name
				if ( rgDeathNoticeList[i].KillerColor )
					DrawUtils::SetConsoleTextColor( rgDeathNoticeList[i].KillerColor[0], rgDeathNoticeList[i].KillerColor[1], rgDeathNoticeList[i].KillerColor[2] );
				x = 5 + DrawUtils::DrawConsoleString( x, y, rgDeathNoticeList[i].szKiller );
			}

			r = 255;  g = 80;	b = 0;
			if ( rgDeathNoticeList[i].bTeamKill )
			{
				r = 10;	g = 240; b = 10;  // display it in sickly green
			}

			// Draw death weapon
			SPR_Set( gHUD.GetSprite(id), r, g, b );
			SPR_DrawAdditive( 0, x, y, &gHUD.GetSpriteRect(id) );

			x += (gHUD.GetSpriteRect(id).right - gHUD.GetSpriteRect(id).left);

			if( rgDeathNoticeList[i].iHeadShotId)
			{
				SPR_Set( gHUD.GetSprite(m_HUD_d_headshot), r, g, b );
				SPR_DrawAdditive( 0, x, y, &gHUD.GetSpriteRect(m_HUD_d_headshot));
				x += (gHUD.GetSpriteRect(m_HUD_d_headshot).right - gHUD.GetSpriteRect(m_HUD_d_headshot).left);
			}

			// Draw victims name (if it was a player that was killed)
			if (!rgDeathNoticeList[i].bNonPlayerKill)
			{
				if ( rgDeathNoticeList[i].VictimColor )
					DrawUtils::SetConsoleTextColor( rgDeathNoticeList[i].VictimColor[0], rgDeathNoticeList[i].VictimColor[1], rgDeathNoticeList[i].VictimColor[2] );
				x = DrawUtils::DrawConsoleString( x, y, rgDeathNoticeList[i].szVictim );
			}
		}
	}

	if (m_showKill)
	{
		m_killEffectTime = min(m_killEffectTime, gHUD.m_flTime + KILLEFFECT_DISPLAY_TIME);

		if (gHUD.m_flTime < m_killEffectTime)
		{
			int r = 255, g = 255, b = 255;
			float alpha = (m_killEffectTime - gHUD.m_flTime) / KILLEFFECT_DISPLAY_TIME;
			int numIndex = -1;

			if (alpha > 0)
			{
				r *= alpha;
				g *= alpha;
				b *= alpha;

				switch (m_multiKills)
				{
				case 1:
				{
					numIndex = m_KM_Number0;
					break;
				}

				case 2:
				{
					numIndex = m_KM_Number1;
					break;
				}

				case 3:
				{
					numIndex = m_KM_Number2;
					break;
				}

				case 4:
				{
					numIndex = m_KM_Number3;
					break;
				}
				}

				if (numIndex != -1)
				{
					int numWidth, numHeight;
					int textWidth, textHeight;
					int iconWidth, iconHeight;

					numWidth = gHUD.GetSpriteRect(numIndex).right - gHUD.GetSpriteRect(numIndex).left;
					numHeight = gHUD.GetSpriteRect(numIndex).bottom - gHUD.GetSpriteRect(numIndex).top;
					textWidth = gHUD.GetSpriteRect(m_KM_KillText).right - gHUD.GetSpriteRect(m_KM_KillText).left;
					textHeight = gHUD.GetSpriteRect(m_KM_KillText).bottom - gHUD.GetSpriteRect(m_KM_KillText).top;
					iconWidth = gHUD.GetSpriteRect(m_KM_Icon_Head).right - gHUD.GetSpriteRect(m_KM_Icon_Head).left;
					iconHeight = gHUD.GetSpriteRect(m_KM_Icon_Head).bottom - gHUD.GetSpriteRect(m_KM_Icon_Head).top;

					if (m_multiKills == 1)
						numWidth += 10;

					y = (25.0 * 0.01 * ScreenHeight) - (iconHeight + textHeight) * 0.5;
					x = (50.0 * 0.01 * ScreenWidth) - (numHeight + textWidth) * 0.5;

					SPR_Set(gHUD.GetSprite(numIndex), r, g, b);
					SPR_DrawAdditive(0, x, y - (gHUD.GetSpriteRect(numIndex).bottom + gHUD.GetSpriteRect(m_KM_KillText).top - gHUD.GetSpriteRect(m_KM_KillText).bottom - gHUD.GetSpriteRect(numIndex).top) * 0.6, &gHUD.GetSpriteRect(numIndex));

					SPR_Set(gHUD.GetSprite(m_KM_KillText), r, g, b);
					SPR_DrawAdditive(0, x + numWidth, y, &gHUD.GetSpriteRect(m_KM_KillText));

					x = (50.0 * 0.01 * ScreenWidth) - (iconWidth) * 0.5;
					y = y + textHeight;

					m_killIconTime = min(m_killIconTime, gHUD.m_flTime + KILLICON_DISPLAY_TIME);

					if (m_showIcon)
					{
						alpha = (m_killIconTime - gHUD.m_flTime) / KILLICON_DISPLAY_TIME;

						if (alpha > 0)
						{
							r *= alpha;
							g *= alpha;
							b *= alpha;

							switch (m_iconIndex)
							{
							case 1:
							{
								SPR_Set(gHUD.GetSprite(m_KM_Icon_Head), r, g, b);
								SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_KM_Icon_Head));
								break;
							}

							case 2:
							{
								SPR_Set(gHUD.GetSprite(m_KM_Icon_Knife), r, g, b);
								SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_KM_Icon_Knife));
								break;
							}

							case 3:
							{
								SPR_Set(gHUD.GetSprite(m_KM_Icon_Frag), r, g, b);
								SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_KM_Icon_Frag));
								break;
							}
							}
						}
					}
				}
			}
		}
		else
		{
			m_showKill = false;
			m_showIcon = false;
		}
	}


	if( i == 0 )
		m_iFlags &= ~HUD_DRAW; // disable hud item

	return 1;
}

// This message handler may be better off elsewhere
int CHudDeathNotice :: MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf )
{
	m_iFlags |= HUD_DRAW;

	BufferReader reader( pszName, pbuf, iSize );

	int killer = reader.ReadByte();
	int victim = reader.ReadByte();
	int headshot = reader.ReadByte();
	int multiKills = 0;
	int idx       = gEngfuncs.GetLocalPlayer( )->index;

	char killedwith[32];
	strncpy( killedwith, "d_", sizeof(killedwith) );
	strcat( killedwith, reader.ReadString() );

	//if (gViewPort)
	//	gViewPort->DeathMsg( killer, victim );
	gHUD.m_Scoreboard.DeathMsg( killer, victim );

	gHUD.m_Spectator.DeathMessage(victim);
	
	if ( !strcmp( killedwith, "d_knife" ) )
	{
		if ( killer == idx )
		{
			m_showIcon     = true;
			m_iconIndex    = 2;
			m_killIconTime = gHUD.m_flTime + KILLICON_DISPLAY_TIME;
			gHUD.m_SoundManager.AddSound( "Humililation" );
		}

		if ( victim == idx )
		{
			gHUD.m_SoundManager.AddSound( "OhNo" );
			m_showIcon     = true;
			m_iconIndex    = 2;
			m_killIconTime = gHUD.m_flTime + KILLICON_DISPLAY_TIME;
		}
	}

	if ( killer == idx && victim != idx )
	{
		m_killNums++;
		m_multiKills++;
		m_showIcon = false;

		if ( headshot )
		{
			m_showIcon     = true;
			m_iconIndex    = 1;
			m_killIconTime = gHUD.m_flTime + KILLICON_DISPLAY_TIME;
			
			 gHUD.m_SoundManager.AddSound( "HeadShot" );
		}

		if ( !strcmp( killedwith, "d_grenade" ) )
		{
			m_showIcon     = true;
			m_iconIndex    = 3;
			m_killIconTime = gHUD.m_flTime + KILLICON_DISPLAY_TIME;
			 gHUD.m_SoundManager.AddSound( "GotIt" );
		}

		switch ( m_multiKills )
		{
		case 1:
			gHUD.m_SoundManager.AddSound( "FirstKill" );
			m_showKill       = true;
			m_killEffectTime = gHUD.m_flTime + KILLEFFECT_DISPLAY_TIME;
			break;
		case 2:
			gHUD.m_SoundManager.AddSound( "DoubleKill" );
			m_showKill       = true;
			m_killEffectTime = gHUD.m_flTime + KILLEFFECT_DISPLAY_TIME;
			break;
		case 3:
			gHUD.m_SoundManager.AddSound( "TripleKill" );
			m_showKill       = true;
			m_killEffectTime = gHUD.m_flTime + KILLEFFECT_DISPLAY_TIME;
			break;
		default:
			if ( m_multiKills > 3 )
			{
				gHUD.m_SoundManager.AddSound( "MultiKill" );
				m_showKill       = true;
				m_killEffectTime = gHUD.m_flTime + KILLEFFECT_DISPLAY_TIME;
			}
			break;
		}

		
		switch ( m_killNums )
		{
			case 5:
				 gHUD.m_SoundManager.AddSound( "Excellent" );
				break;
			case 10:
				 gHUD.m_SoundManager.AddSound( "Incredible" );
				break;
			case 15:
				 gHUD.m_SoundManager.AddSound( "Crazy" );
				break;
			case 20:
				 gHUD.m_SoundManager.AddSound( "CantBelive" );
				break;
			case 25:
				 gHUD.m_SoundManager.AddSound( "OutofWorld" );
				break;
		}
	}


	int i;
	for ( i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( rgDeathNoticeList[i].iId == 0 )
			break;
	}
	if ( i == MAX_DEATHNOTICES )
	{ // move the rest of the list forward to make room for this item
		memmove( rgDeathNoticeList, rgDeathNoticeList+1, sizeof(DeathNoticeItem) * MAX_DEATHNOTICES );
		i = MAX_DEATHNOTICES - 1;
	}

	//if (gViewPort)
		//gViewPort->GetAllPlayersInfo();
	gHUD.m_Scoreboard.GetAllPlayersInfo();

	// Get the Killer's name
	const char *killer_name = g_PlayerInfoList[ killer ].name;
	if ( !killer_name )
	{
		killer_name = "";
		rgDeathNoticeList[i].szKiller[0] = 0;
	}
	else
	{
		rgDeathNoticeList[i].KillerColor = GetClientColor( killer );
		strncpy( rgDeathNoticeList[i].szKiller, killer_name, MAX_PLAYER_NAME_LENGTH );
		rgDeathNoticeList[i].szKiller[MAX_PLAYER_NAME_LENGTH-1] = 0;
	}

	// Get the Victim's name
	const char *victim_name = NULL;
	// If victim is -1, the killer killed a specific, non-player object (like a sentrygun)
	if ( ((char)victim) != -1 )
		victim_name = g_PlayerInfoList[ victim ].name;
	if ( !victim_name )
	{
		victim_name = "";
		rgDeathNoticeList[i].szVictim[0] = 0;
	}
	else
	{
		rgDeathNoticeList[i].VictimColor = GetClientColor( victim );
		strncpy( rgDeathNoticeList[i].szVictim, victim_name, MAX_PLAYER_NAME_LENGTH );
		rgDeathNoticeList[i].szVictim[MAX_PLAYER_NAME_LENGTH-1] = 0;
	}

	// Is it a non-player object kill?
	if ( ((char)victim) == -1 )
	{
		rgDeathNoticeList[i].bNonPlayerKill = true;

		// Store the object's name in the Victim slot (skip the d_ bit)
		strncpy( rgDeathNoticeList[i].szVictim, killedwith+2, sizeof(killedwith) );
	}
	else
	{
		if ( killer == victim || killer == 0 )
			rgDeathNoticeList[i].bSuicide = true;

		if ( !strncmp( killedwith, "d_teammate", sizeof(killedwith)  ) )
			rgDeathNoticeList[i].bTeamKill = true;
	}

	rgDeathNoticeList[i].iHeadShotId = headshot;

	// Find the sprite in the list
	int spr = gHUD.GetSpriteIndex( killedwith );

	rgDeathNoticeList[i].iId = spr;

	rgDeathNoticeList[i].flDisplayTime = gHUD.m_flTime + hud_deathnotice_time->value;


	if (rgDeathNoticeList[i].bNonPlayerKill)
	{
		ConsolePrint( rgDeathNoticeList[i].szKiller );
		ConsolePrint( " killed a " );
		ConsolePrint( rgDeathNoticeList[i].szVictim );
		ConsolePrint( "\n" );
	}
	else
	{
		// record the death notice in the console
		if ( rgDeathNoticeList[i].bSuicide )
		{
			ConsolePrint( rgDeathNoticeList[i].szVictim );

			if ( !strncmp( killedwith, "d_world", sizeof(killedwith)  ) )
			{
				ConsolePrint( " died" );
			}
			else
			{
				ConsolePrint( " killed self" );
			}
		}
		else if ( rgDeathNoticeList[i].bTeamKill )
		{
			ConsolePrint( rgDeathNoticeList[i].szKiller );
			ConsolePrint( " killed his teammate " );
			ConsolePrint( rgDeathNoticeList[i].szVictim );
		}
		else
		{
			if( headshot )
				ConsolePrint( "*** ");
			ConsolePrint( rgDeathNoticeList[i].szKiller );
			ConsolePrint( " killed " );
			ConsolePrint( rgDeathNoticeList[i].szVictim );
		}

		if ( *killedwith && (*killedwith > 13 ) && strncmp( killedwith, "d_world", sizeof(killedwith) ) && !rgDeathNoticeList[i].bTeamKill )
		{
			if ( headshot )
				ConsolePrint(" with a headshot from ");
			else
				ConsolePrint(" with ");

			ConsolePrint( killedwith+2 ); // skip over the "d_" part
		}

		if( headshot ) ConsolePrint( " ***");
		ConsolePrint( "\n" );
	}

	return 1;
}




