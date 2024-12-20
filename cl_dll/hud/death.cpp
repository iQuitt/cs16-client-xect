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
#include <cmath>


#define KILLICON_DISPLAY_TIME hud_killicon_display_time->value
#define KILLEFFECT_DISPLAY_TIME hud_killeffect_display_time->value

float color[3];

DECLARE_MESSAGE( m_DeathNotice, DeathMsg )
static const CHudCFMarks::Marks_Type killMarkStarts[] = {
    CHudCFMarks::CF_MARK_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_15TH_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_ANGELICBEAST_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_ARMOREDBEAST_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_BLUENEON_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_BORNBEAST2_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_BORNBEAST2NG_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_CFS2019_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_CHAMPQ9_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_CHRISTIMAS1_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_CHRISTIMAS2_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_DEMONICBEAST_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_DISCO_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_DOMINATOR_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_FURYBEAST_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_FURYBEASTNG_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_GAMINGGLORY_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_GOLDFRAME_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_HALLOWEEN1_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_HALLOWEEN2_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_HERO_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_IRONBEAST2_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_IRONBEAST2NG_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_KAR98KSATELITE_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_M82A1_ANGELICBEAST_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_M82A1_DEMONICBEAST_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_NEWYEAR_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_NIGHTMARE_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_RANKMACH19_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_RANKMACH19_2_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_RANKMACH25_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_RANKMACH25_2_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_RANKMACH26_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_RANKMACH26_2_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_RANKMACH26_3_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_REACTIVEARMORSET_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_REDNANO_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_SUMMER2_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_SUMMER2_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_TIANSHEN_EXPLOSIVE_C4,
    CHudCFMarks::CF_MARK_VALORBEAST_EXPLOSIVE_C4

};
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

int announcerBackground;
int announcerLogobg;

int m_lastSoldier[2];

DeathNoticeItem rgDeathNoticeList[ MAX_DEATHNOTICES + 1 ];

int CHudDeathNotice :: Init( void )
{
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( DeathMsg );

	hud_deathnotice_time = CVAR_CREATE( "hud_deathnotice_time", "6", 0 );
	hud_killicon_display_time = CVAR_CREATE( "hud_killicon_display_time", "1.5", FCVAR_ARCHIVE );
	hud_killeffect_display_time = CVAR_CREATE( "hud_killeffect_display_time", "3", FCVAR_ARCHIVE );
	gHUD.m_AnnouncerIcon.hud_killfx                  = gEngfuncs.pfnRegisterVariable( "hud_killfx", "1", FCVAR_ARCHIVE ); // 1 old cso killfx // 2 new cso killfx // 3 Crossfire Killfx
	gHUD.m_CHudCFMarks.hud_crossfire_killmark_type = gEngfuncs.pfnRegisterVariable( "hud_crossfire_killmark_type", "1", FCVAR_ARCHIVE );
	// 1 default, 2 15th anniversary, 3 Angelic Beast, 4 Armored Beast, 5 Blueneon, 6 Bornbeast2, 7 Bornbeast2NG, 8 CFS_2019, 9 CHAMPQ9, 10 christimas
	// 11 Christmas two, 12 Demonic Beast, 13 Disco, 14 Dominator, 15 Fury Beast, 16 Fury BeastNG,17 Gaming Glory, 18 Halloween, 19 Halloween 2, 20 Hero,
	//21 IronBeast2, 22 IronBeast2NG, 23 Kar98k Satelite, 24 M82A1 Angelic Beast, 25 M82A1 Demonic Beast, 26 New Year 27  Nightmare, 28 Rankmach19, 29 Rankmach19 two, 30 Rankmach25
	// 31 Rankmach25 two, 32 Rankmach26, 33 Rankmach26 Two, 34 Rankmach26 Three, 35 Reactive Armor Set, 36 Red Nano, 37 Summer, 38 Summer 2, 39 TianShen, 40 ValorBeast


	m_iFlags = 0;

	return 1;
}
#define M_PI 3.14159265358979323846 // matches value in gcc v2 math.h



float randomFloat( float min, float max )
{
	return min + static_cast< float >( rand( ) ) / ( static_cast< float >( RAND_MAX / ( max - min ) ) );
}

void CHudCFMarks::CFupdateAnimation( float deltaTime )
{
	if ( !gHUD.m_CHudCFMarks.isAnimating )
		return;

	gHUD.m_CHudCFMarks.animationTime += deltaTime;
	if ( gHUD.m_CHudCFMarks.animationTime >= gHUD.m_CHudCFMarks.ANIMATION_DURATION )
	{
		gHUD.m_CHudCFMarks.isAnimating = false;
		gHUD.m_CHudCFMarks.scaleBg = gHUD.m_CHudCFMarks.scaleBadge = gHUD.m_CHudCFMarks.MAX_SCALE;
		gHUD.m_CHudCFMarks.shakeOffsetX = gHUD.m_CHudCFMarks.shakeOffsetY = 0.0f;
		return;
	}

	float t  = gHUD.m_CHudCFMarks.animationTime / gHUD.m_CHudCFMarks.ANIMATION_DURATION;
	float t2 = 1 - pow( 2, -10 * t ) * cos( t * 10 * M_PI * 0.75 );

	gHUD.m_CHudCFMarks.scaleBg = gHUD.m_CHudCFMarks.scaleBadge = gHUD.m_CHudCFMarks.MIN_SCALE + ( gHUD.m_CHudCFMarks.MAX_SCALE - gHUD.m_CHudCFMarks.MIN_SCALE ) * t2;

	float shakeAmount               = gHUD.m_CHudCFMarks.MAX_SHAKE * ( 1 - t2 );
	float shakePhase                = gHUD.m_CHudCFMarks.animationTime * gHUD.m_CHudCFMarks.SHAKE_FREQUENCY;
	gHUD.m_CHudCFMarks.shakeOffsetX = shakeAmount * ( sin( shakePhase ) + randomFloat( -0.5f, 0.5f ) );
	gHUD.m_CHudCFMarks.shakeOffsetY = shakeAmount * ( cos( shakePhase ) + randomFloat( -0.5f, 0.5f ) );
}




void DrawKillFX( int killfx, int textureID, int width,int height, const char *soundName, float time, bool rowbyrow = false )
{
	if ( killfx == 3 && gHUD.m_AnnouncerIcon.hud_killfx->value == 3 && rowbyrow )
	{
		gHUD.m_CHudCFMarks.textureManager.AddTexture( textureID, time, width, height, rowbyrow ); // crossfire killmark
		gEngfuncs.pfnClientCmd( "speak crossfire_kill" );

		char command[64];
		snprintf( command, sizeof( command ), "speak \"\%s\"", soundName );
		gEngfuncs.pfnClientCmd( command );

	}
	else if ( killfx == 2 && gHUD.m_AnnouncerIcon.hud_killfx->value == 2 )
	{
		gHUD.m_AnnouncerIcon.textureManager.AddTexture( textureID, time, width, height );// new cso style draw texture and sound
		gHUD.m_SoundManager.AddSound( soundName );                       

	}
	else if ( killfx == 1 && gHUD.m_AnnouncerIcon.hud_killfx->value == 1)
	{
		gHUD.m_SoundManager.AddSound( soundName );// old cso style only sound.
	}
	gHUD.m_DeathNotice.m_killIconTime   = gHUD.m_flTime + gHUD.m_DeathNotice.hud_killicon_display_time->value;
	gHUD.m_DeathNotice.m_killEffectTime = gHUD.m_flTime + gHUD.m_DeathNotice.hud_killeffect_display_time->value;
	
}
void CHudDeathNotice :: Reset( void )
{
	m_killNums       = 0;
	m_multiKills     = 0;
	m_multiKillsHS   = 0;
	m_showIcon       = false;
	m_showKill       = false;
	m_iconIndex      = 0;
	m_multiKillsKnife = 0;
	m_killEffectTime = 0;
	m_killIconTime   = 0;
	gHUD.m_CHudCFMarks.m_lastKill    = false;
	gHUD.m_AnnouncerIcon.m_iHeadshot = false;
	gHUD.m_CHudCFMarks.m_bC4Planted  = false;
	gHUD.m_SoundManager.Reset( );
}


void CHudDeathNotice :: InitHUDData( void )
{
	memset( rgDeathNoticeList, 0, sizeof(rgDeathNoticeList) );
}


int CHudDeathNotice :: VidInit( void )
{
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

	// Load new CSO killfx textures
	loadTextures( gHUD.m_AnnouncerIcon.announceTextures,
	              gHUD.m_AnnouncerIcon.texturePaths,
	              CHudAnnouncerIcon::Alarm_Type::ALARM_LAST );


	// 700 kusur texture yuklendigi icin boyle bos bi sorgu koyuyoz xect zombie meate giremeyiz ne yazikki o yuzden bunu ekliyorum
	if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3)
	{
	// Load Crossfire KillMark textures
	loadTextures( gHUD.m_CHudCFMarks.markTextures,
	              gHUD.m_CHudCFMarks.texturePaths,
	              CHudCFMarks::Marks_Type::CF_MARK_UPGRADE3 );
	}	

	announcerBackground = gRenderAPI.GL_LoadTexture( "resource/announceribbon/announceicon/alarm_bg.tga", NULL, 0, TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP );
	announcerLogobg     = gRenderAPI.GL_LoadTexture( "resource/announceribbon/announceicon/alarm_logobg.tga", NULL, 0, TF_NEAREST | TF_NOPICMIP | TF_NOMIPMAP | TF_CLAMP );

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
	int x, y, r, g, b, i,ylbg,xlbg;


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


		gHUD.m_Scoreboard.m_iTeamAlive_T  = gHUD.m_Scoreboard.GetTeamAliveCounts( TEAM_TERRORIST );
		gHUD.m_Scoreboard.m_iTeamAlive_CT = gHUD.m_Scoreboard.GetTeamAliveCounts( TEAM_CT );
		if ( gHUD.m_AnnouncerIcon.hud_killfx->value	)
			gHUD.m_AnnouncerIcon.textureManager.Update( gHUD.m_flTime );

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

	if ( ( m_showKill || m_killNums || gHUD.m_CHudCFMarks.m_bC4Planted ) && gHUD.m_AnnouncerIcon.hud_killfx->value )
	{
		m_killEffectTime            = min( m_killEffectTime, gHUD.m_flTime + KILLEFFECT_DISPLAY_TIME );

		if ( gHUD.m_flTime < m_killEffectTime )
		{
			
				int r = 255, g = 255, b = 255;
				float alpha  = ( m_killEffectTime - gHUD.m_flTime ) / KILLEFFECT_DISPLAY_TIME;
				int numIndex = -1;
			    if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			    {
					if ( gHUD.m_CHudCFMarks.textureManager.HasTextures( ) )
					{
						auto &currentTextureCF = gHUD.m_CHudCFMarks.textureManager.GetCurrentTexture( );
						gHUD.m_CHudCFMarks.CFupdateAnimation( gHUD.m_flTimeDelta );

						float ylbg = ( 25.0f * 0.01f * ScreenHeight ) - 158.0f + 1200 /*y*/ * 0.5f;
						float xlbg = ( 50.0f * 0.01f * ScreenWidth ) - 158.0f + 200 /*x*/ * 0.5f;

						// for revenge and firstblood 
						float y = ( 25.0f * 0.01f * ScreenHeight ) - 158.0f + 1150 /*y*/ * 0.5f;
					    float x = ( 50.0f * 0.01f * ScreenWidth ) - 158.0f + 200 /*x*/ * 0.5f;

						xlbg += gHUD.m_CHudCFMarks.shakeOffsetX;
						ylbg += gHUD.m_CHudCFMarks.shakeOffsetY;

						int badgeWidth  = static_cast< int >( currentTextureCF.textureWidth * gHUD.m_CHudCFMarks.scaleBadge );
						int badgeHeight = static_cast< int >( currentTextureCF.textureHeight * gHUD.m_CHudCFMarks.scaleBadge );

						float centerXlbg = xlbg + 90.0f;
						float centerYlbg = ylbg + 90.0f;
						xlbg             = centerXlbg - badgeWidth / 2.0f;
						ylbg             = centerYlbg - badgeHeight / 2.0f;
					    if ( m_multiKills == 1 || gHUD.m_CHudCFMarks.m_lastKill || gHUD.m_AnnouncerIcon.m_iPayback )
					    {
						    int killMarkType = ( gHUD.m_CHudCFMarks.hud_crossfire_killmark_type->value );

						    int width = 241, height = 33;
						    int textureID;
						    const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
						    killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

						    CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

						    CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 15 );
						  

						    if ( gHUD.m_CHudCFMarks.m_lastKill )
						    {
							    killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 16 );
						    }
				
							if ( gHUD.m_AnnouncerIcon.m_iPayback )
							{
								killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 17 );
								width     = 185;
								height    = 48;
							    gHUD.m_AnnouncerIcon.m_iPayback = false;

							}


						    DrawUtils::Draw2DQuad2( x, y, width, height, 0.0f, 0.0f, 1.0f, 1.0f, gHUD.m_CHudCFMarks.markTextures[killMark], 255, 255, 255, 255 );
					    }

						DrawUtils::Draw2DQuad2( static_cast< int >( xlbg ), static_cast< int >( ylbg ), badgeWidth, badgeHeight, 0.0f, 0.0f, 1.0f, 1.0f, currentTextureCF.textureID, 255, 255, 255, 255 );
					}
			    }
				else if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 2 )
				{
					if ( gHUD.m_AnnouncerIcon.textureManager.HasTextures( ) )
					{
						auto &currentTexture = gHUD.m_AnnouncerIcon.textureManager.GetCurrentTexture( );


						y = ( 25.0 * 0.01 * ScreenHeight ) - currentTexture.textureHeight * 0.5;
						x = ( 50.0 * 0.01 * ScreenWidth ) - currentTexture.textureWidth * 0.5;

						int ybg = ( 25.0 * 0.01 * ScreenHeight ) - 307 + 360 * 0.5;
						int xbg = ( 50.0 * 0.01 * ScreenWidth ) - 248 + 185 * 0.5;

						ylbg = ( 25.0 * 0.01 * ScreenHeight ) - 94 + 300 * 0.5;
						xlbg = ( 50.0 * 0.01 * ScreenWidth ) - 94 - 170 * 0.5;
						// TODO: add text on logobg via using VGUI2 or Imgui
						DrawUtils::Draw2DQuad2( xbg, ybg, 307 /*widthbg*/, 248 /*heightbg*/, 0.0f, 0.0f, 1.0f, 1.0f, announcerBackground, 255, 255, 255, 255 );
						DrawUtils::Draw2DQuad2( xlbg, ylbg, 358, 31, 0.0f, 0.0f, 1.0f, 1.0f, announcerLogobg, 255, 255, 255, 255 );

						DrawUtils::Draw2DQuad2( x, y, currentTexture.textureWidth, currentTexture.textureHeight, 0.0f, 0.0f, 1.0f, 1.0f, currentTexture.textureID, 255, 255, 255, currentTexture.alpha * 255 );
					}
				}
			    else
			    {
				    if ( alpha > 0 && gHUD.m_AnnouncerIcon.hud_killfx->value == 1 )
				    {
					    r *= alpha;
					    g *= alpha;
					    b *= alpha;

					    switch ( m_multiKills )
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

					    if ( numIndex != -1 )
					    {
						    int numWidth, numHeight, iconWidth, iconHeight, textWidth, textHeight;

						    numWidth   = gHUD.GetSpriteRect( numIndex ).right - gHUD.GetSpriteRect( numIndex ).left;
						    numHeight  = gHUD.GetSpriteRect( numIndex ).bottom - gHUD.GetSpriteRect( numIndex ).top;
						    textWidth  = gHUD.GetSpriteRect( m_KM_KillText ).right - gHUD.GetSpriteRect( m_KM_KillText ).left;
						    textHeight = gHUD.GetSpriteRect( m_KM_KillText ).bottom - gHUD.GetSpriteRect( m_KM_KillText ).top;
						    iconWidth  = gHUD.GetSpriteRect( m_KM_Icon_Head ).right - gHUD.GetSpriteRect( m_KM_Icon_Head ).left;
						    iconHeight = gHUD.GetSpriteRect( m_KM_Icon_Head ).bottom - gHUD.GetSpriteRect( m_KM_Icon_Head ).top;

						    if ( m_multiKills == 1 )
							    numWidth += 10;

						    y = ( 25.0 * 0.01 * ScreenHeight ) - ( iconHeight + textHeight ) * 0.5;
						    x = ( 50.0 * 0.01 * ScreenWidth ) - ( numHeight + textWidth ) * 0.5;

						    SPR_Set( gHUD.GetSprite( numIndex ), r, g, b );
						    SPR_DrawAdditive( 0, x, y - ( gHUD.GetSpriteRect( numIndex ).bottom + gHUD.GetSpriteRect( m_KM_KillText ).top - gHUD.GetSpriteRect( m_KM_KillText ).bottom - gHUD.GetSpriteRect( numIndex ).top ) * 0.6, &gHUD.GetSpriteRect( numIndex ) );

						    SPR_Set( gHUD.GetSprite( m_KM_KillText ), r, g, b );
						    SPR_DrawAdditive( 0, x + numWidth, y, &gHUD.GetSpriteRect( m_KM_KillText ) );

						    x = ( 50.0 * 0.01 * ScreenWidth ) - (iconWidth)*0.5;
						    y = y + textHeight;

						    m_killIconTime = min( m_killIconTime, gHUD.m_flTime + KILLICON_DISPLAY_TIME );

						    if ( m_showIcon && gHUD.m_AnnouncerIcon.hud_killfx->value == 1 )
						    {
							    alpha = ( m_killIconTime - gHUD.m_flTime ) / KILLICON_DISPLAY_TIME;

							    if ( alpha > 0 )
							    {
								    r *= alpha;
								    g *= alpha;
								    b *= alpha;
								    switch ( m_iconIndex )
								    {
								    case 1:
								    {
									    SPR_Set( gHUD.GetSprite( m_KM_Icon_Head ), r, g, b );
									    SPR_DrawAdditive( 0, x, y, &gHUD.GetSpriteRect( m_KM_Icon_Head ) );
									    break;
								    }

								    case 2:
								    {
									    SPR_Set( gHUD.GetSprite( m_KM_Icon_Knife ), r, g, b );
									    SPR_DrawAdditive( 0, x, y, &gHUD.GetSpriteRect( m_KM_Icon_Knife ) );
									    break;
								    }

								    case 3:
								    {
									    SPR_Set( gHUD.GetSprite( m_KM_Icon_Frag ), r, g, b );
									    SPR_DrawAdditive( 0, x, y, &gHUD.GetSpriteRect( m_KM_Icon_Frag ) );
									    break;
								    }
								    }
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
		gHUD.m_AnnouncerIcon.m_iHeadshot = false;
		gHUD.m_CHudCFMarks.m_lastKill    = false;
		if (gHUD.m_CHudCFMarks.m_bC4Planted )
		{
			gHUD.m_CHudCFMarks.m_bC4Planted = false;
		}


	}
	

	if( i == 0 )
		m_iFlags &= ~HUD_DRAW; // disable hud item

	return 1;
}

// This message handler may be better off elsewhere
int CHudDeathNotice::MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf )
{
	m_iFlags |= HUD_DRAW;

	BufferReader reader( pszName, pbuf, iSize );

	int killer     = reader.ReadByte( );
	int victim     = reader.ReadByte( );
	int headshot   = reader.ReadByte( );
	int multiKills = 0;
	int idx        = gEngfuncs.GetLocalPlayer( )->index;
	int killMarkType = ( gHUD.m_CHudCFMarks.hud_crossfire_killmark_type->value );

	char killedwith[32];
	strncpy( killedwith, "d_", sizeof( killedwith ) );
	strcat( killedwith, reader.ReadString( ) );

	gHUD.m_Scoreboard.DeathMsg( killer, victim );
	gHUD.m_Spectator.DeathMessage( victim );

	int first = 0, back = 0, kingmaker = 1;
	int count[2]{ };
	for ( int i = 0; i < 33; i++ )
	{
		if ( g_PlayerExtraInfo[i].teamnumber == g_PlayerExtraInfo[victim].teamnumber )
		{
			        if ( !( g_PlayerInfoList[first].name && g_PlayerInfoList[first].name[0] != 0 ) )
				        first = i;

			        if ( !( g_PlayerInfoList[back].name && g_PlayerInfoList[back].name[0] != 0 ) )
				        back = i;

			        if ( g_PlayerExtraInfo[i].frags < g_PlayerExtraInfo[back].frags )
				        back = i;

			        if ( g_PlayerExtraInfo[i].frags > g_PlayerExtraInfo[first].frags )
				        first = i;
		}
		else if ( g_PlayerExtraInfo[i].teamnumber == g_PlayerExtraInfo[killer].teamnumber )
		{
			        if ( g_PlayerExtraInfo[i].frags > g_PlayerExtraInfo[killer].frags + 1 )
				        kingmaker = 0;
		}
	}
	int i;
	for ( i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( rgDeathNoticeList[i].iId == 0 )
			break;
	}
	if ( i == MAX_DEATHNOTICES )
	{
		memmove( rgDeathNoticeList, rgDeathNoticeList + 1, sizeof( DeathNoticeItem ) * MAX_DEATHNOTICES );
		i = MAX_DEATHNOTICES - 1;
	}

	gHUD.m_Scoreboard.GetAllPlayersInfo( );

	const char *killer_name = g_PlayerInfoList[killer].name;
	if ( !killer_name )
	{
		killer_name                      = "";
		rgDeathNoticeList[i].szKiller[0] = 0;
	}
	else
	{
		rgDeathNoticeList[i].KillerColor = GetClientColor( killer );
		strncpy( rgDeathNoticeList[i].szKiller, killer_name, MAX_PLAYER_NAME_LENGTH );
		rgDeathNoticeList[i].szKiller[MAX_PLAYER_NAME_LENGTH - 1] = 0;
	}

 const char *victim_name = NULL;
	if ( ( (char)victim ) != -1 )
		victim_name = g_PlayerInfoList[victim].name;
	if ( !victim_name )
	{
		victim_name                      = "";
		rgDeathNoticeList[i].szVictim[0] = 0;
	}
	else
	{
		rgDeathNoticeList[i].VictimColor = GetClientColor( victim );
		strncpy( rgDeathNoticeList[i].szVictim, victim_name, MAX_PLAYER_NAME_LENGTH );
		rgDeathNoticeList[i].szVictim[MAX_PLAYER_NAME_LENGTH - 1] = 0;
	}

	// Is it a non-player object kill?
	if ( ( (char)victim ) == -1 )
	{
		rgDeathNoticeList[i].bNonPlayerKill = true;

		// Store the object's name in the Victim slot (skip the d_ bit)
		strncpy( rgDeathNoticeList[i].szVictim, killedwith + 2, sizeof( killedwith ) );
	}
	else
	{
		if ( killer == victim || killer == 0 )
			rgDeathNoticeList[i].bSuicide = true;

		if ( !strncmp( killedwith, "d_teammate", sizeof( killedwith ) ) )
			rgDeathNoticeList[i].bTeamKill = true;
	}

	for ( int j = 0; j < MAX_DEATHNOTICES; j++ )
	{
		if ( rgDeathNoticeList[j].iId != 0 &&
		     strcmp( rgDeathNoticeList[j].szKiller, victim_name ) == 0 &&
		     strcmp( rgDeathNoticeList[j].szVictim, killer_name ) == 0 )
		{
			gHUD.m_AnnouncerIcon.m_iPayback = true;
			break;
		}
	}
	rgDeathNoticeList[i].iHeadShotId = headshot;

	// Find the sprite in the list
	int spr = gHUD.GetSpriteIndex( killedwith );

	rgDeathNoticeList[i].iId = spr;

	rgDeathNoticeList[i].flDisplayTime = gHUD.m_flTime + hud_deathnotice_time->value;

	if ( rgDeathNoticeList[i].bNonPlayerKill )
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

			if ( !strncmp( killedwith, "d_world", sizeof( killedwith ) ) )
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
			if ( headshot )
				    ConsolePrint( "*** " );
			ConsolePrint( rgDeathNoticeList[i].szKiller );
			ConsolePrint( " killed " );
			ConsolePrint( rgDeathNoticeList[i].szVictim );
		}

		if ( *killedwith && ( *killedwith > 13 ) && strncmp( killedwith, "d_world", sizeof( killedwith ) ) && !rgDeathNoticeList[i].bTeamKill )
		{
			if ( headshot )
				    ConsolePrint( " with a headshot from " );
			else
				    ConsolePrint( " with " );

			ConsolePrint( killedwith + 2 ); // skip over the "d_" part
		}

		if ( headshot )
			ConsolePrint( " ***" );
		ConsolePrint( "\n" );
	}

	if ( !strcmp( killedwith, "d_knife" ) )
	{
		if ( killer == idx )
		{
			m_showIcon  = true;
			m_iconIndex = 2;
			m_killNums++;
			m_multiKills++;
			m_multiKillsKnife++;
			m_showKill = true;

			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{
				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 7 ); // KNIFE0 is 7th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/Knifekill_T" : "crossfire/man/Knifekill_CT", gHUD.m_flTime, true );
				return true;
			}
			else
				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_KNIFE],94,94, "Humililation", gHUD.m_flTime );
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
		if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
		{
			gHUD.m_CHudCFMarks.isAnimating   = true;
			gHUD.m_CHudCFMarks.animationTime = 0.0f;
			gHUD.m_CHudCFMarks.scaleBg = gHUD.m_CHudCFMarks.scaleBadge = gHUD.m_CHudCFMarks.MIN_SCALE;
		}

		if ( headshot )
		{
			m_showIcon                       = true;
			m_iconIndex                      = 1;
			gHUD.m_AnnouncerIcon.m_iHeadshot = true;
			m_showKill                       = true;

			m_multiKillsHS++;
			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{
				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum  = killMarkStarts[killMarkType - 1];
				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 3 ); // HEADSHOT is 3rd

				if ( m_multiKillsHS >= 2 )
				{
					CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 4 ); // HEADSHOT GOLD is 4th
				}

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/Headshot_T" : "crossfire/man/Headshot_CT", gHUD.m_flTime, true );
				return true;
			}
			else
			{
				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_HEADSHOT], 94, 94, "Headshot", gHUD.m_flTime );
			}	
		}


		if ( !strcmp( killedwith, "d_grenade" ) )
		{
			m_showIcon  = true;
			m_iconIndex = 3;
			m_showKill  = true;

			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{
				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 10 ); // KILL6 is 12th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/Grenadekill_T" : "crossfire/man/Grenadekill_CT", gHUD.m_flTime, true );
				return true;
			}
			else
				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_GRENADE], 94, 94, "GotIt", gHUD.m_flTime );
		}

		switch ( m_multiKills )
		{
		case 1:
			m_showKill = true;
			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{

				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type firstKillMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 15 ); // FIRSTKILL is 15th
				CHudCFMarks::Marks_Type killMark     = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 8 );  // KILL1 is 8th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[firstKillMark], 241, 33, nullptr, gHUD.m_flTime, true );
				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/MultiKill_1_T" : "crossfire/man/MultiKill_1_CT", gHUD.m_flTime, true );
			}
			else
				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_FIRSTBLOOD], 94, 94, "FirstKill", gHUD.m_flTime );
			break;

		case 2:
			m_showKill = true;
			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{

				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 9 ); // KILL2 is 9th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/MultiKill_2_T" : "crossfire/man/MultiKill_2_CT", gHUD.m_flTime, true );
			}
			else
				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_2KILL], 94, 94, "DoubleKill", gHUD.m_flTime );
			break;
		case 3:
			m_showKill = true;
			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{

				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 10 ); // KILL3 is 10th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/MultiKill_3_T" : "crossfire/man/MultiKill_3_CT", gHUD.m_flTime, true );
			}
			else
				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_3KILL], 94, 94, "TripleKill", gHUD.m_flTime );
			break;

		case 4:
			m_showKill = true;
			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{
				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 11 ); // KILL4 is 11th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/MultiKill_4_T" : "crossfire/man/MultiKill_4_CT", gHUD.m_flTime, true );
			}
			else
				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_4KILL], 94, 94, "MultiKill", gHUD.m_flTime );
			break;

		case 5:
			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{
				m_showKill                   = true;
				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 12 ); // KILL5 is 12th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/MultiKill_5_T" : "crossfire/man/MultiKill_5_CT", gHUD.m_flTime, true );
			}
			break;

		case 6:
			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{
				m_showKill                   = true;
				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 13 ); // KILL6 is 13th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/MultiKill_6_T" : "crossfire/man/MultiKill_6_CT", gHUD.m_flTime, true );
			}
			break;
		case 7:
			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{
				m_showKill                   = true;
				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 13 ); // KILL6 is 13th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/MultiKill_7_T" : "crossfire/man/MultiKill_7_CT", gHUD.m_flTime, true );
			}
			break;
		case 8:
			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{
				m_showKill                   = true;
				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 13 ); // KILL6 is 13th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, ( gHUD.m_CHudCFMarks.iTeam == TEAM_TERRORIST ) ? "crossfire/man/MultiKill_8_T" : "crossfire/man/MultiKill_8_CT", gHUD.m_flTime, true );
			}
			break;
		default:
			if ( gHUD.m_AnnouncerIcon.hud_killfx->value == 3 )
			{
				m_showKill                   = true;
				const size_t numKillMarkSets = sizeof( killMarkStarts ) / sizeof( killMarkStarts[0] );
				killMarkType                 = max( 1, min( static_cast< int >( numKillMarkSets ), killMarkType ) );

				CHudCFMarks::Marks_Type baseEnum = killMarkStarts[killMarkType - 1];

				CHudCFMarks::Marks_Type killMark = static_cast< CHudCFMarks::Marks_Type >( baseEnum + 13 ); // KILL6 is 13th

				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_CHudCFMarks.markTextures[killMark], 158, 158, nullptr, gHUD.m_flTime, true );
			}
		}

		switch ( m_killNums )
		{
		case 5:
			m_showKill = true;
			DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_EXCELLENT], 94, 94, "Excellent", gHUD.m_flTime );
			break;
		case 10:
			m_showKill = true;
			DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_INCREDIBLE], 94, 94, "Incredible", gHUD.m_flTime );
			break;
		case 15:
			DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_CRAZY], 94, 94, "Crazy", gHUD.m_flTime );
			m_showKill = true;
			break;
		case 20:
			DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_CANTBELIEVE], 94, 94, "CantBelieve", gHUD.m_flTime );
			m_showKill = true;
			break;
		case 25:
			m_showKill = true;
			DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_OUTOFWORLD], 94, 94, "OutOfWorld", gHUD.m_flTime );
			break;
		}

		if ( gHUD.m_AnnouncerIcon.m_iPayback )
		{
			m_showKill = true;
			DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_PAYBACK], 94, 94, "Supply", gHUD.m_flTime );
			gHUD.m_AnnouncerIcon.m_iPayback = false;

			
		}
		if ( gHUD.m_AnnouncerIcon.m_iLastLocalAlive )
		{
			m_showKill = true;
			DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_LASTSOLDIER], 94, 94, "Supply", gHUD.m_flTime );
		}

		if ( first == victim && g_PlayerExtraInfo[first].frags != g_PlayerExtraInfo[back].frags )
		{
			if ( idx == killer )
			{
				m_showKill = true;
				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_KINGMURDER], 94, 94, "Supply", gHUD.m_flTime );
			}
		}

		if ( back == victim && g_PlayerExtraInfo[first].frags != g_PlayerExtraInfo[back].frags )
		{
			if ( idx == killer )
			{
				m_showKill = true;
				DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_BACKMARKER], 94, 94, "Supply", gHUD.m_flTime );
			}
		}
	}

	if ( ( gHUD.m_Scoreboard.m_iTeamAlive_CT ) == 1 || ( gHUD.m_Scoreboard.m_iTeamAlive_T ) == 1 )
	{
		if ( ( gHUD.m_Scoreboard.m_iTeamAlive_CT ) == 1 && g_PlayerExtraInfo[killer].teamnumber == 1 )
		{
			if ( !g_PlayerExtraInfo[killer].dead && idx == killer )
			{
				    m_showKill = true;
				    gHUD.m_CHudCFMarks.m_lastKill = true;
				    DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_THELAST],94,94, "Supply", gHUD.m_flTime );
			}
		}
		else if ( ( gHUD.m_Scoreboard.m_iTeamAlive_T ) == 1 && g_PlayerExtraInfo[killer].teamnumber == 2 )
		{
			if ( !g_PlayerExtraInfo[killer].dead && idx == killer )
			{
				    m_showKill = true;
				    gHUD.m_CHudCFMarks.m_lastKill = true;
				    DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_THELAST],94,94, "Supply", gHUD.m_flTime );
			}
		}
	}

	if ( ( gHUD.m_Scoreboard.m_iTeamAlive_CT - 1 ) == 1 || ( gHUD.m_Scoreboard.m_iTeamAlive_T - 1 ) == 1 )
	{
		for ( int i = 0; i < 33; i++ )
		{
			if ( !( g_PlayerInfoList[i].name && g_PlayerInfoList[i].name[0] != 0 ) )
				    continue;

			if ( ( ( gHUD.m_Scoreboard.m_iTeamAlive_T - 1 ) == 1 && g_PlayerExtraInfo[i].teamnumber == 1 && m_lastSoldier[0] ) || ( ( gHUD.m_Scoreboard.m_iTeamAlive_CT - 1 == 1 ) && g_PlayerExtraInfo[i].teamnumber == 2 && !m_lastSoldier[1] ) )
			{
				    if ( idx == i )
				    {
					        m_showKill = true;
					        DrawKillFX( gHUD.m_AnnouncerIcon.hud_killfx->value, gHUD.m_AnnouncerIcon.announceTextures[CHudAnnouncerIcon::Alarm_Type::ALARM_LASTSOLDIER],94,94, "Supply", gHUD.m_flTime );
				    }
				    m_lastSoldier[g_PlayerExtraInfo[i].teamnumber - 1] = 1;
				    break;
			}
		}
	}

	return 1;
}
