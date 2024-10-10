/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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
// Scoreboard.cpp
//
// implementation of CHudScoreboard class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "triangleapi.h"
#include "com_weapons.h"
#include "cdll_dll.h"

#include <string.h>
#include <stdio.h>
#include "draw_util.h"
#include <algorithm>
#include "com_model.h"
#include "r_studioint.h"
hud_player_info_t   g_PlayerInfoList[MAX_PLAYERS+1]; // player info from the engine
extra_player_info_t	g_PlayerExtraInfo[MAX_PLAYERS+1]; // additional player info sent directly to the client dll
team_info_t         g_TeamInfo[MAX_TEAMS+1];
hostage_info_t      g_HostageInfo[MAX_HOSTAGES+1];
extern engine_studio_api_t IEngineStudio;
int g_iUser1;
int g_iUser2;
int g_iUser3;
int g_iTeamNumber;


// X positions

int xstart, xend;
int ystart, yend;
// relative to the side of the scoreboard
inline int NAME_POS_START()		{ return xstart + 15; }
inline int NAME_POS_END()		{ return xend - 210; }

inline int ATTRIB_POS_START()	{ return xend - 420; }
inline int ATTRIB_POS_END()		{ return xend - 360; }

inline int HEALTH_POS_START()	{ return xend - 350; }
inline int HEALTH_POS_END()		{ return xend - 290; }

inline int MONEY_POS_START()	{ return xend - 280; }
inline int MONEY_POS_END()		{ return xend - 220; }

inline int KILLS_POS_START()	{ return xend - 210; }
inline int KILLS_POS_END()		{ return xend - 150; }

inline int DEATHS_POS_START()	{ return xend - 140; }
inline int DEATHS_POS_END()		{ return xend - 80; }

inline int PING_POS_START()		{ return xend - 70; }
inline int PING_POS_END()		{ return xend - 10; }

inline int WEAPON_POS_START()	{ return xend - 430; }
inline int WEAPON_POS_END( )	{ return xend - 600; }


DECLARE_COMMAND( m_Scoreboard, ShowScores )
DECLARE_COMMAND( m_Scoreboard, HideScores )
DECLARE_COMMAND( m_Scoreboard, ShowScoreboard2 )
DECLARE_COMMAND( m_Scoreboard, HideScoreboard2 )

DECLARE_MESSAGE( m_Scoreboard, ScoreInfo )
DECLARE_MESSAGE( m_Scoreboard, TeamInfo )
DECLARE_MESSAGE( m_Scoreboard, TeamScore )
DECLARE_MESSAGE( m_Scoreboard, HealthInfo )
DECLARE_MESSAGE( m_Scoreboard, Account )

int CHudScoreboard :: Init( void )
{
	gHUD.AddHudElem( this );

	hud_scoreboard = gEngfuncs.pfnRegisterVariable( "hud_topscoreboard", "1", FCVAR_ARCHIVE );// 1 old cso scoreboard 2 new cso scoreboard 3 cs2 scoreboard (not ready)
	// Hook messages & commands here
	HOOK_COMMAND( "+showscores", ShowScores );
	HOOK_COMMAND( "-showscores", HideScores );
	HOOK_COMMAND( "showscoreboard2", ShowScoreboard2 );
	HOOK_COMMAND( "hidescoreboard2", HideScoreboard2 );

	HOOK_MESSAGE( ScoreInfo );
	HOOK_MESSAGE( TeamScore );
	HOOK_MESSAGE( TeamInfo );
	HOOK_MESSAGE( HealthInfo );
	HOOK_MESSAGE( Account );

	InitHUDData();

	return 1;
}


int CHudScoreboard :: VidInit( void )
{

	m_iTeamScore_Max = 0;
	m_iTeamScore_T   = 0;
	m_iTeamScore_CT  = 0;
	m_iTeamAlive_T   = 0;
	m_iTeamAlive_CT  = 0;
	m_flNextCache    = 0;

	m_iOriginalBG  = gHUD.GetSpriteIndex( "SBOriginalBG" );
	m_iTeamDeathBG = gHUD.GetSpriteIndex( "SBTeamDeathBG" );
	m_iUnitehBG    = gHUD.GetSpriteIndex( "SBUnitehBG" );
	m_iNum_L       = gHUD.GetSpriteIndex( "SBNum_L" );
	m_iNum_S       = gHUD.GetSpriteIndex( "SBNum_S" );
	m_iText_CT     = gHUD.GetSpriteIndex( "SBText_CT" );
	m_iText_T      = gHUD.GetSpriteIndex( "SBText_T" );
	m_iText_TR     = gHUD.GetSpriteIndex( "SBText_TR" );
	m_iText_HM     = gHUD.GetSpriteIndex( "SBText_HM" );
	m_iText_ZB     = gHUD.GetSpriteIndex( "SBText_ZB" );
	m_iText_1st    = gHUD.GetSpriteIndex( "SBText_1st" );
	m_iText_Kill   = gHUD.GetSpriteIndex( "SBText_Kill" );
	m_iText_Round  = gHUD.GetSpriteIndex( "SBText_Round" );

	m_iBGIndex              = m_iOriginalBG;
	m_iTextIndex            = m_iText_Round;
	m_iTTextIndex           = m_iText_TR;
	m_iCTTextIndex          = m_iText_CT;
	m_bIsTeamplay           = true;
	m_bTopScoreBoardEnabled = true;

	BuildHudNumberRect( m_iNum_L, m_rcNumber_Large, 13, 13, 1, 1 );
	BuildHudNumberRect( m_iNum_S, m_rcNumber_Small, 10, 10, 1, 1 );

	auto loadTextures = []( std::vector< int > &textureVector, const std::vector< std::string > &pathVector, int maxTextures )
	{
		textureVector.resize( maxTextures, 0 );
		for ( int i = 0; i < maxTextures && i < static_cast< int >( pathVector.size( ) ); ++i )
		{
			if ( !pathVector[i].empty( ) )
			{
				textureVector[i] = gRenderAPI.GL_LoadTexture( pathVector[i].c_str( ), NULL, 0, TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP );
				if ( textureVector[i] == 0 )
				{
					gEngfuncs.Con_Printf( "^9ERROR: ^7Failed to load texture: %s\n", pathVector[i].c_str( ) );
				}
			}
		}
	};
	// Load CSO TopScoreboard New
	loadTextures( gHUD.m_Scoreboard.csoTexture, gHUD.m_Scoreboard.texturePaths, CHudScoreboard::CSO_New_Scoreboard::SB_NUM_YELLOW );

	xstart = ScreenWidth * 0.125f;
	xend = ScreenWidth - xstart;
	ystart = 100;
	yend = ScreenHeight - ystart;
	m_bForceDraw = false;

	// Load sprites here
	return 1;
}

void CHudScoreboard :: InitHUDData( void )
{
	memset( g_PlayerExtraInfo, 0, sizeof g_PlayerExtraInfo );
	m_iLastKilledBy = 0;
	m_fLastKillTime = 0;
	m_iPlayerNum = 0;
	m_iNumTeams = 0;
	memset( g_TeamInfo, 0, sizeof g_TeamInfo );

	m_iFlags &= ~HUD_DRAW;  // starts out inactive

	m_iFlags |= HUD_INTERMISSION; // is always drawn during an intermission

	m_iTeamScore_Max = 0;
	m_iTeamScore_T   = 0;
	m_iTeamScore_CT  = 0;
	m_iTeamAlive_T   = 0;
	m_iTeamAlive_CT  = 0;
}

// Y positions
#define ROW_GAP  15

int CHudScoreboard :: Draw( float flTime )
{
	//https://github.com/hasandramali/cs16advanced/blob/e2694e545a2ce5c13e5477a5e786e0bf3caf7d9f/jni/cs16adv/cl_dll/hud/scoreboard.cpp#L198

	if ( hud_scoreboard->value )
	{
		DrawTopScoreBoard( flTime );
	}
	if( !m_bForceDraw )
	{
		if ( (!m_bShowscoresHeld && gHUD.m_Health.m_iHealth > 0 && !gHUD.m_iIntermission) )
			return 1;
		else
		{
			xstart     = 0.125f * ScreenWidth;
			xend       = ScreenWidth - xstart;
			ystart     = 90;
			yend       = ScreenHeight - ystart;
			m_colors.r = 0;
			m_colors.g = 0;
			m_colors.b = 0;
			m_colors.a = 153;
			m_bDrawStroke = false;
		}
	}

	return DrawScoreboard(flTime);
}

int CHudScoreboard :: DrawScoreboard( float fTime )
{
	GetAllPlayersInfo();
	char ServerName[90];

	// just sort the list on the fly
	// list is sorted first by frags, then by deaths
	float list_slot = 0;

	// print the heading line

	DrawUtils::DrawRectangle(xstart, ystart, xend - xstart, yend - ystart,
		m_colors.r, m_colors.g, m_colors.b, m_colors.a, m_bDrawStroke);

	int ypos = ystart + (list_slot * ROW_GAP) + 10;

	if( gHUD.m_szServerName[0] )
		_snprintf(ServerName, 64, "%s", gHUD.m_szServerName);

	DrawUtils::DrawHudString( NAME_POS_START(), ypos, NAME_POS_END(), ServerName, 255, 140, 0 );
	DrawUtils::DrawHudStringReverse( HEALTH_POS_END(), ypos, 0, "HP", 255, 140, 0 );
	DrawUtils::DrawHudStringReverse( MONEY_POS_END(), ypos, MONEY_POS_START(), "Money", 255, 140, 0 );
	DrawUtils::DrawHudStringReverse( KILLS_POS_END(), ypos, 0, "Score", 255, 140, 0 );
	DrawUtils::DrawHudStringReverse( DEATHS_POS_END(), ypos, DEATHS_POS_START(), "Deaths", 255, 140, 0 );
	DrawUtils::DrawHudStringReverse( PING_POS_END(), ypos, PING_POS_START(), "Latency", 255, 140, 0 );
	DrawUtils::DrawHudStringReverse( WEAPON_POS_END(), ypos, 0, "Weapon", 255, 140, 0 );


	list_slot += 2;
	ypos = ystart + (list_slot * ROW_GAP);
	//FillRGBA( xstart, ypos, xend - xstart, 1, 255, 140, 0, 255);  // draw the separator line
	
	list_slot += 0.8;

	if ( gHUD.m_Teamplay )
	{
		DrawTeams( list_slot );
	}
	else
	{
		// it's not teamplay,  so just draw a simple player list
		DrawPlayers( list_slot );
	}
	return 1;
}

void CHudScoreboard::GetPlayerWeaponModel( int iIndex )
{
	if ( iIndex < 1 || iIndex >= MAX_PLAYERS )
	{
		return;
	}
	cl_entity_t *ent = gEngfuncs.GetEntityByIndex( iIndex );
	if ( !ent )
	{
		return;
	}
	model_t *pweaponmodel = IEngineStudio.GetModelByIndex( ent->curstate.weaponmodel );
	if ( !pweaponmodel || !pweaponmodel->name )
	{
		return;
	}
	const char *modelName = pweaponmodel->name;
	int modelNameLen      = strlen( modelName );
	if ( modelNameLen < 12 || iIndex < 1 || iIndex >= MAX_PLAYERS )
	{
		return;
	}


	// TR: sunucularda farklı model isimleri p_ ekine sahip oluyor veya klasöre ekliyse model örnekklasör/p_modelismi gibi bilgi geri dönüyor burada /p_ yada p_ ekinin önünde olan ismi alıyoruz sadece
	// EN: Different model names on servers have the p_ suffix or if they are added to a folder, information such as examplefolder/p_modelname is returned. Here we only take the name in front of the /p_ or p_ suffix.
	const char *pStart = strstr( modelName, "/p_" );
	if ( !pStart )
		pStart = strstr( modelName, "p_" );

	if ( pStart )
	{
		// TR: /p_ yada p_ yi sil 
		// EN: remove /p_ or p_
		pStart += ( pStart[0] == '/' ? 3 : 2 );


		// TR: silahın /p_ yada p_ ekinden sonraki ismi bul 
		// EN :find the name of the weapon after the /p_ or p_ suffix
		const char *pEnd = strrchr( pStart, '.' );
		if ( !pEnd )
			pEnd = modelName + modelNameLen;

		int iLen = pEnd - pStart;
		if ( iLen >= sizeof( g_PlayerExtraInfo[iIndex].weaponName ) )
		{
			iLen = sizeof( g_PlayerExtraInfo[iIndex].weaponName ) - 1;
		}

		strncpy( g_PlayerExtraInfo[iIndex].weaponName, pStart, iLen );
		g_PlayerExtraInfo[iIndex].weaponName[iLen] = '\0';
	}
	else
	{
		int iLen = modelNameLen - 12;
		if ( iLen >= sizeof( g_PlayerExtraInfo[iIndex].weaponName ) )
		{
			iLen = sizeof( g_PlayerExtraInfo[iIndex].weaponName ) - 1;
		}
		strncpy( g_PlayerExtraInfo[iIndex].weaponName, modelName + 9, iLen );
		g_PlayerExtraInfo[iIndex].weaponName[iLen] = '\0';
	}

	// TR: silah bilgisini alırken .mdl çizdirtmeye çalışıyor ama nedense sadece nokta çiziliyor ak47. gibi burda sadece .'yi siliyorum
	// EN: When getting the weapon information, it tries to draw .mdl but for some reason only a dot is drawn, like ak47. Here I just delete the dot
	int len = strlen( g_PlayerExtraInfo[iIndex].weaponName );
	if ( len > 0 && g_PlayerExtraInfo[iIndex].weaponName[len - 1] == '.' )
	{
		g_PlayerExtraInfo[iIndex].weaponName[len - 1] = '\0';
	}
}
int CHudScoreboard ::DrawTeams( float list_slot )
{
	int j;
	int ypos = ystart + (list_slot * ROW_GAP) + 5;

	// clear out team scores
	for ( int i = 1; i <= m_iNumTeams; i++ )
	{
		if ( !g_TeamInfo[i].scores_overriden )
			g_TeamInfo[i].frags = g_TeamInfo[i].deaths = 0;
		g_TeamInfo[i].sumping = 0;
		g_TeamInfo[i].players = 0;
		g_TeamInfo[i].already_drawn = FALSE;
	}

	// recalc the team scores, then draw them
	for ( int i = 1; i < MAX_PLAYERS; i++ )
	{

		if ( !g_PlayerInfoList[i].name || !g_PlayerInfoList[i].name[0] )
			continue; // empty player slot, skip

		if ( g_PlayerExtraInfo[i].teamname[0] == 0 )
			continue; // skip over players who are not in a team
		if (gHUD.m_CHudCFMarks.iTeam == TEAM_SPECTATOR)
		{
			GetPlayerWeaponModel( i );
		}
		else if ( g_PlayerExtraInfo[i].teamnumber == gHUD.m_CHudCFMarks.iTeam )
		{
			GetPlayerWeaponModel( i );
		}	
		// find what team this player is in
		for ( j = 1; j <= m_iNumTeams; j++ )
		{
			if ( !stricmp( g_PlayerExtraInfo[i].teamname, g_TeamInfo[j].name ) )
				break;
		}

		if ( j > m_iNumTeams )  // player is not in a team, skip to the next guy
			continue;

		if ( !g_TeamInfo[j].scores_overriden )
		{
			g_TeamInfo[j].frags += g_PlayerExtraInfo[i].frags;
			g_TeamInfo[j].deaths += g_PlayerExtraInfo[i].deaths;
		}

		g_TeamInfo[j].sumping += g_PlayerInfoList[i].ping;

		if ( g_PlayerInfoList[i].thisplayer )
			g_TeamInfo[j].ownteam = TRUE;
		else
			g_TeamInfo[j].ownteam = FALSE;

		g_TeamInfo[j].players++;
	}

	// Draw the teams
	int iSpectatorPos = -1;

	while( true )
	{
		int highest_frags = -99999; int lowest_deaths = 99999;
		int best_team = 0;

		for ( int i = 1; i <= m_iNumTeams; i++ )
		{
			// don't draw team without players
			if ( g_TeamInfo[i].players <= 0 )
				continue;

			if (!strnicmp(g_TeamInfo[i].name, "SPECTATOR", MAX_TEAM_NAME))
			{
				iSpectatorPos = i;
				continue;
			}

			if ( !g_TeamInfo[i].already_drawn && g_TeamInfo[i].frags >= highest_frags )
			{
				if ( g_TeamInfo[i].frags > highest_frags || g_TeamInfo[i].deaths < lowest_deaths )
				{
					best_team = i;
					lowest_deaths = g_TeamInfo[i].deaths;
					highest_frags = g_TeamInfo[i].frags;
				}
			}
		}

		// draw the best team on the scoreboard
		if ( !best_team )
		{
			// if spectators is found and still not drawn
			if( iSpectatorPos != -1 && g_TeamInfo[iSpectatorPos].already_drawn == FALSE )
				best_team = iSpectatorPos;
			else break;
		}
		// draw out the best team
		team_info_t *team_info = &g_TeamInfo[best_team];

		// don't draw team without players
		if ( team_info->players <= 0 )
			continue;

		ypos = ystart + (list_slot * ROW_GAP);

		// check we haven't drawn too far down
		if ( ypos > yend )  // don't draw to close to the lower border
			break;

		int r, g, b;
		char teamName[64];

		GetTeamColor( r, g, b, team_info->teamnumber );
		switch( team_info->teamnumber )
		{
		case TEAM_TERRORIST:
			_snprintf(teamName, sizeof(teamName), "Terrorists   -   %i players", team_info->players);
			break;
		case TEAM_CT:
			_snprintf(teamName, sizeof(teamName), "Counter-Terrorists   -   %i players", team_info->players);
			break;
		case TEAM_SPECTATOR:
		case TEAM_UNASSIGNED:
			strncpy( teamName, "Spectators", sizeof(teamName) );
			break;
		}

		DrawUtils::DrawHudString( NAME_POS_START(), ypos, NAME_POS_END(),   teamName,   r, g, b );

		team_info->already_drawn = TRUE;  // set the already_drawn to be TRUE, so this team won't get drawn again

		// draw underline
		list_slot += 1.2f;
		FillRGBA( xstart, ystart + (list_slot * ROW_GAP), xend - xstart, 1, r, g, b, 255);

		list_slot += 0.4f;
		// draw all the players that belong to this team, indented slightly
		list_slot = DrawPlayers( list_slot, 10, team_info->name );
	}

	// draw all the players who are not in a team
	list_slot += 4.0f;
	DrawPlayers( list_slot, 0, "" );

	return 1;
}

inline bool CHudScoreboard::IsConnected( int playerIndex )
{
	return ( g_PlayerInfoList[playerIndex].name && g_PlayerInfoList[playerIndex].name[0] != 0 );
}

inline int CHudScoreboard::GetTeamCounts( short teamnumber )
{
	int count = 0;

	for ( int i = 1; i <= MAX_PLAYERS; i++ )
	{
		GetPlayerInfo( i, &g_PlayerInfoList[i] );
		if ( !IsConnected( i ) )
			continue;

		if ( g_PlayerExtraInfo[i].teamnumber == teamnumber )
			count++;
	}

	return count;
}

inline int CHudScoreboard::GetTeamAliveCounts( short teamnumber )
{
	int count = 0;

	for ( int i = 1; i <= MAX_PLAYERS; i++ )
	{
		GetPlayerInfo( i, &g_PlayerInfoList[i] );
		if ( !IsConnected( i ) )
			continue;

		if ( g_PlayerExtraInfo[i].teamnumber == teamnumber && g_PlayerExtraInfo[i].dead == false )
			count++;
	}

	return count;
}

int FindBestPlayer( const char *team = NULL )
{
	int best_player   = 0;
	int highest_frags = -99999;
	int lowest_deaths = 99999;
	for ( int i = 1; i < MAX_PLAYERS; i++ )
	{
		if ( g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].frags >= highest_frags )
		{
			if ( !team || !stricmp( g_PlayerExtraInfo[i].teamname, team ) ) // make sure it is the specified team
			{
				extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
				if ( pl_info->frags > highest_frags || pl_info->deaths < lowest_deaths )
				{
					best_player   = i;
					lowest_deaths = pl_info->deaths;
					highest_frags = pl_info->frags;
				}
			}
		}
	}
	return best_player;
}

// returns the ypos where it finishes drawing
int CHudScoreboard :: DrawPlayers( float list_slot, int nameoffset, const char *team )
{
	// draw the players, in order,  and restricted to team if set
	while ( 1 )
	{
		// Find the top ranking player
		int highest_frags = -99999;	int lowest_deaths = 99999;
		int best_player = 0;

		for ( int i = 1; i < MAX_PLAYERS; i++ )
		{
			if ( g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].frags >= highest_frags )
			{
				if ( !(team && stricmp(g_PlayerExtraInfo[i].teamname, team)) )  // make sure it is the specified team
				{
					extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];
					if ( pl_info->frags > highest_frags || pl_info->deaths < lowest_deaths )
					{
						best_player = i;
						lowest_deaths = pl_info->deaths;
						highest_frags = pl_info->frags;
					}
				}
			}
		}

		if ( !best_player )
			break;

		// draw out the best player
		hud_player_info_t *pl_info = &g_PlayerInfoList[best_player];

		int ypos = ystart + (list_slot * ROW_GAP);

		// check we haven't drawn too far down
		if ( ypos > yend )  // don't draw to close to the lower border
			break;

		int r = 255, g = 255, b = 255;
		float *colors = GetClientColor( best_player );
		r *= colors[0];
		g *= colors[1];
		b *= colors[2];

		if(pl_info->thisplayer) // hey, it's me!
		{
			FillRGBABlend( xstart, ypos, xend - xstart, ROW_GAP, 255, 255, 255, 15 );
		}

		DrawUtils::DrawHudString( NAME_POS_START() + nameoffset, ypos, NAME_POS_END(), pl_info->name, r, g, b );

		// draw bomb( if player have the bomb )
		if( g_PlayerExtraInfo[best_player].dead )
			DrawUtils::DrawHudString( ATTRIB_POS_START(), ypos, ATTRIB_POS_END(), "Dead", r, g, b );
		else if( g_PlayerExtraInfo[best_player].has_c4 )
			DrawUtils::DrawHudString( ATTRIB_POS_START(), ypos, ATTRIB_POS_END(), "Bomb", r, g, b );
		else if ( g_PlayerExtraInfo[best_player].has_defuse_kit )
			DrawUtils::DrawHudString( ATTRIB_POS_START( ), ypos, ATTRIB_POS_END( ), "D. Kit", r, g, b );
		else if( g_PlayerExtraInfo[best_player].vip )
			DrawUtils::DrawHudString( ATTRIB_POS_START(), ypos, ATTRIB_POS_END(), "VIP",  r, g, b );

		// draw health
		if ( !g_PlayerExtraInfo[best_player].dead && g_PlayerExtraInfo[best_player].healthinfo > -1 )
		{
			DrawUtils::DrawHudNumberString( HEALTH_POS_END( ), ypos, HEALTH_POS_START( ), g_PlayerExtraInfo[best_player].healthinfo, r, g, b );
		}

		// draw weapon
		if ( gHUD.m_Scoreboard.m_pWeaponName && !g_PlayerExtraInfo[best_player].dead )
		{
			DrawUtils::DrawHudString( WEAPON_POS_END( ), ypos, WEAPON_POS_START( ), g_PlayerExtraInfo[best_player].weaponName, r, g, b );
		}
		// draw money
		if ( g_PlayerExtraInfo[best_player].account != -1 )
		{
			char moneyInfo[32];
			_snprintf( moneyInfo, sizeof( moneyInfo ), "$%i", g_PlayerExtraInfo[best_player].account );
			DrawUtils::DrawHudStringReverse( MONEY_POS_END( ), ypos, MONEY_POS_START( ), moneyInfo, r, g, b );
		}

		// draw kills (right to left)
		DrawUtils::DrawHudNumberString( KILLS_POS_END(), ypos, KILLS_POS_START(), g_PlayerExtraInfo[best_player].frags, r, g, b );

		// draw deaths
		DrawUtils::DrawHudNumberString( DEATHS_POS_END(), ypos, DEATHS_POS_START(), g_PlayerExtraInfo[best_player].deaths, r, g, b );

		// draw ping & packetloss
		const char *value;
		if( pl_info->ping <= 5  // must be 0, until Xash's bug not fixed
			&& ( value = gEngfuncs.PlayerInfo_ValueForKey( best_player, "*bot" ) )
			&& atoi( value ) > 0 )
		{
			DrawUtils::DrawHudStringReverse( PING_POS_END(), ypos, PING_POS_START(), "BOT", r, g, b );
		}
		else
		{
			static char buf[64];
			sprintf( buf, "%d", pl_info->ping );
			DrawUtils::DrawHudStringReverse( PING_POS_END(), ypos, PING_POS_START(), buf, r, g, b );
		}
	
		pl_info->name = NULL;  // set the name to be NULL, so this client won't get drawn again
		list_slot++;
	}

	list_slot += 2.0f;

	return list_slot;
}


void CHudScoreboard :: GetAllPlayersInfo( void )
{
	for ( int i = 1; i < MAX_PLAYERS; i++ )
	{
		GetPlayerInfo( i, &g_PlayerInfoList[i] );

		if ( g_PlayerInfoList[i].thisplayer )
			m_iPlayerNum = i;  // !!!HACK: this should be initialized elsewhere... maybe gotten from the engine
	}
}

int CHudScoreboard :: MsgFunc_ScoreInfo( const char *pszName, int iSize, void *pbuf )
{
	m_iFlags |= HUD_DRAW;

	BufferReader reader( pszName, pbuf, iSize );
	short cl = reader.ReadByte();
	short frags = reader.ReadShort();
	short deaths = reader.ReadShort();
	short playerclass = reader.ReadShort();
	short teamnumber = reader.ReadShort();

	if ( cl > 0 && cl <= MAX_PLAYERS )
	{
		g_PlayerExtraInfo[cl].frags = frags;
		g_PlayerExtraInfo[cl].deaths = deaths;
		g_PlayerExtraInfo[cl].playerclass = playerclass;
		g_PlayerExtraInfo[cl].teamnumber = teamnumber;
	}

	return 1;
}

// Message handler for TeamInfo message
// accepts two values:
//		byte: client number
//		string: client team name
int CHudScoreboard :: MsgFunc_TeamInfo( const char *pszName, int iSize, void *pbuf )
{
	BufferReader reader( pszName, pbuf, iSize );
	short cl = reader.ReadByte();
	int teamNumber = 0;
	cl_entity_s *pLocal = gEngfuncs.GetLocalPlayer( );

	if ( cl > 0 && cl <= MAX_PLAYERS )
	{
		// set the players team
		char teamName[MAX_TEAM_NAME];
		strncpy( teamName, reader.ReadString(), MAX_TEAM_NAME );

		if ( !strcmp( teamName, "TERRORIST" ) )
		{
			teamNumber               = TEAM_TERRORIST;
			if ( cl == pLocal->index )
				gHUD.m_CHudCFMarks.iTeam = TEAM_TERRORIST;
		}
		else if ( !strcmp( teamName, "CT" ) )
		{
			teamNumber = TEAM_CT;
			if ( cl == pLocal->index )
				gHUD.m_CHudCFMarks.iTeam = TEAM_CT;

		}
		else if( !strcmp( teamName, "SPECTATOR" ) || !strcmp( teamName, "UNASSIGNED" ) )
		{
			teamNumber = TEAM_SPECTATOR;
			strncpy( teamName, "SPECTATOR", MAX_TEAM_NAME );

			if ( cl == pLocal->index )
				gHUD.m_CHudCFMarks.iTeam = TEAM_SPECTATOR;
		}
		// just in case
		else teamNumber = TEAM_UNASSIGNED;

		strncpy( g_PlayerExtraInfo[cl].teamname, teamName, MAX_TEAM_NAME );
		g_PlayerExtraInfo[cl].teamnumber = teamNumber;
	}

	// rebuild the list of teams

	// clear out player counts from teams
	for ( int i = 1; i <= m_iNumTeams; i++ )
	{
		g_TeamInfo[i].players = 0;
	}

	// rebuild the team list
	GetAllPlayersInfo();
	m_iNumTeams = 0;

	for ( int i = 1; i < MAX_PLAYERS; i++ )
	{
		int j;
		//if ( g_PlayerInfoList[i].name == NULL )
		//	continue;

		if ( g_PlayerExtraInfo[i].teamname[0] == 0 )
			continue; // skip over players who are not in a team

		// is this player in an existing team?
		for ( j = 1; j <= m_iNumTeams; j++ )
		{
			if ( g_TeamInfo[j].name[0] == '\0' )
				break;

			if ( !stricmp( g_PlayerExtraInfo[i].teamname, g_TeamInfo[j].name ) )
				break;
		}

		if ( j > m_iNumTeams )
		{
			// they aren't in a listed team, so make a new one
			for ( j = 1; j <= m_iNumTeams; j++ )
			{
				if ( g_TeamInfo[j].name[0] == '\0' )
					break;
			}


			m_iNumTeams = max( j, m_iNumTeams );

			strncpy( g_TeamInfo[j].name, g_PlayerExtraInfo[i].teamname, MAX_TEAM_NAME );
			g_TeamInfo[j].teamnumber = g_PlayerExtraInfo[i].teamnumber;
			g_TeamInfo[j].players = 0;
		}

		g_TeamInfo[j].players++;
	}

	// clear out any empty teams
	for ( int i = 1; i <= m_iNumTeams; i++ )
	{
		if ( g_TeamInfo[i].players < 1 )
			memset( &g_TeamInfo[i], 0, sizeof(team_info_t) );
	}

	return 1;
}

// Message handler for TeamScore message
// accepts three values:
//		string: team name
//		short: teams kills
//		short: teams deaths 
// if this message is never received, then scores will simply be the combined totals of the players.
int CHudScoreboard :: MsgFunc_TeamScore( const char *pszName, int iSize, void *pbuf )
{
	BufferReader reader( pszName, pbuf, iSize );
	char *TeamName = reader.ReadString();
	int i;


	// find the team matching the name
	for ( i = 1; i <= m_iNumTeams; i++ )
	{
		if ( !stricmp( TeamName, g_TeamInfo[i].name ) )
			break;
	}
	if ( i > m_iNumTeams )
		return 1;

	// use this new score data instead of combined player scores
	g_TeamInfo[i].scores_overriden = TRUE;
	g_TeamInfo[i].frags = reader.ReadShort();
	g_TeamInfo[i].deaths = reader.ReadShort();

	
	if ( TeamName[0] == 'T' )
	{
		m_iTeamScore_T = g_TeamInfo[i].frags;
	}
	else if ( TeamName[0] == 'C' )
	{
		m_iTeamScore_CT = g_TeamInfo[i].frags;
	}
	
	
	return 1;
}

void CHudScoreboard :: DeathMsg( int killer, int victim )
{
	// if we were the one killed,  or the world killed us, set the scoreboard to indicate suicide
	if ( victim == m_iPlayerNum || killer == 0 )
	{
		m_iLastKilledBy = killer ? killer : m_iPlayerNum;
		m_fLastKillTime = gHUD.m_flTime + 10;	// display who we were killed by for 10 seconds

		if ( killer == m_iPlayerNum )
			m_iLastKilledBy = m_iPlayerNum;
	}
}

int CHudScoreboard::MsgFunc_HealthInfo(const char* pszName, int iSize, void* pbuf)
{
	BufferReader reader( pszName, pbuf, iSize );
	int i = reader.ReadByte( );
	long healthInfo = reader.ReadLong( );
	g_PlayerExtraInfo[i].healthinfo = healthInfo;
	if ( g_PlayerInfoList[i].thisplayer && g_PlayerExtraInfo[i].healthinfo > 255 && g_PlayerExtraInfo[i].healthinfo != gHUD.m_Health.m_iHealth )
	{
		gHUD.m_Health.m_fFade = FADE_TIME;
		gHUD.m_Health.m_iHealth = g_PlayerExtraInfo[i].healthinfo;
	}
	return 1;
}

int CHudScoreboard::MsgFunc_Account( const char *pszName, int iSize, void *pbuf )
{
	BufferReader reader( pszName, pbuf, iSize );
	int i = reader.ReadByte( );
	long account = reader.ReadLong( );
	g_PlayerExtraInfo[i].account = account;
	return 1;
}

void CHudScoreboard :: UserCmd_ShowScores( void )
{
	m_bForceDraw = false;
	m_bShowscoresHeld = true;
}

void CHudScoreboard :: UserCmd_HideScores( void )
{
	m_bForceDraw = m_bShowscoresHeld = false;
}

void CHudScoreboard	:: UserCmd_ShowScoreboard2()
{
	if( gEngfuncs.Cmd_Argc() != 9 )
	{
		ConsolePrint("showscoreboard2 <xstart> <xend> <ystart> <yend> <r> <g> <b> <a>");
	}
	
	xstart     = atof(gEngfuncs.Cmd_Argv(1)) * ScreenWidth;
	xend       = atof(gEngfuncs.Cmd_Argv(2)) * ScreenWidth;
	ystart     = atof(gEngfuncs.Cmd_Argv(3)) * ScreenHeight;
	yend       = atof(gEngfuncs.Cmd_Argv(4)) * ScreenHeight;
	m_colors.r = atoi(gEngfuncs.Cmd_Argv(5));
	m_colors.b = atoi(gEngfuncs.Cmd_Argv(6));
	m_colors.b = atoi(gEngfuncs.Cmd_Argv(7));
	m_colors.a = atoi(gEngfuncs.Cmd_Argv(8));
	m_bDrawStroke = false;
	m_bForceDraw = true;
}

void CHudScoreboard :: UserCmd_HideScoreboard2()
{
	m_bForceDraw = m_bShowscoresHeld = false; // and disable it
}

void CHudScoreboard::DrawNumber( int number, float x, float y, int r, int g, int b, int a, int textureID, int w, int h )
{
	int textureWidth  = 180;
	int textureHeight = 22;
	int digitWidth    = textureWidth / 10;
	int digitHeight   = textureHeight;
	char numberStr[20];
	snprintf( numberStr, sizeof( numberStr ), "%02d", number );
	int len = strlen( numberStr );

	float scale           = min( (float)w / ( digitWidth * len ), (float)h / digitHeight );
	int scaledDigitWidth  = (int)( digitWidth * scale );
	int scaledDigitHeight = (int)( digitHeight * scale );
	int scaledSpacing     = (int)( scaledDigitWidth * 0.10f );

	int totalWidth = len * ( scaledDigitWidth + scaledSpacing ) - scaledSpacing;

	float startX = ( x * ScreenWidth ) - totalWidth / 2.0f;
	float startY = ( y * ScreenHeight ) - scaledDigitHeight / 2.0f;

	for ( int i = 0; i < len; ++i )
	{
		int digit      = numberStr[i] - '0';
		int digitIndex = digit;
		float digitX   = startX + i * ( scaledDigitWidth + scaledSpacing );
		float digitY   = startY;

		DrawUtils::Draw2DQuad2(
		    digitX, digitY, scaledDigitWidth, scaledDigitHeight, (float)digitIndex / 10.0f, 0.0f, (float)( digitIndex + 1 ) / 10.0f, 1.0f, textureID, r, g, b, a );
	}
}



int CHudScoreboard::DrawTopScoreBoard( float flTime )
{
	if ( g_iUser1 )
		return 1;

	int idx = gEngfuncs.GetLocalPlayer( )->index;

	if ( g_PlayerExtraInfo[idx].dead == true )
		return 1;

	if ( gHUD.m_flTime > m_flNextCache )
	{
		CacheTeamAliveNumber( );
		m_flNextCache = gHUD.m_flTime + 1.0;
	}

	if ( m_iBGIndex == -1 )
		return 1;

	int bgSprite = gHUD.GetSprite( m_iBGIndex );
	wrect_t bgRect   = gHUD.GetSpriteRect( m_iBGIndex );
	int bgHeight     = ( bgRect.bottom - bgRect.top );
	int bgWidth      = ( bgRect.right - bgRect.left );
	int bgY          = 2;
	int bgX          = ( ScreenWidth - bgWidth ) / 2;



	int textWidth_TAlive  = GetHudNumberWidth( m_iNum_S, m_rcNumber_Small, DHN_2DIGITS | DHN_DRAWZERO, m_iTeamAlive_T );
	int textWidth_CTAlive = GetHudNumberWidth( m_iNum_S, m_rcNumber_Small, DHN_2DIGITS | DHN_DRAWZERO, m_iTeamAlive_CT );
	int roundNumber       = m_iTeamScore_Max ? m_iTeamScore_Max : m_iTeamScore_T + m_iTeamScore_CT + 1;

	if ( !m_bIsTeamplay )
	{
		int best_player = FindBestPlayer( );
		m_iTeamScore_CT = g_PlayerExtraInfo[gEngfuncs.GetLocalPlayer( )->index].frags;
		m_iTeamScore_T  = best_player ? g_PlayerExtraInfo[best_player].frags : 0;

		roundNumber = m_iTeamScore_Max ? m_iTeamScore_Max : 0;
	}

	if ( hud_scoreboard->value == 1 )
	{
		if ( bgSprite )
		{
			SPR_Set( bgSprite, 255, 255, 255 );
			SPR_DrawHoles( 0, bgX, bgY, &bgRect );
		}

		int textSprite = gHUD.GetSprite( m_iTextIndex );

		if ( textSprite )
		{
			wrect_t textRect = gHUD.GetSpriteRect( m_iTextIndex );

			SPR_Set( textSprite, 128, 128, 128 );
			SPR_DrawAdditive( 0, ( ScreenWidth - ( textRect.right - textRect.left ) ) / 2, bgY + 29, &textRect );
		}

		int textSprite_T = gHUD.GetSprite( m_iTTextIndex );

		if ( textSprite_T )
		{
			wrect_t textRect = gHUD.GetSpriteRect( m_iTTextIndex );

			SPR_Set( textSprite_T, 128, 128, 128 );
			SPR_DrawAdditive( 0, ( ScreenWidth ) / 2 - 50, bgY + 11, &textRect );
		}

		int textSprite_CT = gHUD.GetSprite( m_iCTTextIndex );

		if ( textSprite_CT )
		{
			wrect_t textRect = gHUD.GetSpriteRect( m_iCTTextIndex );

			SPR_Set( textSprite_CT, 128, 128, 128 );
			SPR_DrawAdditive( 0, ( ScreenWidth ) / 2 + 27, bgY + 11, &textRect );
		}
		if ( roundNumber >= 1000 )
		{
			int textWidth = GetHudNumberWidth( m_iNum_S, m_rcNumber_Small, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, roundNumber );

			if ( textWidth > 0 )
				DrawHudNumber( m_iNum_S, m_rcNumber_Small, ( ScreenWidth - textWidth ) / 2, bgY + 10, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, roundNumber, 128, 128, 128 );
		}
		else if ( roundNumber >= 1000 )
		{
			int textWidth = GetHudNumberWidth( m_iNum_L, m_rcNumber_Large, DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, roundNumber );

			if ( textWidth > 0 )
				DrawHudNumber( m_iNum_L, m_rcNumber_Large, ( ScreenWidth - textWidth ) / 2, bgY + 10, DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, roundNumber, 128, 128, 128 );
		}
		else
		{
			int textWidth = GetHudNumberWidth( m_iNum_L, m_rcNumber_Large, DHN_2DIGITS | DHN_DRAWZERO, roundNumber );

			if ( textWidth > 0 )
				DrawHudNumber( m_iNum_L, m_rcNumber_Large, ( ScreenWidth - textWidth ) / 2, bgY + 10, DHN_2DIGITS | DHN_DRAWZERO, roundNumber, 128, 128, 128 );
		}

		if ( m_iTeamScore_T >= 1000 )
		{
			int textWidth_T = GetHudNumberWidth( m_iNum_S, m_rcNumber_Small, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, m_iTeamScore_T );

			if ( textWidth_T > 0 )
				DrawHudNumber( m_iNum_S, m_rcNumber_Small, ( ScreenWidth ) / 2 - 90, bgY + 11, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, m_iTeamScore_T, 128, 128, 128 );
		}
		else if ( m_iTeamScore_T >= 100 )
		{
			int textWidth_T = GetHudNumberWidth( m_iNum_L, m_rcNumber_Large, DHN_3DIGITS | DHN_DRAWZERO, m_iTeamScore_T );

			if ( textWidth_T > 0 )
				DrawHudNumber( m_iNum_L, m_rcNumber_Large, ( ScreenWidth ) / 2 - 89, bgY + 10, DHN_3DIGITS | DHN_DRAWZERO, m_iTeamScore_T, 128, 128, 128 );
		}
		else
		{
			int textWidth_T = GetHudNumberWidth( m_iNum_L, m_rcNumber_Large, DHN_2DIGITS | DHN_DRAWZERO, m_iTeamScore_T );

			if ( textWidth_T > 0 )
				DrawHudNumber( m_iNum_L, m_rcNumber_Large, ( ScreenWidth ) / 2 - 89, bgY + 10, DHN_2DIGITS | DHN_DRAWZERO, m_iTeamScore_T, 128, 128, 128 );
		}

		if ( m_iTeamScore_CT >= 1000 )
		{
			int textWidth_CT = GetHudNumberWidth( m_iNum_S, m_rcNumber_Small, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, m_iTeamScore_CT );

			if ( textWidth_CT > 0 )
				DrawHudNumber( m_iNum_S, m_rcNumber_Small, ( ( ScreenWidth ) / 2 ) + 71 - ( textWidth_CT / 2 ), bgY + 11, DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, m_iTeamScore_CT, 128, 128, 128 );
		}
		else if ( m_iTeamScore_CT >= 100 )
		{
			int textWidth_CT = GetHudNumberWidth( m_iNum_L, m_rcNumber_Large, DHN_3DIGITS | DHN_2DIGITS | DHN_DRAWZERO, m_iTeamScore_CT );

			if ( textWidth_CT > 0 )
				DrawHudNumber( m_iNum_L, m_rcNumber_Large, ( ( ScreenWidth ) / 2 ) + 70 - ( textWidth_CT / 2 ), bgY + 10, DHN_3DIGITS | DHN_DRAWZERO, m_iTeamScore_CT, 128, 128, 128 );
		}
		else
		{
			int textWidth_CT = GetHudNumberWidth( m_iNum_L, m_rcNumber_Large, DHN_2DIGITS | DHN_DRAWZERO, m_iTeamScore_CT );

			if ( textWidth_CT > 0 )
				DrawHudNumber( m_iNum_L, m_rcNumber_Large, ( ( ScreenWidth ) / 2 ) + 73 - ( textWidth_CT / 2 ), bgY + 10, DHN_2DIGITS | DHN_DRAWZERO, m_iTeamScore_CT, 128, 128, 128 );
		}

		if ( m_bIsTeamplay )
		{
			if ( textWidth_TAlive > 0 )
				DrawHudNumber( m_iNum_S, m_rcNumber_Small, ( ScreenWidth ) / 2 - 69, bgY + 30, DHN_2DIGITS | DHN_DRAWZERO, m_iTeamAlive_T, 128, 128, 128 );

			if ( textWidth_CTAlive > 0 )
				DrawHudNumber( m_iNum_S, m_rcNumber_Small, ( ScreenWidth ) / 2 + 47, bgY + 30, DHN_2DIGITS | DHN_DRAWZERO, m_iTeamAlive_CT, 128, 128, 128 );
		}
	}
	else
	{
		DrawUtils::Draw2DQuad2( ScreenWidth * 0.5f - 140, 0, 280, 83, 0.0f, 0.0f, 1.0f, 1.0f, gHUD.m_Scoreboard.csoTexture[CHudScoreboard::CSO_New_Scoreboard::HUD_SCOREBOARD_BACKGROUND], 255, 255, 255, 255 );

		DrawNumber( m_iTeamScore_T, 0.465, 0.035, 255, 255, 255, 255, gHUD.m_Scoreboard.csoTexture[CHudScoreboard::CSO_New_Scoreboard::SB_NUM_RED], 80, 22 );
		DrawNumber( m_iTeamScore_CT, 0.535, 0.035, 255, 255, 255, 255, gHUD.m_Scoreboard.csoTexture[CHudScoreboard::CSO_New_Scoreboard::SB_NUM_BLUE], 80, 22 );

		DrawNumber( roundNumber, 0.5, 0.035, 255, 255, 255, 255, gHUD.m_Scoreboard.csoTexture[CHudScoreboard::CSO_New_Scoreboard::SB_NUM_CENTER], 80, 22 );

		DrawNumber( m_iTeamAlive_T, 0.465, 0.065, 255, 255, 255, 255, gHUD.m_Scoreboard.csoTexture[CHudScoreboard::CSO_New_Scoreboard::SB_NUM_SMALL_RED], 80, 11 );
		DrawNumber( m_iTeamAlive_CT, 0.535, 0.065, 255, 255, 255, 255, gHUD.m_Scoreboard.csoTexture[CHudScoreboard::CSO_New_Scoreboard::SB_NUM_SMALL_BLUE], 80, 11 );

		int minutes = max( 0, (int)( gHUD.m_Timer.m_iTime + gHUD.m_Timer.m_fStartTime - gHUD.m_flTime ) / 60 );
		int seconds = max( 0, (int)( gHUD.m_Timer.m_iTime + gHUD.m_Timer.m_fStartTime - gHUD.m_flTime ) - ( minutes * 60 ) );
		int r = 255, g = 255, b = 255;
		if ( minutes * 60 + seconds > 20 )
		{
			r = 255;
			g = 255;
			b = 255;
		}
		else
		{
			gHUD.m_Timer.m_flPanicTime += gHUD.m_flTimeDelta;
			// add 0.1 sec, so it's not flicker fast
			if ( gHUD.m_Timer.m_flPanicTime > ( (float)seconds / 40.0f ) + 0.1f )
			{
				gHUD.m_Timer.m_flPanicTime = 0;
				gHUD.m_Timer.m_bPanicColorChange = !gHUD.m_Timer.m_bPanicColorChange;
			}
			if ( gHUD.m_Timer.m_bPanicColorChange )
			{
				r = 255;
				g = 0;
				b = 0;
			}
		}

		DrawNumber( minutes, 0.49, 0.065, r, g, b, 255, gHUD.m_Scoreboard.csoTexture[CHudScoreboard::CSO_New_Scoreboard::SB_NUM_WHITE_BOTTOM], 80, 12 );
		DrawNumber( seconds, 0.508, 0.065, r, g, b, 255, gHUD.m_Scoreboard.csoTexture[CHudScoreboard::CSO_New_Scoreboard::SB_NUM_WHITE_BOTTOM], 80, 12 );
	}


	return 1;
}

void CHudScoreboard::CacheTeamAliveNumber( void )
{
	m_iTeamAlive_T  = GetTeamAliveCounts( TEAM_TERRORIST );
	m_iTeamAlive_CT = GetTeamAliveCounts( TEAM_CT );
}

void CHudScoreboard::BuildHudNumberRect( int moe, wrect_t *prc, int w, int h, int xOffset, int yOffset )
{
	wrect_t rc = gHUD.GetSpriteRect( moe );
	int x      = rc.left;
	int y      = rc.top;

	for ( int i = 0; i < 10; i++ )
	{
		prc[i].left   = x;
		prc[i].top    = 0;
		prc[i].right  = prc[i].left + w + xOffset;
		prc[i].bottom = h + yOffset;

		x += w;
		y += h;
	}
}

int CHudScoreboard::DrawHudNumber( int moe, wrect_t *prc, int x, int y, int iFlags, int iNumber, int r, int g, int b )
{
	int iWidth = prc[0].right - prc[0].left;
	int k;
	wrect_t rc;

	if ( iNumber >= 10000 )
	{
		k = iNumber / 10000;
		SPR_Set( gHUD.GetSprite( moe ), r, g, b );
		SPR_DrawAdditive( 0, x, y, &prc[k] );
		x += iWidth;
	}
	else if ( iFlags & ( DHN_5DIGITS ) )
	{
		if ( iFlags & DHN_DRAWZERO )
		{
			SPR_Set( gHUD.GetSprite( moe ), r, g, b );
			SPR_DrawAdditive( 0, x, y, &prc[0] );
		}

		x += iWidth;
	}

	if ( iNumber >= 1000 )
	{
		k = ( iNumber % 10000 ) / 1000;
		SPR_Set( gHUD.GetSprite( moe ), r, g, b );
		SPR_DrawAdditive( 0, x, y, &prc[k] );
		x += iWidth;
	}
	else if ( iFlags & ( DHN_5DIGITS | DHN_4DIGITS ) )
	{
		if ( iFlags & DHN_DRAWZERO )
		{
			SPR_Set( gHUD.GetSprite( moe ), r, g, b );
			SPR_DrawAdditive( 0, x, y, &prc[0] );
		}

		x += iWidth;
	}

	if ( iNumber >= 100 )
	{
		k = ( iNumber % 1000 ) / 100;
		SPR_Set( gHUD.GetSprite( moe ), r, g, b );
		SPR_DrawAdditive( 0, x, y, &prc[k] );
		x += iWidth;
	}
	else if ( iFlags & ( DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS ) )
	{
		if ( iFlags & DHN_DRAWZERO )
		{
			SPR_Set( gHUD.GetSprite( moe ), r, g, b );
			SPR_DrawAdditive( 0, x, y, &prc[0] );
		}

		x += iWidth;
	}

	if ( iNumber >= 10 )
	{
		k  = ( iNumber % 100 ) / 10;
		rc = prc[k];
		SPR_Set( gHUD.GetSprite( moe ), r, g, b );
		SPR_DrawAdditive( 0, x, y, &rc );
		x += iWidth;
	}
	else if ( iFlags & ( DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS ) )
	{
		if ( iFlags & DHN_DRAWZERO )
		{
			SPR_Set( gHUD.GetSprite( moe ), r, g, b );
			SPR_DrawAdditive( 0, x, y, &prc[0] );
		}

		x += iWidth;
	}

	k = iNumber % 10;
	SPR_Set( gHUD.GetSprite( moe ), r, g, b );
	SPR_DrawAdditive( 0, x, y, &prc[k] );
	x += iWidth;

	return x;
}

int CHudScoreboard::GetHudNumberWidth( int moe, wrect_t *prc, int iFlags, int iNumber )
{
	int iWidth = prc[0].right - prc[0].left;
	int k;
	wrect_t rc;
	int x = 0;

	if ( iNumber >= 10000 )
	{
		k = iNumber / 10000;
		x += iWidth;
	}
	else if ( iFlags & ( DHN_5DIGITS ) )
		x += iWidth;

	if ( iNumber >= 1000 )
	{
		k = ( iNumber % 10000 ) / 1000;
		x += iWidth;
	}
	else if ( iFlags & ( DHN_5DIGITS | DHN_4DIGITS ) )
		x += iWidth;

	if ( iNumber >= 100 )
	{
		k = ( iNumber % 1000 ) / 100;
		x += iWidth;
	}
	else if ( iFlags & ( DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS ) )
		x += iWidth;

	if ( iNumber >= 10 )
	{
		k  = ( iNumber % 100 ) / 10;
		rc = prc[k];
		x += iWidth;
	}
	else if ( iFlags & ( DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS ) )
		x += iWidth;

	k = iNumber % 10;
	x += iWidth;

	return x;
}