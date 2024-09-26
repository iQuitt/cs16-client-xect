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
//  hud.h
//
// class CHud declaration
//
// CHud handles the message, calculation, and drawing the HUD
//
#pragma once

#define RGB_YELLOWISH 0x00FFA000 //255,160,0
#define RGB_REDISH 0x00FF1010 //255,16,16
#define RGB_GREENISH 0x0000A000 //0,160,0
#define RGB_WHITE 0x00FFFFFF

#include <assert.h>
#include <string.h>
#include "wrect.h"
#include "cl_dll.h"
#include "ammo.h"
#include <vector>
#include <string>
#include "csprite.h"
#include "cvardef.h"


#define MIN_ALPHA	 100	
#define	HUDELEM_ACTIVE	1
#define CHudMsgFunc(x) int MsgFunc_##x(const char *pszName, int iSize, void *buf)
#define CHudUserCmd(x) void UserCmd_##x()
#define TMSG( x ) !strcmp( szMessage, x )
#define RMSG( x ) !strcmp( szMessage, x )
#define WIN_TEAM_TR 2
#define WIN_TEAM_CT 1
#define WIN_TEAM_NONE 0



class RGBA
{
public:
	unsigned char r, g, b, a;
};

enum 
{ 
	MAX_PLAYERS = 33, // including the bomb
	MAX_TEAMS = 3,
	MAX_TEAM_NAME = 16,
	MAX_HOSTAGES = 24,
};

extern const char *sPlayerModelFiles[];
extern wrect_t nullrc;

class CClientSprite;

inline bool BIsValidTModelIndex( int i )
{
	if ( i == 1 || i == 5 || i == 6 || i == 8 || i == 11 )
		return true;
	else
		return false;
}

inline bool BIsValidCTModelIndex( int i )
{
	if ( i == 7 || i == 2 || i == 10 || i == 4 || i == 9)
		return true;
	else
		return false;
}

#define HUD_DRAW         (1 << 0)
#define HUD_THINK        (1 << 1)
#define HUD_ACTIVE       (HUD_DRAW | HUD_THINK)
#define HUD_INTERMISSION (1 << 2)

#define MAX_PLAYER_NAME_LENGTH		32

#define	MAX_MOTD_LENGTH				1536

//
//-----------------------------------------------------
//
class CHudBase
{
public:
	int	  m_iFlags; // active, moving,
	virtual		~CHudBase() {}
	virtual int Init( void ) {return 0;}
	virtual int VidInit( void ) {return 0;}
	virtual int Draw(float flTime) {return 0;}
	virtual void Think(void) {return;}
	virtual void Reset(void) {return;}
	virtual void InitHUDData( void ) {}		// called every time a server is connected to
	virtual void Shutdown( void ) {}

};

struct HUDLIST {
	CHudBase	*p;
	HUDLIST		*pNext;
};



//
//-----------------------------------------------------
//
//#include "voice_status.h"
#include "hud_spectator.h"


//
//-----------------------------------------------------
//
class CHudAmmo: public CHudBase
{
	friend class WeaponsResource;
	friend class HistoryResource;

public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Think(void);
	void Reset(void);

	void DrawCrosshair(float flTime);
	void CalcCrosshairSize();
	void CalcCrosshairDrawMode();
	void CalcCrosshairColor();

	int DrawWList(float flTime);
	CHudMsgFunc(CurWeapon);
	CHudMsgFunc(WeaponList);
	CHudMsgFunc(AmmoX);
	CHudMsgFunc(AmmoPickup);
	CHudMsgFunc(WeapPickup);
	CHudMsgFunc(ItemPickup);
	CHudMsgFunc(HideWeapon);
	CHudMsgFunc(Crosshair);
	CHudMsgFunc(Brass);


	void SlotInput( int iSlot );
	CHudUserCmd(Slot1);
	CHudUserCmd(Slot2);
	CHudUserCmd(Slot3);
	CHudUserCmd(Slot4);
	CHudUserCmd(Slot5);
	CHudUserCmd(Slot6);
	CHudUserCmd(Slot7);
	CHudUserCmd(Slot8);
	CHudUserCmd(Slot9);
	CHudUserCmd(Slot10);
	CHudUserCmd(Close);
	CHudUserCmd(NextWeapon);
	CHudUserCmd(PrevWeapon);
	CHudUserCmd(Adjust_Crosshair);
	CHudUserCmd(Rebuy);
	CHudUserCmd(Autobuy);

private:
	float m_fFade;
	RGBA  m_rgba;
	WEAPON *m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;

	int m_iAlpha;
	int m_R, m_G, m_B;
	int m_cvarR, m_cvarG, m_cvarB;
	int m_iCurrentCrosshair;
	int m_iCrosshairScaleBase;
	float m_flCrosshairDistance;
	bool m_bAdditive;
	bool m_bObserverCrosshair ;
	bool m_bDrawCrosshair;
	int m_iAmmoLastCheck;

	convar_t *m_pClCrosshairColor;
	convar_t *m_pClCrosshairTranslucent;
	convar_t *m_pClCrosshairSize;
	cvar_t *m_pClDynamicCrosshair;
	cvar_t *m_pHud_FastSwitch;
	cvar_t *m_pHud_DrawHistory_Time;

};

//
//-----------------------------------------------------
//

class CHudAmmoSecondary: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	void Reset( void );
	int Draw(float flTime);

	CHudMsgFunc(SecAmmoVal);
	CHudMsgFunc(SecAmmoIcon);

private:
	enum {
		MAX_SEC_AMMO_VALUES = 4
	};

	int m_HUD_ammoicon; // sprite indices
	int m_iAmmoAmounts[MAX_SEC_AMMO_VALUES];
	float m_fFade;
};


#include "health.h"
#include "radar.h"

#define FADE_TIME 100


//
//-----------------------------------------------------
//
class CHudGeiger: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	CHudMsgFunc(Geiger);
	
private:
	int m_iGeigerRange;

};

//
//-----------------------------------------------------
//
class CHudTrain: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	CHudMsgFunc(Train);

private:
	_HSPRITE m_hSprite;
	int m_iPos;

};

//
//-----------------------------------------------------
//
//  MOTD in cs16 must render HTML, so it disabled
//

class CHudMOTD : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );

	CHudMsgFunc(MOTD);
	void Scroll( int dir );
	void Scroll( float amount );
	float scroll;
	bool m_bShow;
	cvar_t *cl_hide_motd;

protected:
	static int MOTD_DISPLAY_TIME;
	char m_szMOTD[ MAX_MOTD_LENGTH ];
	
	int m_iLines;
	int m_iMaxLength;
	bool ignoreThisMotd;
};


class CHudScoreboard: public CHudBase
{
	friend class CHudSpectatorGui;
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );

	int DrawScoreboard( float flTime );
	int DrawTopScoreBoard( float flTime );

	int DrawTeams( float listslot );
	int DrawPlayers( float listslot, int nameoffset = 0, const char *team = NULL ); // returns the ypos where it finishes drawing

	void DeathMsg( int killer, int victim );
	int MsgFunc_HealthInfo( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_Account( const char *pszName, int iSize, void *pbuf );
	void SetScoreboardDefaults( void );
	void GetAllPlayersInfo( void );
	inline int GetTeamAliveCounts( short teamnumber );
	inline bool IsConnected( int playerIndex );
	inline int GetTeamCounts( short teamnumber );


	CHudUserCmd(ShowScores);
	CHudUserCmd(HideScores);
	CHudUserCmd(ShowScoreboard2);
	CHudUserCmd(HideScoreboard2);

	CHudMsgFunc(ScoreInfo);
	CHudMsgFunc(TeamInfo);
	CHudMsgFunc(TeamScore);
	CHudMsgFunc(TeamScores);
	CHudMsgFunc(TeamNames);

	int m_iPlayerNum;
	int m_iNumTeams;
	int m_iTeamScore_Max;
	int m_iTeamScore_T, m_iTeamScore_CT;
	int m_iTeamAlive_T, m_iTeamAlive_CT;


	bool m_bTopScoreBoardEnabled;
	int m_iBGIndex;
	int m_iTextIndex;
	int m_iTTextIndex, m_iCTTextIndex;

	wrect_t m_rcNumber_Large[10];
	wrect_t m_rcNumber_Small[10];

	int m_iOriginalBG;
	int m_iTeamDeathBG;
	int m_iUnitehBG;
	int m_iNum_L;
	int m_iNum_S;
	int m_iText_CT;
	int m_iText_T;
	int m_iText_TR;
	int m_iText_HM;
	int m_iText_ZB;
	int m_iText_1st;
	int m_iText_Kill;
	int m_iText_Round;
	bool m_bIsTeamplay;
	float m_flNextCache;

	void CacheTeamAliveNumber( void );
	void BuildHudNumberRect( int moe, wrect_t *prc, int w, int h, int xOffset, int yOffset );
	int DrawHudNumber( int moe, wrect_t *prc, int x, int y, int iFlags, int iNumber, int r, int g, int b );
	int GetHudNumberWidth( int moe, wrect_t *prc, int iFlags, int iNumber );
	void DrawNumber( int number, float x, float y, int r, int g, int b, int a, int textureID, int w, int h );
	cvar_t *hud_scoreboard;

	enum CSO_New_Scoreboard
	{
		SB_NUM_BLUE = 0,// FOR CT
		SB_NUM_RED,// FOR TR,
		SB_NUM_BIG_WHITE,//
		SB_NUM_WHITE_BOTTOM,//FOR ROUNDTIME
		SB_NUM_SMALL_BLUE,// FOR ALIVE CT
		HUD_SCOREBOARD_BACKGROUND,
		SB_NUM_SMALL_RED, // FOR ALIVE TR
		SB_NUM_CENTER,//ROUND COUNT
		SB_NUM_YELLOW,

	};
	std::vector< int > csoTexture;
	std::vector< std::string > texturePaths =
	{
	    "resource/hud_cso/hud_sb_num_big_blue.tga",
	    "resource/hud_cso/hud_sb_num_big_red.tga",
	    "resource/hud_cso/hud_sb_num_big_white.tga",
	    "resource/hud_cso/hud_sb_num_bottom.tga",
	    "resource/hud_cso/hud_sb_num_small_blue.tga",
		"resource/hud_cso/hud_scoreboard_bg.tga",
	    "resource/hud_cso/hud_sb_num_small_red.tga",
	    "resource/hud_cso/hud_sb_num_center.tga",
	    "resource/hud_cso/hud_cd_num_big_yellow.tga"

	};

private:
	int m_iLastKilledBy;
	int m_fLastKillTime;
	RGBA m_colors;
	bool m_bDrawStroke;
	bool m_bForceDraw; // if called by showscoreboard2
	bool m_bShowscoresHeld;
};

//
//-----------------------------------------------------
//
class CHudStatusBar : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );
	void ParseStatusString( int line_num );

	CHudMsgFunc(StatusText);
	CHudMsgFunc(StatusValue);

protected:
	enum {
		MAX_STATUSTEXT_LENGTH = 128,
		MAX_STATUSBAR_VALUES = 8,
		MAX_STATUSBAR_LINES = 2,
	};

	char m_szStatusText[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];  // a text string describing how the status bar is to be drawn
	char m_szStatusBar[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];	// the constructed bar that is drawn
	int m_iStatusValues[MAX_STATUSBAR_VALUES];  // an array of values for use in the status bar

	int m_bReparseString; // set to TRUE whenever the m_szStatusBar needs to be recalculated

	// an array of colors...one color for each line
	float *m_pflNameColors[MAX_STATUSBAR_LINES];

	cvar_t *hud_centerid;
};

struct extra_player_info_t 
{
	short frags;
	short deaths;
	short playerclass;
	short teamnumber;
	long healthinfo;
	long account;
	char teamname[MAX_TEAM_NAME];
	bool has_c4;
	bool has_defuse_kit;
	bool vip;
	bool dead;
	bool showhealth;
	bool nextflash;
	bool talking;
	Vector origin;
	int health;
	int radarflashes;
	float radarflashtime;
	float radarflashtimedelta;
	char location[32];
};

struct team_info_t 
{
	char name[MAX_TEAM_NAME];
	short frags;
	short deaths;
	short ownteam;
	short players;
	int already_drawn;
	int scores_overriden;
	int sumping;
	int teamnumber;
};

struct hostage_info_t
{
	vec3_t origin;
	float radarflashtimedelta;
	float radarflashtime;
	bool dead;
	bool nextflash;
	int radarflashes;
};

extern hud_player_info_t	g_PlayerInfoList[MAX_PLAYERS+1];	   // player info from the engine
extern extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];   // additional player info sent directly to the client dll
extern team_info_t			g_TeamInfo[MAX_TEAMS+1];
extern hostage_info_t		g_HostageInfo[MAX_HOSTAGES+1];
extern int					g_IsSpectator[MAX_PLAYERS+1];


//
//-----------------------------------------------------
//
class CHudDeathNotice : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );
	CHudMsgFunc(DeathMsg);
	float m_killEffectTime, m_killIconTime;
	int m_killNums, m_multiKills,m_multiKillsHS,m_multiKillsKnife;
	cvar_t *hud_deathnotice_time;
	cvar_t *hud_killicon_display_time;
	cvar_t *hud_killeffect_display_time;
	enum Killfx_Type
	{
		CSO_OLD = 1,
		CSO_NEW,
		CROSSFIRE,
		CS2,      // soon
		WOLFTEAM, // soon
	};

  private:
	int m_HUD_d_skull;  // sprite index of skull icon
	int m_HUD_d_headshot;

	int m_iconIndex;
	bool m_showIcon, m_showKill;
	int m_killBg[3];
	int m_deathBg[3];
	int m_KM_Number0;
	int m_KM_Number1;
	int m_KM_Number2;
	int m_KM_Number3;
	int m_KM_KillText;
	int m_KM_Icon_Head;
	int m_KM_Icon_Knife;
	int m_KM_Icon_Frag;
};



//
//-----------------------------------------------------
//

class SoundManager
{
  private:
	static const int MAX_SOUNDS       = 20;
	static const int MAX_SOUND_LENGTH = 32;
	char soundQueue[MAX_SOUNDS][MAX_SOUND_LENGTH];
	int front;
	int rear;
	int count;
	float lastSoundTime;
	const float SOUND_DELAY = 1.0f;

  public:
	SoundManager( )
	    : front( 0 ), rear( -1 ), count( 0 ), lastSoundTime( 0 )
	{
		for ( int i = 0; i < MAX_SOUNDS; i++ )
		{
			soundQueue[i][0] = '\0';
		}
	}

	void AddSound( const char *sound )
	{
		if ( count < MAX_SOUNDS )
		{
			rear = ( rear + 1 ) % MAX_SOUNDS;
			strncpy( soundQueue[rear], sound, MAX_SOUND_LENGTH - 1 );
			soundQueue[rear][MAX_SOUND_LENGTH - 1] = '\0';
			count++;
		}
	}

	void Update( float currentTime )
	{
		if ( count > 0 && currentTime - lastSoundTime > SOUND_DELAY )
		{
			PlaySound( soundQueue[front] );
			front = ( front + 1 ) % MAX_SOUNDS;
			count--;
			lastSoundTime = currentTime;
		}
	}
	void Reset( )
	{
		front         = 0;
		rear          = -1;
		count         = 0;
		lastSoundTime = 0;
		for ( int i = 0; i < MAX_SOUNDS; i++ )
		{
			soundQueue[i][0] = '\0';
		}
	}

  private:
	void PlaySound( const char *sound )
	{
		char command[MAX_SOUND_LENGTH];
		snprintf( command, sizeof( command ), "speak \"%s\"", sound );
		gEngfuncs.pfnClientCmd( command );
	}
};
class TextureManager
{
  private:
	struct TextureInfo
	{
		int textureID;
		float duration;
		float startTime;
		float alpha;
		bool overlapping;
		int textureWidth;
		int textureHeight;
	};
	std::vector< TextureInfo > textureList;
	float lastDrawTime;
	const float ALARM_DURATION      = 0.5f; // Base duration for each alarm
	const float ALARM_FADE_DURATION = 0.1f; // Fade in/out duration
	bool defaultOverlapping;

  public:
	TextureManager( )
	    : lastDrawTime( 0 ), defaultOverlapping( false ) {}

	void SetDefaultOverlapping( bool overlap )
	{
		defaultOverlapping = overlap;
	}

	void AddTexture( int textureID, float currentTime, int width, int height, bool rowbyrow = false )
	{
		bool runRowbyRow = rowbyrow || defaultOverlapping;
		if ( runRowbyRow )
		{
			textureList.clear( );
			textureList.push_back( { textureID, ALARM_DURATION, currentTime, 1.0f, true, width, height } );
		}
		else
		{
			float startTime = currentTime;
			if ( !textureList.empty( ) )
			{
				TextureInfo &lastTexture = textureList.back( );
				startTime                = lastTexture.startTime + lastTexture.duration;
			}
			textureList.push_back( { textureID, ALARM_DURATION, startTime, 0.0f, false, width, height } );
		}
	}

	void Update( float currentTime )
	{
		if ( textureList.empty( ) )
			return;
		TextureInfo &currentTexture = textureList.front( );
		float elapsedTime           = currentTime - currentTexture.startTime;
		if ( currentTexture.overlapping )
		{
			if ( elapsedTime > currentTexture.duration )
			{
				textureList.clear( );
			}
		}
		else
		{
			if ( elapsedTime <= ALARM_FADE_DURATION )
			{
				currentTexture.alpha = elapsedTime / ALARM_FADE_DURATION; // Fade in
			}
			else if ( elapsedTime > currentTexture.duration - ALARM_FADE_DURATION )
			{
				currentTexture.alpha = ( currentTexture.duration - elapsedTime ) / ALARM_FADE_DURATION; // Fade out
			}
			else
			{
				currentTexture.alpha = 1.0f;
			}
			if ( elapsedTime > currentTexture.duration )
			{
				textureList.erase( textureList.begin( ) );
				if ( !textureList.empty( ) )
				{
					textureList.front( ).startTime = currentTime;
				}
			}
		}
	}

	bool HasTextures( ) const
	{
		return !textureList.empty( );
	}

	const TextureInfo &GetCurrentTexture( ) const
	{
		return textureList.front( );
	}
};


class CHudCS2 : public CHudBase
{
	enum CS2Resources
	{
		CS2_ICO_D_57,
		CS2_ICO_57,
		CS2_ICO_AK47,
		CS2_ICO_D_AK47,

		CS2_ARMOR,
		CS2_ARMOR_B,
		CS2_BOMB_C4,
		CS2_BOMB_ICON,
		CS2_BULLET,
		CS2_BULLET_H,
		CS2_BULLET2,
		CS2_BULLETPACK,
		CS2_BUYZONE,
		CS2_C4_TIMER,
		CS2_DOLLAR_SIGN,
		CS2_HEALTH_ICON,
		CS2_HUDBG_CT,
		CS2_HUDBG_TR,
		CS2_ICO_ARMOR,
		CS2_ICO_ARMORHELM,
		CS2_ICO_DOLLAR,
		CS2_ICO_HOSTAGE_ZONE,
		CS2_ICO_BOMBZONE_A,
		CS2_ICO_BOMBZONE_B,
		CS2_TIMER,
		CS2_WEAPONSLOT,
		CS2_WEAPON_BG_ACTIVE,
		CS2_WEAPON_BG_INACTIVE,
		// to be continued...
	};
};
class CHudCFMarks : public CHudBase
{
  public:
	float scaleBg       = 1.0f;
	float scaleBadge    = 1.0f;
	float shakeOffsetX  = 0.0f;
	float shakeOffsetY  = 0.0f;
	float animationTime = 0.0f;
	bool isAnimating    = false;
	int iTeam;
	bool m_lastKill;
	bool m_bC4Planted;
	const float ANIMATION_DURATION = 0.5f;
	const float MIN_SCALE          = 0.5f;
	const float MAX_SCALE          = 1.0f;
	const float MAX_SHAKE          = 10.0f;
	const float SHAKE_FREQUENCY    = 20.0f;

	std::vector< int > markTextures; // kill mark textureid
	TextureManager textureManager;
	cvar_t *hud_crossfire_killmark_type;
	void CFupdateAnimation( float deltaTime );
	enum Marks_Type
	{
		CF_MARK_EXPLOSIVE_C4, // Crossfire 15th aniversary kill marks
		CF_MARK_DEFUSE_C4,
		CF_MARK_GRENADE,
		CF_MARK_HEADSHOT,
		CF_MARK_HEADSHOT_GOLD,
		CF_MARK_HEADWALLS,
		CF_MARK_HEADWALLS_GOLD,
		CF_MARK_KNIFE0,
		CF_MARK_KILL1,
		CF_MARK_KILL2,
		CF_MARK_KILL3,
		CF_MARK_KILL4,
		CF_MARK_KILL5,
		CF_MARK_KILL6,
		CF_MARK_WALLSHOT,
		CF_MARK_FIRSTKILL,
		CF_MARK_LASTKILL,
		CF_MARK_REVENGE,

		CF_MARK_15TH_EXPLOSIVE_C4,//Crossfire 15th aniversary kill marks
		CF_MARK_15TH_DEFUSE_C4,
		CF_MARK_15TH_GRENADE,
		CF_MARK_15TH_HEADSHOT,
		CF_MARK_15TH_HEADSHOT_GOLD,
		CF_MARK_15TH_HEADWALLS,
		CF_MARK_15TH_HEADWALLS_GOLD,
		CF_MARK_15TH_KNIFE0,
		CF_MARK_15TH_KILL1,
		CF_MARK_15TH_KILL2,
		CF_MARK_15TH_KILL3,
		CF_MARK_15TH_KILL4,
		CF_MARK_15TH_KILL5,
		CF_MARK_15TH_KILL6,
		CF_MARK_15TH_WALLSHOT,
		CF_MARK_15TH_FIRSTKILL,
		CF_MARK_15TH_LASTKILL,
		CF_MARK_15TH_REVENGE,

		CF_MARK_ANGELICBEAST_EXPLOSIVE_C4,// Crossfire Angelicbeast kill marks
		CF_MARK_ANGELICBEAST_DEFUSE_C4,
		CF_MARK_ANGELICBEAST_GRENADE,
		CF_MARK_ANGELICBEAST_HEADSHOT,
		CF_MARK_ANGELICBEAST_HEADSHOT_GOLD,
		CF_MARK_ANGELICBEAST_HEADWALLS,
		CF_MARK_ANGELICBEAST_HEADWALLS_GOLD,
		CF_MARK_ANGELICBEAST_KNIFE0,
		CF_MARK_ANGELICBEAST_KILL1,
		CF_MARK_ANGELICBEAST_KILL2,
		CF_MARK_ANGELICBEAST_KILL3,
		CF_MARK_ANGELICBEAST_KILL4,
		CF_MARK_ANGELICBEAST_KILL5,
		CF_MARK_ANGELICBEAST_KILL6,
		CF_MARK_ANGELICBEAST_WALLSHOT,
		CF_MARK_ANGELICBEAST_FIRSTKILL,
		CF_MARK_ANGELICBEAST_LASTKILL,
		CF_MARK_ANGELICBEAST_REVENGE,

		CF_MARK_ARMOREDBEAST_EXPLOSIVE_C4, // Crossfire Armoredbeast kill marks
		CF_MARK_ARMOREDBEAST_DEFUSE_C4,
		CF_MARK_ARMOREDBEAST_GRENADE,
		CF_MARK_ARMOREDBEAST_HEADSHOT,
		CF_MARK_ARMOREDBEAST_HEADSHOT_GOLD,
		CF_MARK_ARMOREDBEAST_HEADWALLS,
		CF_MARK_ARMOREDBEAST_HEADWALLS_GOLD,
		CF_MARK_ARMOREDBEAST_KNIFE0,
		CF_MARK_ARMOREDBEAST_KILL1,
		CF_MARK_ARMOREDBEAST_KILL2,
		CF_MARK_ARMOREDBEAST_KILL3,
		CF_MARK_ARMOREDBEAST_KILL4,
		CF_MARK_ARMOREDBEAST_KILL5,
		CF_MARK_ARMOREDBEAST_KILL6,
		CF_MARK_ARMOREDBEAST_WALLSHOT,
		CF_MARK_ARMOREDBEAST_FIRSTKILL,
		CF_MARK_ARMOREDBEAST_LASTKILL,
		CF_MARK_ARMOREDBEAST_REVENGE,

		CF_MARK_BLUENEON_EXPLOSIVE_C4, // Crossfire Blueneon kill marks
		CF_MARK_BLUENEON_DEFUSE_C4,
		CF_MARK_BLUENEON_GRENADE,
		CF_MARK_BLUENEON_HEADSHOT,
		CF_MARK_BLUENEON_HEADSHOT_GOLD,
		CF_MARK_BLUENEON_HEADWALLS,
		CF_MARK_BLUENEON_HEADWALLS_GOLD,
		CF_MARK_BLUENEON_KNIFE0,
		CF_MARK_BLUENEON_KILL1,
		CF_MARK_BLUENEON_KILL2,
		CF_MARK_BLUENEON_KILL3,
		CF_MARK_BLUENEON_KILL4,
		CF_MARK_BLUENEON_KILL5,
		CF_MARK_BLUENEON_KILL6,
		CF_MARK_BLUENEON_WALLSHOT,
		CF_MARK_BLUENEON_FIRSTKILL,
		CF_MARK_BLUENEON_LASTKILL,
		CF_MARK_BLUENEON_REVENGE,

		CF_MARK_BORNBEAST2_EXPLOSIVE_C4, // Crossfire Bornbeast2 kill marks
		CF_MARK_BORNBEAST2_DEFUSE_C4,
		CF_MARK_BORNBEAST2_GRENADE,
		CF_MARK_BORNBEAST2_HEADSHOT,
		CF_MARK_BORNBEAST2_HEADSHOT_GOLD,
		CF_MARK_BORNBEAST2_HEADWALLS,
		CF_MARK_BORNBEAST2_HEADWALLS_GOLD,
		CF_MARK_BORNBEAST2_KNIFE0,
		CF_MARK_BORNBEAST2_KILL1,
		CF_MARK_BORNBEAST2_KILL2,
		CF_MARK_BORNBEAST2_KILL3,
		CF_MARK_BORNBEAST2_KILL4,
		CF_MARK_BORNBEAST2_KILL5,
		CF_MARK_BORNBEAST2_KILL6,
		CF_MARK_BORNBEAST2_WALLSHOT,
		CF_MARK_BORNBEAST2_FIRSTKILL,
		CF_MARK_BORNBEAST2_LASTKILL,
		CF_MARK_BORNBEAST2_REVENGE,

		CF_MARK_BORNBEAST2NG_EXPLOSIVE_C4, // Crossfire Bornbeast2ng kill marks
		CF_MARK_BORNBEAST2NG_DEFUSE_C4,
		CF_MARK_BORNBEAST2NG_GRENADE,
		CF_MARK_BORNBEAST2NG_HEADSHOT,
		CF_MARK_BORNBEAST2NG_HEADSHOT_GOLD,
		CF_MARK_BORNBEAST2NG_HEADWALLS,
		CF_MARK_BORNBEAST2NG_HEADWALLS_GOLD,
		CF_MARK_BORNBEAST2NG_KNIFE0,
		CF_MARK_BORNBEAST2NG_KILL1,
		CF_MARK_BORNBEAST2NG_KILL2,
		CF_MARK_BORNBEAST2NG_KILL3,
		CF_MARK_BORNBEAST2NG_KILL4,
		CF_MARK_BORNBEAST2NG_KILL5,
		CF_MARK_BORNBEAST2NG_KILL6,
		CF_MARK_BORNBEAST2NG_WALLSHOT,
		CF_MARK_BORNBEAST2NG_FIRSTKILL,
		CF_MARK_BORNBEAST2NG_LASTKILL,
		CF_MARK_BORNBEAST2NG_REVENGE,

		CF_MARK_CFS2019_EXPLOSIVE_C4, // Crossfire CFS 2019 kill marks
		CF_MARK_CFS2019_DEFUSE_C4,
		CF_MARK_CFS2019_GRENADE,
		CF_MARK_CFS2019_HEADSHOT,
		CF_MARK_CFS2019_HEADSHOT_GOLD,
		CF_MARK_CFS2019_HEADWALLS,
		CF_MARK_CFS2019_HEADWALLS_GOLD,
		CF_MARK_CFS2019_KNIFE0,
		CF_MARK_CFS2019_KILL1,
		CF_MARK_CFS2019_KILL2,
		CF_MARK_CFS2019_KILL3,
		CF_MARK_CFS2019_KILL4,
		CF_MARK_CFS2019_KILL5,
		CF_MARK_CFS2019_KILL6,
		CF_MARK_CFS2019_WALLSHOT,
		CF_MARK_CFS2019_FIRSTKILL,
		CF_MARK_CFS2019_LASTKILL,
		CF_MARK_CFS2019_REVENGE,

		CF_MARK_CHAMPQ9_EXPLOSIVE_C4, // Crossfire ChampQ9 kill marks
		CF_MARK_CHAMPQ9_DEFUSE_C4,
		CF_MARK_CHAMPQ9_GRENADE,
		CF_MARK_CHAMPQ9_HEADSHOT,
		CF_MARK_CHAMPQ9_HEADSHOT_GOLD,
		CF_MARK_CHAMPQ9_HEADWALLS,
		CF_MARK_CHAMPQ9_HEADWALLS_GOLD,
		CF_MARK_CHAMPQ9_KNIFE0,
		CF_MARK_CHAMPQ9_KILL1,
		CF_MARK_CHAMPQ9_KILL2,
		CF_MARK_CHAMPQ9_KILL3,
		CF_MARK_CHAMPQ9_KILL4,
		CF_MARK_CHAMPQ9_KILL5,
		CF_MARK_CHAMPQ9_KILL6,
		CF_MARK_CHAMPQ9_WALLSHOT,
		CF_MARK_CHAMPQ9_FIRSTKILL,
		CF_MARK_CHAMPQ9_LASTKILL,
		CF_MARK_CHAMPQ9_REVENGE,

		CF_MARK_CHRISTIMAS1_EXPLOSIVE_C4,  // Crossfire Christimas 1 kill marks
		CF_MARK_CHRISTIMAS1_DEFUSE_C4,
		CF_MARK_CHRISTIMAS1_GRENADE,
		CF_MARK_CHRISTIMAS1_HEADSHOT,
		CF_MARK_CHRISTIMAS1_HEADSHOT_GOLD,
		CF_MARK_CHRISTIMAS1_HEADWALLS,
		CF_MARK_CHRISTIMAS1_HEADWALLS_GOLD,
		CF_MARK_CHRISTIMAS1_KNIFE0,
		CF_MARK_CHRISTIMAS1_KILL1,
		CF_MARK_CHRISTIMAS1_KILL2,
		CF_MARK_CHRISTIMAS1_KILL3,
		CF_MARK_CHRISTIMAS1_KILL4,
		CF_MARK_CHRISTIMAS1_KILL5,
		CF_MARK_CHRISTIMAS1_KILL6,
		CF_MARK_CHRISTIMAS1_WALLSHOT,
		CF_MARK_CHRISTIMAS1_FIRSTKILL,
		CF_MARK_CHRISTIMAS1_LASTKILL,
		CF_MARK_CHRISTIMAS1_REVENGE,
		 
		CF_MARK_CHRISTIMAS2_EXPLOSIVE_C4, // Crossfire Christimas 2 kill marks
		CF_MARK_CHRISTIMAS2_DEFUSE_C4,
		CF_MARK_CHRISTIMAS2_GRENADE,
		CF_MARK_CHRISTIMAS2_HEADSHOT,
		CF_MARK_CHRISTIMAS2_HEADSHOT_GOLD,
		CF_MARK_CHRISTIMAS2_HEADWALLS,
		CF_MARK_CHRISTIMAS2_HEADWALLS_GOLD,
		CF_MARK_CHRISTIMAS2_KNIFE0,
		CF_MARK_CHRISTIMAS2_KILL1,
		CF_MARK_CHRISTIMAS2_KILL2,
		CF_MARK_CHRISTIMAS2_KILL3,
		CF_MARK_CHRISTIMAS2_KILL4,
		CF_MARK_CHRISTIMAS2_KILL5,
		CF_MARK_CHRISTIMAS2_KILL6,
		CF_MARK_CHRISTIMAS2_WALLSHOT,
		CF_MARK_CHRISTIMAS2_FIRSTKILL,
		CF_MARK_CHRISTIMAS2_LASTKILL,
		CF_MARK_CHRISTIMAS2_REVENGE,

		CF_MARK_DEMONICBEAST_EXPLOSIVE_C4, // Crossfire Demonicbeast kill marks
		CF_MARK_DEMONICBEAST_DEFUSE_C4,
		CF_MARK_DEMONICBEAST_GRENADE,
		CF_MARK_DEMONICBEAST_HEADSHOT,
		CF_MARK_DEMONICBEAST_HEADSHOT_GOLD,
		CF_MARK_DEMONICBEAST_HEADWALLS,
		CF_MARK_DEMONICBEAST_HEADWALLS_GOLD,
		CF_MARK_DEMONICBEAST_KNIFE0,
		CF_MARK_DEMONICBEAST_KILL1,
		CF_MARK_DEMONICBEAST_KILL2,
		CF_MARK_DEMONICBEAST_KILL3,
		CF_MARK_DEMONICBEAST_KILL4,
		CF_MARK_DEMONICBEAST_KILL5,
		CF_MARK_DEMONICBEAST_KILL6,
		CF_MARK_DEMONICBEAST_WALLSHOT,
		CF_MARK_DEMONICBEAST_FIRSTKILL,
		CF_MARK_DEMONICBEAST_LASTKILL,
		CF_MARK_DEMONICBEAST_REVENGE,

		CF_MARK_DISCO_EXPLOSIVE_C4, // Crossfire Disco kill marks
		CF_MARK_DISCO_DEFUSE_C4,
		CF_MARK_DISCO_GRENADE,
		CF_MARK_DISCO_HEADSHOT,
		CF_MARK_DISCO_HEADSHOT_GOLD,
		CF_MARK_DISCO_HEADWALLS,
		CF_MARK_DISCO_HEADWALLS_GOLD,
		CF_MARK_DISCO_KNIFE0,
		CF_MARK_DISCO_KILL1,
		CF_MARK_DISCO_KILL2,
		CF_MARK_DISCO_KILL3,
		CF_MARK_DISCO_KILL4,
		CF_MARK_DISCO_KILL5,
		CF_MARK_DISCO_KILL6,
		CF_MARK_DISCO_WALLSHOT,
		CF_MARK_DISCO_FIRSTKILL,
		CF_MARK_DISCO_LASTKILL,
		CF_MARK_DISCO_REVENGE,

		CF_MARK_DOMINATOR_EXPLOSIVE_C4, // Crossfire Dominator kill marks
		CF_MARK_DOMINATOR_DEFUSE_C4,
		CF_MARK_DOMINATOR_GRENADE,
		CF_MARK_DOMINATOR_HEADSHOT,
		CF_MARK_DOMINATOR_HEADSHOT_GOLD,
		CF_MARK_DOMINATOR_HEADWALLS,
		CF_MARK_DOMINATOR_HEADWALLS_GOLD,
		CF_MARK_DOMINATOR_KNIFE0,
		CF_MARK_DOMINATOR_KILL1,
		CF_MARK_DOMINATOR_KILL2,
		CF_MARK_DOMINATOR_KILL3,
		CF_MARK_DOMINATOR_KILL4,
		CF_MARK_DOMINATOR_KILL5,
		CF_MARK_DOMINATOR_KILL6,
		CF_MARK_DOMINATOR_WALLSHOT,
		CF_MARK_DOMINATOR_FIRSTKILL,
		CF_MARK_DOMINATOR_LASTKILL,
		CF_MARK_DOMINATOR_REVENGE,

		CF_MARK_FURYBEAST_EXPLOSIVE_C4,  // Crossfire Fury Beast kill marks
		CF_MARK_FURYBEAST_DEFUSE_C4,
		CF_MARK_FURYBEAST_GRENADE,
		CF_MARK_FURYBEAST_HEADSHOT,
		CF_MARK_FURYBEAST_HEADSHOT_GOLD,
		CF_MARK_FURYBEAST_HEADWALLS,
		CF_MARK_FURYBEAST_HEADWALLS_GOLD,
		CF_MARK_FURYBEAST_KNIFE0,
		CF_MARK_FURYBEAST_KILL1,
		CF_MARK_FURYBEAST_KILL2,
		CF_MARK_FURYBEAST_KILL3,
		CF_MARK_FURYBEAST_KILL4,
		CF_MARK_FURYBEAST_KILL5,
		CF_MARK_FURYBEAST_KILL6,
		CF_MARK_FURYBEAST_WALLSHOT,
		CF_MARK_FURYBEAST_FIRSTKILL,
		CF_MARK_FURYBEAST_LASTKILL,
		CF_MARK_FURYBEAST_REVENGE,

		CF_MARK_FURYBEASTNG_EXPLOSIVE_C4,  // Crossfire Fury BeastNG kill marks
		CF_MARK_FURYBEASTNG_DEFUSE_C4,
		CF_MARK_FURYBEASTNG_GRENADE,
		CF_MARK_FURYBEASTNG_HEADSHOT,
		CF_MARK_FURYBEASTNG_HEADSHOT_GOLD,
		CF_MARK_FURYBEASTNG_HEADWALLS,
		CF_MARK_FURYBEASTNG_HEADWALLS_GOLD,
		CF_MARK_FURYBEASTNG_KNIFE0,
		CF_MARK_FURYBEASTNG_KILL1,
		CF_MARK_FURYBEASTNG_KILL2,
		CF_MARK_FURYBEASTNG_KILL3,
		CF_MARK_FURYBEASTNG_KILL4,
		CF_MARK_FURYBEASTNG_KILL5,
		CF_MARK_FURYBEASTNG_KILL6,
		CF_MARK_FURYBEASTNG_WALLSHOT,
		CF_MARK_FURYBEASTNG_FIRSTKILL,
		CF_MARK_FURYBEASTNG_LASTKILL,
		CF_MARK_FURYBEASTNG_REVENGE,

		CF_MARK_GAMINGGLORY_EXPLOSIVE_C4, // Crossfire Gaming Lory kill marks
		CF_MARK_GAMINGGLORY_DEFUSE_C4, 
		CF_MARK_GAMINGGLORY_GRENADE,
		CF_MARK_GAMINGGLORY_HEADSHOT,
		CF_MARK_GAMINGGLORY_HEADSHOT_GOLD,
		CF_MARK_GAMINGGLORY_HEADWALLS,
		CF_MARK_GAMINGGLORY_HEADWALLS_GOLD,
		CF_MARK_GAMINGGLORY_KNIFE0,
		CF_MARK_GAMINGGLORY_KILL1,
		CF_MARK_GAMINGGLORY_KILL2,
		CF_MARK_GAMINGGLORY_KILL3,
		CF_MARK_GAMINGGLORY_KILL4,
		CF_MARK_GAMINGGLORY_KILL5,
		CF_MARK_GAMINGGLORY_KILL6,
		CF_MARK_GAMINGGLORY_WALLSHOT,
		CF_MARK_GAMINGGLORY_FIRSTKILL,
		CF_MARK_GAMINGGLORY_LASTKILL,
		CF_MARK_GAMINGGLORY_REVENGE,

		CF_MARK_GOLDFRAME_EXPLOSIVE_C4, // Crossfire Goldframe kill marks
		CF_MARK_GOLDFRAME_DEFUSE_C4,
		CF_MARK_GOLDFRAME_GRENADE,
		CF_MARK_GOLDFRAME_HEADSHOT,
		CF_MARK_GOLDFRAME_HEADSHOT_GOLD,
		CF_MARK_GOLDFRAME_HEADWALLS,
		CF_MARK_GOLDFRAME_HEADWALLS_GOLD,
		CF_MARK_GOLDFRAME_KNIFE0,
		CF_MARK_GOLDFRAME_KILL1,
		CF_MARK_GOLDFRAME_KILL2,
		CF_MARK_GOLDFRAME_KILL3,
		CF_MARK_GOLDFRAME_KILL4,
		CF_MARK_GOLDFRAME_KILL5,
		CF_MARK_GOLDFRAME_KILL6,
		CF_MARK_GOLDFRAME_WALLSHOT,
		CF_MARK_GOLDFRAME_FIRSTKILL,
		CF_MARK_GOLDFRAME_LASTKILL,
		CF_MARK_GOLDFRAME_REVENGE,

		CF_MARK_HALLOWEEN1_EXPLOSIVE_C4, // Crossfire Halloween kill marks
		CF_MARK_HALLOWEEN1_DEFUSE_C4,
		CF_MARK_HALLOWEEN1_GRENADE,
		CF_MARK_HALLOWEEN1_HEADSHOT,
		CF_MARK_HALLOWEEN1_HEADSHOT_GOLD,
		CF_MARK_HALLOWEEN1_HEADWALLS,
		CF_MARK_HALLOWEEN1_HEADWALLS_GOLD,
		CF_MARK_HALLOWEEN1_KNIFE0,
		CF_MARK_HALLOWEEN1_KILL1,
		CF_MARK_HALLOWEEN1_KILL2,
		CF_MARK_HALLOWEEN1_KILL3,
		CF_MARK_HALLOWEEN1_KILL4,
		CF_MARK_HALLOWEEN1_KILL5,
		CF_MARK_HALLOWEEN1_KILL6,
		CF_MARK_HALLOWEEN1_WALLSHOT,
		CF_MARK_HALLOWEEN1_FIRSTKILL,
		CF_MARK_HALLOWEEN1_LASTKILL,
		CF_MARK_HALLOWEEN1_REVENGE,

		CF_MARK_HALLOWEEN2_EXPLOSIVE_C4, // Crossfire Halloween 2 kill marks
		CF_MARK_HALLOWEEN2_DEFUSE_C4,
		CF_MARK_HALLOWEEN2_GRENADE,
		CF_MARK_HALLOWEEN2_HEADSHOT,
		CF_MARK_HALLOWEEN2_HEADSHOT_GOLD,
		CF_MARK_HALLOWEEN2_HEADWALLS,
		CF_MARK_HALLOWEEN2_HEADWALLS_GOLD,
		CF_MARK_HALLOWEEN2_KNIFE0,
		CF_MARK_HALLOWEEN2_KILL1,
		CF_MARK_HALLOWEEN2_KILL2,
		CF_MARK_HALLOWEEN2_KILL3,
		CF_MARK_HALLOWEEN2_KILL4,
		CF_MARK_HALLOWEEN2_KILL5,
		CF_MARK_HALLOWEEN2_KILL6,
		CF_MARK_HALLOWEEN2_WALLSHOT,
		CF_MARK_HALLOWEEN2_FIRSTKILL,
		CF_MARK_HALLOWEEN2_LASTKILL,
		CF_MARK_HALLOWEEN2_REVENGE,

		CF_MARK_HERO_EXPLOSIVE_C4,  // Crossfire hero kill marks
		CF_MARK_HERO_DEFUSE_C4,
		CF_MARK_HERO_GRENADE,
		CF_MARK_HERO_HEADSHOT,
		CF_MARK_HERO_HEADSHOT_GOLD,
		CF_MARK_HERO_HEADWALLS,
		CF_MARK_HERO_HEADWALLS_GOLD,
		CF_MARK_HERO_KNIFE0,
		CF_MARK_HERO_KILL1,
		CF_MARK_HERO_KILL2,
		CF_MARK_HERO_KILL3,
		CF_MARK_HERO_KILL4,
		CF_MARK_HERO_KILL5,
		CF_MARK_HERO_KILL6,
		CF_MARK_HERO_WALLSHOT,
		CF_MARK_HERO_FIRSTKILL,
		CF_MARK_HERO_LASTKILL,
		CF_MARK_HERO_REVENGE,

		CF_MARK_IRONBEAST2_EXPLOSIVE_C4, // Crossfire IronBeast2 kill marks
		CF_MARK_IRONBEAST2_DEFUSE_C4,
		CF_MARK_IRONBEAST2_GRENADE,
		CF_MARK_IRONBEAST2_HEADSHOT,
		CF_MARK_IRONBEAST2_HEADSHOT_GOLD,
		CF_MARK_IRONBEAST2_HEADWALLS,
		CF_MARK_IRONBEAST2_HEADWALLS_GOLD,
		CF_MARK_IRONBEAST2_KNIFE0,
		CF_MARK_IRONBEAST2_KILL1,
		CF_MARK_IRONBEAST2_KILL2,
		CF_MARK_IRONBEAST2_KILL3,
		CF_MARK_IRONBEAST2_KILL4,
		CF_MARK_IRONBEAST2_KILL5,
		CF_MARK_IRONBEAST2_KILL6,
		CF_MARK_IRONBEAST2_WALLSHOT,
		CF_MARK_IRONBEAST2_FIRSTKILL,
		CF_MARK_IRONBEAST2_LASTKILL,
		CF_MARK_IRONBEAST2_REVENGE,

		CF_MARK_IRONBEAST2NG_EXPLOSIVE_C4, // Crossfire IronBeast2NG kill marks
		CF_MARK_IRONBEAST2NG_DEFUSE_C4,
		CF_MARK_IRONBEAST2NG_GRENADE,
		CF_MARK_IRONBEAST2NG_HEADSHOT,
		CF_MARK_IRONBEAST2NG_HEADSHOT_GOLD,
		CF_MARK_IRONBEAST2NG_HEADWALLS,
		CF_MARK_IRONBEAST2NG_HEADWALLS_GOLD,
		CF_MARK_IRONBEAST2NG_KNIFE0,
		CF_MARK_IRONBEAST2NG_KILL1,
		CF_MARK_IRONBEAST2NG_KILL2,
		CF_MARK_IRONBEAST2NG_KILL3,
		CF_MARK_IRONBEAST2NG_KILL4,
		CF_MARK_IRONBEAST2NG_KILL5,
		CF_MARK_IRONBEAST2NG_KILL6,
		CF_MARK_IRONBEAST2NG_WALLSHOT,
		CF_MARK_IRONBEAST2NG_FIRSTKILL,
		CF_MARK_IRONBEAST2NG_LASTKILL,
		CF_MARK_IRONBEAST2NG_REVENGE,

		CF_MARK_KAR98KSATELITE_EXPLOSIVE_C4, // Crossfire Kar98K Satelite kill marks
		CF_MARK_KAR98KSATELITE_DEFUSE_C4,
		CF_MARK_KAR98KSATELITE_GRENADE,
		CF_MARK_KAR98KSATELITE_HEADSHOT,
		CF_MARK_KAR98KSATELITE_HEADSHOT_GOLD,
		CF_MARK_KAR98KSATELITE_HEADWALLS,
		CF_MARK_KAR98KSATELITE_HEADWALLS_GOLD,
		CF_MARK_KAR98KSATELITE_KNIFE0,
		CF_MARK_KAR98KSATELITE_KILL1,
		CF_MARK_KAR98KSATELITE_KILL2,
		CF_MARK_KAR98KSATELITE_KILL3,
		CF_MARK_KAR98KSATELITE_KILL4,
		CF_MARK_KAR98KSATELITE_KILL5,
		CF_MARK_KAR98KSATELITE_KILL6,
		CF_MARK_KAR98KSATELITE_WALLSHOT,
		CF_MARK_KAR98KSATELITE_FIRSTKILL,
		CF_MARK_KAR98KSATELITE_LASTKILL,
		CF_MARK_KAR98KSATELITE_REVENGE,

		CF_MARK_M82A1_ANGELICBEAST_EXPLOSIVE_C4, // Crossfire M82A1 Angelicbeast kill marks
		CF_MARK_M82A1_ANGELICBEAST_DEFUSE_C4,
		CF_MARK_M82A1_ANGELICBEAST_GRENADE,
		CF_MARK_M82A1_ANGELICBEAST_HEADSHOT,
		CF_MARK_M82A1_ANGELICBEAST_HEADSHOT_GOLD,
		CF_MARK_M82A1_ANGELICBEAST_HEADWALLS,
		CF_MARK_M82A1_ANGELICBEAST_HEADWALLS_GOLD,
		CF_MARK_M82A1_ANGELICBEAST_KNIFE0,
		CF_MARK_M82A1_ANGELICBEAST_KILL1,
		CF_MARK_M82A1_ANGELICBEAST_KILL2,
		CF_MARK_M82A1_ANGELICBEAST_KILL3,
		CF_MARK_M82A1_ANGELICBEAST_KILL4,
		CF_MARK_M82A1_ANGELICBEAST_KILL5,
		CF_MARK_M82A1_ANGELICBEAST_KILL6,
		CF_MARK_M82A1_ANGELICBEAST_WALLSHOT,
		CF_MARK_M82A1_ANGELICBEAST_FIRSTKILL,
		CF_MARK_M82A1_ANGELICBEAST_LASTKILL,
		CF_MARK_M82A1_ANGELICBEAST_REVENGE,

		CF_MARK_M82A1_DEMONICBEAST_EXPLOSIVE_C4,// Crossfire M82A1 Demonicbeast kill marks
		CF_MARK_M82A1_DEMONICBEAST_DEFUSE_C4,
		CF_MARK_M82A1_DEMONICBEAST_GRENADE,
		CF_MARK_M82A1_DEMONICBEAST_HEADSHOT,
		CF_MARK_M82A1_DEMONICBEAST_HEADSHOT_GOLD,
		CF_MARK_M82A1_DEMONICBEAST_HEADWALLS,
		CF_MARK_M82A1_DEMONICBEAST_HEADWALLS_GOLD,
		CF_MARK_M82A1_DEMONICBEAST_KNIFE0,
		CF_MARK_M82A1_DEMONICBEAST_KILL1,
		CF_MARK_M82A1_DEMONICBEAST_KILL2,
		CF_MARK_M82A1_DEMONICBEAST_KILL3,
		CF_MARK_M82A1_DEMONICBEAST_KILL4,
		CF_MARK_M82A1_DEMONICBEAST_KILL5,
		CF_MARK_M82A1_DEMONICBEAST_KILL6,
		CF_MARK_M82A1_DEMONICBEAST_WALLSHOT,
		CF_MARK_M82A1_DEMONICBEAST_FIRSTKILL,
		CF_MARK_M82A1_DEMONICBEAST_LASTKILL,
		CF_MARK_M82A1_DEMONICBEAST_REVENGE,

		CF_MARK_NEWYEAR_EXPLOSIVE_C4,// Crossfire M82A1 Newyear kill marks
		CF_MARK_NEWYEAR_DEFUSE_C4,
		CF_MARK_NEWYEAR_GRENADE,
		CF_MARK_NEWYEAR_HEADSHOT,
		CF_MARK_NEWYEAR_HEADSHOT_GOLD,
		CF_MARK_NEWYEAR_HEADWALLS,
		CF_MARK_NEWYEAR_HEADWALLS_GOLD,
		CF_MARK_NEWYEAR_KNIFE0,
		CF_MARK_NEWYEAR_KILL1,
		CF_MARK_NEWYEAR_KILL2,
		CF_MARK_NEWYEAR_KILL3,
		CF_MARK_NEWYEAR_KILL4,
		CF_MARK_NEWYEAR_KILL5,
		CF_MARK_NEWYEAR_KILL6,
		CF_MARK_NEWYEAR_WALLSHOT,
		CF_MARK_NEWYEAR_FIRSTKILL,
		CF_MARK_NEWYEAR_LASTKILL,
		CF_MARK_NEWYEAR_REVENGE,

		CF_MARK_NIGHTMARE_EXPLOSIVE_C4,// Crossfire M82A1 Nightmare kill marks
		CF_MARK_NIGHTMARE_DEFUSE_C4,
		CF_MARK_NIGHTMARE_GRENADE,
		CF_MARK_NIGHTMARE_HEADSHOT,
		CF_MARK_NIGHTMARE_HEADSHOT_GOLD,
		CF_MARK_NIGHTMARE_HEADWALLS,
		CF_MARK_NIGHTMARE_HEADWALLS_GOLD,
		CF_MARK_NIGHTMARE_KNIFE0,
		CF_MARK_NIGHTMARE_KILL1,
		CF_MARK_NIGHTMARE_KILL2,
		CF_MARK_NIGHTMARE_KILL3,
		CF_MARK_NIGHTMARE_KILL4,
		CF_MARK_NIGHTMARE_KILL5,
		CF_MARK_NIGHTMARE_KILL6,
		CF_MARK_NIGHTMARE_WALLSHOT,
		CF_MARK_NIGHTMARE_FIRSTKILL,
		CF_MARK_NIGHTMARE_LASTKILL,
		CF_MARK_NIGHTMARE_REVENGE,

		CF_MARK_RANKMACH19_EXPLOSIVE_C4,// Crossfire RankMach19 kill marks
		CF_MARK_RANKMACH19_DEFUSE_C4,
		CF_MARK_RANKMACH19_GRENADE,
		CF_MARK_RANKMACH19_HEADSHOT,
		CF_MARK_RANKMACH19_HEADSHOT_GOLD,
		CF_MARK_RANKMACH19_HEADWALLS,
		CF_MARK_RANKMACH19_HEADWALLS_GOLD,
		CF_MARK_RANKMACH19_KNIFE0,
		CF_MARK_RANKMACH19_KILL1,
		CF_MARK_RANKMACH19_KILL2,
		CF_MARK_RANKMACH19_KILL3,
		CF_MARK_RANKMACH19_KILL4,
		CF_MARK_RANKMACH19_KILL5,
		CF_MARK_RANKMACH19_KILL6,
		CF_MARK_RANKMACH19_WALLSHOT,
		CF_MARK_RANKMACH19_FIRSTKILL,
		CF_MARK_RANKMACH19_LASTKILL,
		CF_MARK_RANKMACH19_REVENGE,

		CF_MARK_RANKMACH19_2_EXPLOSIVE_C4,// Crossfire RankMach19 2 kill marks
		CF_MARK_RANKMACH19_2_DEFUSE_C4,
		CF_MARK_RANKMACH19_2_GRENADE,
		CF_MARK_RANKMACH19_2_HEADSHOT,
		CF_MARK_RANKMACH19_2_HEADSHOT_GOLD,
		CF_MARK_RANKMACH19_2_HEADWALLS,
		CF_MARK_RANKMACH19_2_HEADWALLS_GOLD,
		CF_MARK_RANKMACH19_2_KNIFE0,
		CF_MARK_RANKMACH19_2_KILL1,
		CF_MARK_RANKMACH19_2_KILL2,
		CF_MARK_RANKMACH19_2_KILL3,
		CF_MARK_RANKMACH19_2_KILL4,
		CF_MARK_RANKMACH19_2_KILL5,
		CF_MARK_RANKMACH19_2_KILL6,
		CF_MARK_RANKMACH19_2_WALLSHOT,
		CF_MARK_RANKMACH19_2_FIRSTKILL,
		CF_MARK_RANKMACH19_2_LASTKILL,
		CF_MARK_RANKMACH19_2_REVENGE,

		CF_MARK_RANKMACH25_EXPLOSIVE_C4,// Crossfire RankMach25 kill marks
		CF_MARK_RANKMACH25_DEFUSE_C4,
		CF_MARK_RANKMACH25_GRENADE,
		CF_MARK_RANKMACH25_HEADSHOT,
		CF_MARK_RANKMACH25_HEADSHOT_GOLD,
		CF_MARK_RANKMACH25_HEADWALLS,
		CF_MARK_RANKMACH25_HEADWALLS_GOLD,
		CF_MARK_RANKMACH25_KNIFE0,
		CF_MARK_RANKMACH25_KILL1,
		CF_MARK_RANKMACH25_KILL2,
		CF_MARK_RANKMACH25_KILL3,
		CF_MARK_RANKMACH25_KILL4,
		CF_MARK_RANKMACH25_KILL5,
		CF_MARK_RANKMACH25_KILL6,
		CF_MARK_RANKMACH25_WALLSHOT,
		CF_MARK_RANKMACH25_FIRSTKILL,
		CF_MARK_RANKMACH25_LASTKILL,
		CF_MARK_RANKMACH25_REVENGE,

		CF_MARK_RANKMACH25_2_EXPLOSIVE_C4,// Crossfire RankMach25 2 kill marks
		CF_MARK_RANKMACH25_2_DEFUSE_C4,
		CF_MARK_RANKMACH25_2_GRENADE,
		CF_MARK_RANKMACH25_2_HEADSHOT,
		CF_MARK_RANKMACH25_2_HEADSHOT_GOLD,
		CF_MARK_RANKMACH25_2_HEADWALLS,
		CF_MARK_RANKMACH25_2_HEADWALLS_GOLD,
		CF_MARK_RANKMACH25_2_KNIFE0,
		CF_MARK_RANKMACH25_2_KILL1,
		CF_MARK_RANKMACH25_2_KILL2,
		CF_MARK_RANKMACH25_2_KILL3,
		CF_MARK_RANKMACH25_2_KILL4,
		CF_MARK_RANKMACH25_2_KILL5,
		CF_MARK_RANKMACH25_2_KILL6,
		CF_MARK_RANKMACH25_2_WALLSHOT,
		CF_MARK_RANKMACH25_2_FIRSTKILL,
		CF_MARK_RANKMACH25_2_LASTKILL,
		CF_MARK_RANKMACH25_2_REVENGE,

		CF_MARK_RANKMACH26_EXPLOSIVE_C4,// Crossfire RankMach26 kill marks
		CF_MARK_RANKMACH26_DEFUSE_C4,
		CF_MARK_RANKMACH26_GRENADE,
		CF_MARK_RANKMACH26_HEADSHOT,
		CF_MARK_RANKMACH26_HEADSHOT_GOLD,
		CF_MARK_RANKMACH26_HEADWALLS,
		CF_MARK_RANKMACH26_HEADWALLS_GOLD,
		CF_MARK_RANKMACH26_KNIFE0,
		CF_MARK_RANKMACH26_KILL1,
		CF_MARK_RANKMACH26_KILL2,
		CF_MARK_RANKMACH26_KILL3,
		CF_MARK_RANKMACH26_KILL4,
		CF_MARK_RANKMACH26_KILL5,
		CF_MARK_RANKMACH26_KILL6,
		CF_MARK_RANKMACH26_WALLSHOT,
		CF_MARK_RANKMACH26_FIRSTKILL,
		CF_MARK_RANKMACH26_LASTKILL,
		CF_MARK_RANKMACH26_REVENGE,

		CF_MARK_RANKMACH26_2_EXPLOSIVE_C4,// Crossfire RankMach26 2 kill marks
		CF_MARK_RANKMACH26_2_DEFUSE_C4,
		CF_MARK_RANKMACH26_2_GRENADE,
		CF_MARK_RANKMACH26_2_HEADSHOT,
		CF_MARK_RANKMACH26_2_HEADSHOT_GOLD,
		CF_MARK_RANKMACH26_2_HEADWALLS,
		CF_MARK_RANKMACH26_2_HEADWALLS_GOLD,
		CF_MARK_RANKMACH26_2_KNIFE0,
		CF_MARK_RANKMACH26_2_KILL1,
		CF_MARK_RANKMACH26_2_KILL2,
		CF_MARK_RANKMACH26_2_KILL3,
		CF_MARK_RANKMACH26_2_KILL4,
		CF_MARK_RANKMACH26_2_KILL5,
		CF_MARK_RANKMACH26_2_KILL6,
		CF_MARK_RANKMACH26_2_WALLSHOT,
		CF_MARK_RANKMACH26_2_FIRSTKILL,
		CF_MARK_RANKMACH26_2_LASTKILL,
		CF_MARK_RANKMACH26_2_REVENGE,

		CF_MARK_RANKMACH26_3_EXPLOSIVE_C4,// Crossfire RankMach26 3 kill marks
		CF_MARK_RANKMACH26_3_DEFUSE_C4,
		CF_MARK_RANKMACH26_3_GRENADE,
		CF_MARK_RANKMACH26_3_HEADSHOT,
		CF_MARK_RANKMACH26_3_HEADSHOT_GOLD,
		CF_MARK_RANKMACH26_3_HEADWALLS,
		CF_MARK_RANKMACH26_3_HEADWALLS_GOLD,
		CF_MARK_RANKMACH26_3_KNIFE0,
		CF_MARK_RANKMACH26_3_KILL1,
		CF_MARK_RANKMACH26_3_KILL2,
		CF_MARK_RANKMACH26_3_KILL3,
		CF_MARK_RANKMACH26_3_KILL4,
		CF_MARK_RANKMACH26_3_KILL5,
		CF_MARK_RANKMACH26_3_KILL6,
		CF_MARK_RANKMACH26_3_WALLSHOT,
		CF_MARK_RANKMACH26_3_FIRSTKILL,
		CF_MARK_RANKMACH26_3_LASTKILL,
		CF_MARK_RANKMACH26_3_REVENGE,

		CF_MARK_REACTIVEARMORSET_EXPLOSIVE_C4,// Crossfire Reactive Armor set kill marks
		CF_MARK_REACTIVEARMORSET_DEFUSE_C4,
		CF_MARK_REACTIVEARMORSET_GRENADE,
		CF_MARK_REACTIVEARMORSET_HEADSHOT,
		CF_MARK_REACTIVEARMORSET_HEADSHOT_GOLD,
		CF_MARK_REACTIVEARMORSET_HEADWALLS,
		CF_MARK_REACTIVEARMORSET_HEADWALLS_GOLD,
		CF_MARK_REACTIVEARMORSET_KNIFE0,
		CF_MARK_REACTIVEARMORSET_KILL1,
		CF_MARK_REACTIVEARMORSET_KILL2,
		CF_MARK_REACTIVEARMORSET_KILL3,
		CF_MARK_REACTIVEARMORSET_KILL4,
		CF_MARK_REACTIVEARMORSET_KILL5,
		CF_MARK_REACTIVEARMORSET_KILL6,
		CF_MARK_REACTIVEARMORSET_WALLSHOT,
		CF_MARK_REACTIVEARMORSET_FIRSTKILL,
		CF_MARK_REACTIVEARMORSET_LASTKILL,
		CF_MARK_REACTIVEARMORSET_REVENGE,

		CF_MARK_REDNANO_EXPLOSIVE_C4,// Crossfire RedNano kill marks
		CF_MARK_REDNANO_DEFUSE_C4,
		CF_MARK_REDNANO_GRENADE,
		CF_MARK_REDNANO_HEADSHOT,
		CF_MARK_REDNANO_HEADSHOT_GOLD,
		CF_MARK_REDNANO_HEADWALLS,
		CF_MARK_REDNANO_HEADWALLS_GOLD,
		CF_MARK_REDNANO_KNIFE0,
		CF_MARK_REDNANO_KILL1,
		CF_MARK_REDNANO_KILL2,
		CF_MARK_REDNANO_KILL3,
		CF_MARK_REDNANO_KILL4,
		CF_MARK_REDNANO_KILL5,
		CF_MARK_REDNANO_KILL6,
		CF_MARK_REDNANO_WALLSHOT,
		CF_MARK_REDNANO_FIRSTKILL,
		CF_MARK_REDNANO_LASTKILL,
		CF_MARK_REDNANO_REVENGE,

		CF_MARK_SUMMER_EXPLOSIVE_C4,// Crossfire Summer kill marks
		CF_MARK_SUMMER_DEFUSE_C4,
		CF_MARK_SUMMER_GRENADE,
		CF_MARK_SUMMER_HEADSHOT,
		CF_MARK_SUMMER_HEADSHOT_GOLD,
		CF_MARK_SUMMER_HEADWALLS,
		CF_MARK_SUMMER_HEADWALLS_GOLD,
		CF_MARK_SUMMER_KNIFE0,
		CF_MARK_SUMMER_KILL1,
		CF_MARK_SUMMER_KILL2,
		CF_MARK_SUMMER_KILL3,
		CF_MARK_SUMMER_KILL4,
		CF_MARK_SUMMER_KILL5,
		CF_MARK_SUMMER_KILL6,
		CF_MARK_SUMMER_WALLSHOT,
		CF_MARK_SUMMER_FIRSTKILL,
		CF_MARK_SUMMER_LASTKILL,
		CF_MARK_SUMMER_REVENGE,

		CF_MARK_SUMMER2_EXPLOSIVE_C4,// Crossfire Summer 2 kill marks
		CF_MARK_SUMMER2_DEFUSE_C4,
		CF_MARK_SUMMER2_GRENADE,
		CF_MARK_SUMMER2_HEADSHOT,
		CF_MARK_SUMMER2_HEADSHOT_GOLD,
		CF_MARK_SUMMER2_HEADWALLS,
		CF_MARK_SUMMER2_HEADWALLS_GOLD,
		CF_MARK_SUMMER2_KNIFE0,
		CF_MARK_SUMMER2_KILL1,
		CF_MARK_SUMMER2_KILL2,
		CF_MARK_SUMMER2_KILL3,
		CF_MARK_SUMMER2_KILL4,
		CF_MARK_SUMMER2_KILL5,
		CF_MARK_SUMMER2_KILL6,
		CF_MARK_SUMMER2_WALLSHOT,
		CF_MARK_SUMMER2_FIRSTKILL,
		CF_MARK_SUMMER2_LASTKILL,
		CF_MARK_SUMMER2_REVENGE,

		CF_MARK_TIANSHEN_EXPLOSIVE_C4,// Crossfire TianShen kill marks
		CF_MARK_TIANSHEN_DEFUSE_C4,
		CF_MARK_TIANSHEN_GRENADE,
		CF_MARK_TIANSHEN_HEADSHOT,
		CF_MARK_TIANSHEN_HEADSHOT_GOLD,
		CF_MARK_TIANSHEN_HEADWALLS,
		CF_MARK_TIANSHEN_HEADWALLS_GOLD,
		CF_MARK_TIANSHEN_KNIFE0,
		CF_MARK_TIANSHEN_KILL1,
		CF_MARK_TIANSHEN_KILL2,
		CF_MARK_TIANSHEN_KILL3,
		CF_MARK_TIANSHEN_KILL4,
		CF_MARK_TIANSHEN_KILL5,
		CF_MARK_TIANSHEN_KILL6,
		CF_MARK_TIANSHEN_WALLSHOT,
		CF_MARK_TIANSHEN_FIRSTKILL,
		CF_MARK_TIANSHEN_LASTKILL,
		CF_MARK_TIANSHEN_REVENGE,

		CF_MARK_VALORBEAST_EXPLOSIVE_C4,// Crossfire Valorbeast kill marks
		CF_MARK_VALORBEAST_DEFUSE_C4,
		CF_MARK_VALORBEAST_GRENADE,
		CF_MARK_VALORBEAST_HEADSHOT,
		CF_MARK_VALORBEAST_HEADSHOT_GOLD,
		CF_MARK_VALORBEAST_HEADWALLS,
		CF_MARK_VALORBEAST_HEADWALLS_GOLD,
		CF_MARK_VALORBEAST_KNIFE0,
		CF_MARK_VALORBEAST_KILL1,
		CF_MARK_VALORBEAST_KILL2,
		CF_MARK_VALORBEAST_KILL3,
		CF_MARK_VALORBEAST_KILL4,
		CF_MARK_VALORBEAST_KILL5,
		CF_MARK_VALORBEAST_KILL6,
		CF_MARK_VALORBEAST_WALLSHOT,
		CF_MARK_VALORBEAST_FIRSTKILL,
		CF_MARK_VALORBEAST_LASTKILL,
		CF_MARK_VALORBEAST_REVENGE,

		CF_MARK_HEADSHOT_GOLD_VVIP,
		CF_MARK_HEADSHOT_VVIP,
		CF_MARK_HEADSHOT_WALL,
		CF_MARK_KNIFE1,
		CF_MARK_KNIFE2,
		CF_MARK_KNIFE3,
		CF_MARK_KILL1_VVIP,
		CF_MARK_KILL2_VVIP,
		CF_MARK_KILL3_VVIP,
		CF_MARK_KILL4_VVIP,
		CF_MARK_KILL5_VVIP,
		CF_MARK_KILL6_VVIP,
		CF_MARK_BONUS1,
		CF_MARK_BONUS2,
		CF_MARK_BONUS3,
		CF_MARK_CRAZY_HEADKILL,
		CF_MARK_FIRSTBLOOD,
		CF_MARK_KILL2_FX,
		CF_MARK_KILL3_FX,
		CF_MARK_KILL4_FX,
		CF_MARK_KILL5_FX,
		CF_MARK_KILL6_FX,
		CF_MARK_NANO_MULTI1,
		CF_MARK_NANO_MULTI2,
		CF_MARK_NANO_MULTI3,
		CF_MARK_NANO_MULTI4,
		CF_MARK_NANO_RAGE,
		CF_MARK_UPGRADE3


	};

	std::vector< std::string > texturePaths = {
	    "resource/cfmark/badge_c4.tga",
	    "resource/cfmark/badge_c4defuse.tga",
	    "resource/cfmark/badge_grenade.tga",
	    "resource/cfmark/badge_headshot.tga",
	    "resource/cfmark/badge_headshot_gold.tga",
	    "resource/cfmark/badge_headwallshot.tga",
	    "resource/cfmark/badge_headwallshot_gold.tga",
	    "resource/cfmark/badge_knife.tga",
	    "resource/cfmark/badge_multi1.tga",
	    "resource/cfmark/badge_multi2.tga",
	    "resource/cfmark/badge_multi3.tga",
	    "resource/cfmark/badge_multi4.tga",
	    "resource/cfmark/badge_multi5.tga",
	    "resource/cfmark/badge_multi6.tga",
	    "resource/cfmark/badge_wallshot.tga",
	    "resource/cfmark/FIRSTKILL.tga",
	    "resource/cfmark/LASTKILL.tga",
	    "resource/cfmark/Revenge.tga",

	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/15th Anniversary/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/AngelicBeast/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Armored_Beast/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/BlueNeon Killmark/REVENGE.tga",


		"resource/cfmark/CustomKillMark/BornBeast2/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2/REVENGE.tga",

		"resource/cfmark/CustomKillMark/BornBeast2NG/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/BornBeast2NG/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/CFS_2019/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/CFS_2019/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/ChampQ9/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/ChampQ9/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/christmas1/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/christmas1/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/christmas1/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/christmas1/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/christmas1/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Christmas2/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Christmas2/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/DemonicBeast/REVENGE.tga",

		"resource/cfmark/CustomKillMark/Disco/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Disco/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Disco/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Disco/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Disco/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Dominator/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Dominator/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Dominator/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Dominator/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Dominator/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/FuryBeast/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/FuryBeast/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/FuryBeastNG/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/GamingGlory/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/GamingGlory/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/GoldFrame Killmark/REVENGE.tga",

		"resource/cfmark/CustomKillMark/Halloween1/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Halloween1/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Halloween2/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Halloween2/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/HeroKillMark/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/IronBeast2/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/IronBeast2NG/REVENGE.tga",

		"resource/cfmark/CustomKillMark/Kar98kSatellite/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Kar98kSatellite/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/M82A1_AngelicBeast/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/M82A1_DemonicBeast/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/NewYear/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/NewYear/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/NewYear/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/NewYear/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/NewYear/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Nightmare Killmark/REVENGE.tga",


	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach19Killmark2/REVENGE.tga",

		"resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach25Killmark2/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark2/REVENGE.tga",


	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Rankmach26Killmark3/REVENGE.tga",


	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/ReactiveArmorSET/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/RedNanoKillmark/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/Summer Killmark2/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/TianShen/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/TianShen/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/TianShen/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/TianShen/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/TianShen/REVENGE.tga",

	    "resource/cfmark/CustomKillMark/ValorBeast/badge_c4.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_c4defuse.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_grenade.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_headshot.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_headshot_gold.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_headwallshot.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_headwallshot_gold.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_knife.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_multi1.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_multi2.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_multi3.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_multi4.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_multi5.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_multi6.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/badge_wallshot.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/FIRSTKILL.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/LASTKILL.tga",
	    "resource/cfmark/CustomKillMark/ValorBeast/REVENGE.tga",

		"resource/cfmark/badge_headshot_gold_VVIP.tga",
	    "resource/cfmark/badge_headshot_VVIP.tga",
	    "resource/cfmark/badge_headwallshot.tga",
	    "resource/cfmark/badge_knife_1.tga",
	    "resource/cfmark/badge_knife_2.tga",
	    "resource/cfmark/badge_knife_3.tga",
	    "resource/cfmark/badge_multi1_vvip_common.tga",
	    "resource/cfmark/badge_multi2_vvip_common.tga",
	    "resource/cfmark/badge_multi3_vvip_common.tga",
	    "resource/cfmark/badge_multi4_vvip_common.tga",
	    "resource/cfmark/badge_multi5_vvip_common.tga",
	    "resource/cfmark/badge_multi6_vvip_common.tga",
	    "resource/cfmark/Bonuskill01.tga",
	    "resource/cfmark/Bonuskill02.tga",
	    "resource/cfmark/Bonuskill03.tga",
	    "resource/cfmark/CrazyBigHeadKill.tga",
	    "resource/cfmark/FIRSTKILL.tga",
	    "resource/cfmark/multi2_fx.tga",
	    "resource/cfmark/multi3_fx.tga",
	    "resource/cfmark/multi4_fx.tga",
	    "resource/cfmark/multi5_fx.tga",
	    "resource/cfmark/multi6_fx.tga",
	    "resource/cfmark/NANO_multi1.tga",
	    "resource/cfmark/NANO_multi2.tga",
	    "resource/cfmark/NANO_multi3.tga",
	    "resource/cfmark/NANO_multi4.tga",
	    "resource/cfmark/NanoStepUpMark.tga",
	    "resource/cfmark/KillMark_Upgrade3.tga",


	};



};

class CHudAnnouncerIcon : public CHudBase
{


  public:
	bool m_iLastLocalAlive; // Local Player Last Alive
	bool m_pLastPlayer;     // Kill the Last Enemy
	bool m_iWellRound;      // Kill everyone on the enemy team without dying
	bool m_pBully;          // kill the worst player of the game
	bool m_iUntouchable;    // kill the best player of the game
	bool m_iHeadshot;
	bool m_iPayback; // Kill the enemy who killed you previously.	  
	int iIndex;
	std::vector< int > announceTextures; // Texture id
	cvar_t *hud_killfx;


	TextureManager textureManager;

	enum Alarm_Type
	{
		ALARM_KNIFE = 0,
		ALARM_GRENADE,
		ALARM_HEADSHOT,
		ALARM_KILL,
		ALARM_2KILL,
		ALARM_3KILL,
		ALARM_4KILL,
		ALARM_FIRSTBLOOD,
		ALARM_PAYBACK,
		ALARM_EXCELLENT,
		ALARM_INCREDIBLE,
		ALARM_CRAZY,
		ALARM_CANTBELIEVE,
		ALARM_OUTOFWORLD,
		ALARM_LIBERATOR,
		ALARM_ALLROUND,
		ALARM_INFECTOR,
		ALARM_SUPPORTER,
		ALARM_KINGMAKER,
		ALARM_ASSIST,
		ALARM_THELAST,
		ALARM_LASTSOLDIER,
		ALARM_SAVIOR,
		ALARM_INVISHAND,
		ALARM_KINGMURDER,
		ALARM_BACKMARKER,
		ALARM_WELCOME,
		ALARM_COMEBACK,
		ALARM_C4MANKILL,
		ALARM_C4PLANT,
		ALARM_C4PLANTBLOCK,
		ALARM_C4DEFUSTBLOCK,
		ALARM_ZOMBIEBOMB,
		ALARM_ZOMBIETANKER,
		ALARM_INTOXICATION,
		ALARM_REVIVALWILL,
		ALARM_MAXLEVEL,
		ALARM_PROFESSIONAL,
		ALARM_UNTOUCHABLE,
		ALARM_DESTRUCTION,
		ALARM_TERMINATION,
		ALARM_RESCUE,
		ALARM_COMBO,
		ALARM_ALIVE,
		ALARM_NOPARASHUTE,
		ALARM_CHICKENCHASER,
		ALARM_OVERRUN,
		ALARM_ATTACKER,
		ALARM_BESTMOMENT,
		ALARM_HOLDOUT,
		ALARM_BATPULL,
		ALARM_DROPWEAPON,
		ALARM_HIDE,
		ALARM_HEALER,
		ALARM_LAST
	};


	std::vector< std::string > texturePaths = {
	    "resource/announceribbon/announceicon/alarm_knife.tga",         // ALARM_KNIFE
	    "resource/announceribbon/announceicon/alarm_grenade.tga",       // ALARM_GRENADE
	    "resource/announceribbon/announceicon/alarm_headshot.tga",      // ALARM_HEADSHOT
	    "resource/announceribbon/announceicon/alarm_kill.tga",          // ALARM_KILL
	    "resource/announceribbon/announceicon/alarm_2kill.tga",         // ALARM_2KILL
	    "resource/announceribbon/announceicon/alarm_3kill.tga",         // ALARM_3KILL
	    "resource/announceribbon/announceicon/alarm_4kill.tga",         // ALARM_4KILL
	    "resource/announceribbon/announceicon/alarm_firstblood.tga",    // ALARM_FIRSTBLOOD
	    "resource/announceribbon/announceicon/alarm_payback.tga",       // ALARM_PAYBACK
	    "resource/announceribbon/announceicon/alarm_excellent.tga",     // ALARM_EXCELLENT
	    "resource/announceribbon/announceicon/alarm_incredible.tga",    // ALARM_INCREDIBLE
	    "resource/announceribbon/announceicon/alarm_crazy.tga",         // ALARM_CRAZY
	    "resource/announceribbon/announceicon/alarm_cantbelieve.tga",   // ALARM_CANTBELIEVE
	    "resource/announceribbon/announceicon/alarm_outofworld.tga",    // ALARM_OUTOFWORLD
	    "resource/announceribbon/announceicon/alarm_liberator.tga",     // ALARM_LIBERATOR
	    "resource/announceribbon/announceicon/alarm_allround.tga",      // ALARM_ALLROUND
	    "resource/announceribbon/announceicon/alarm_infector.tga",      // ALARM_INFECTOR
	    "resource/announceribbon/announceicon/alarm_supporter.tga",     // ALARM_SUPPORTER
	    "resource/announceribbon/announceicon/alarm_kingmaker.tga",     // ALARM_KINGMAKER
	    "resource/announceribbon/announceicon/alarm_assist.tga",        // ALARM_ASSIST
	    "resource/announceribbon/announceicon/alarm_thelast.tga",       // ALARM_THELAST
	    "resource/announceribbon/announceicon/alarm_lastsoldier.tga",   // ALARM_LASTSOLDIER
	    "resource/announceribbon/announceicon/alarm_savior.tga",        // ALARM_SAVIOR
	    "resource/announceribbon/announceicon/alarm_invishand.tga",     // ALARM_INVISHAND
	    "resource/announceribbon/announceicon/alarm_kingmurder.tga",    // ALARM_KINGMURDER
	    "resource/announceribbon/announceicon/alarm_backmarker.tga",    // ALARM_BACKMARKER
	    "resource/announceribbon/announceicon/alarm_welcome.tga",       // ALARM_WELCOME
	    "resource/announceribbon/announceicon/alarm_comeback.tga",      // ALARM_COMEBACK
	    "resource/announceribbon/announceicon/alarm_c4mankill.tga",     // ALARM_C4MANKILL
	    "resource/announceribbon/announceicon/alarm_c4plant.tga",       // ALARM_C4PLANT
	    "resource/announceribbon/announceicon/alarm_c4plantblock.tga",  // ALARM_C4PLANTBLOCK
	    "resource/announceribbon/announceicon/alarm_c4defustblock.tga", // ALARM_C4DEFUSTBLOCK
	    "resource/announceribbon/announceicon/alarm_zombiebomb.tga",    // ALARM_ZOMBIEBOMB
	    "resource/announceribbon/announceicon/alarm_zombietanker.tga",  // ALARM_ZOMBIETANKER
	    "resource/announceribbon/announceicon/alarm_intoxication.tga",  // ALARM_INTOXICATION
	    "resource/announceribbon/announceicon/alarm_revivalwill.tga",   // ALARM_REVIVALWILL
	    "resource/announceribbon/announceicon/alarm_maxlevel.tga",      // ALARM_MAXLEVEL
	    "resource/announceribbon/announceicon/alarm_professional.tga",  // ALARM_PROFESSIONAL
	    "resource/announceribbon/announceicon/alarm_untouchable.tga",   // ALARM_UNTOUCHABLE
	    "resource/announceribbon/announceicon/alarm_destruction.tga",   // ALARM_DESTRUCTION
	    "resource/announceribbon/announceicon/alarm_termination.tga",   // ALARM_TERMINATION
	    "resource/announceribbon/announceicon/alarm_rescue.tga",        // ALARM_RESCUE
	    "resource/announceribbon/announceicon/alarm_combo.tga",         // ALARM_COMBO
	    "resource/announceribbon/announceicon/alarm_alive.tga",         // ALARM_ALIVE
	    "resource/announceribbon/announceicon/alarm_noparashute.tga",   // ALARM_NOPARASHUTE
	    "resource/announceribbon/announceicon/alarm_chickenchaser.tga", // ALARM_CHICKENCHASER
	    "resource/announceribbon/announceicon/alarm_overrun.tga",       // ALARM_OVERRUN
	    "resource/announceribbon/announceicon/alarm_attacker.tga",      // ALARM_ATTACKER
	    "resource/announceribbon/announceicon/alarm_bestmoment.tga",    // ALARM_BESTMOMENT
	    "resource/announceribbon/announceicon/alarm_holdout.tga",       // ALARM_HOLDOUT
	    "resource/announceribbon/announceicon/alarm_batpull.tga",       // ALARM_BATPULL
	    "resource/announceribbon/announceicon/alarm_dropweapon.tga",    // ALARM_DROPWEAPON
	    "resource/announceribbon/announceicon/alarm_hide.tga",          // ALARM_HIDE
	    "resource/announceribbon/announceicon/alarm_healer.tga"         // ALARM_HEALER
	};



};




//
//-----------------------------------------------------
//
class CHudMenu : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	void Reset( void );
	int Draw( float flTime );

	CHudMsgFunc(ShowMenu);
	CHudMsgFunc(BuyClose);
	CHudMsgFunc(VGUIMenu);
	// server sends false when spectating is not allowed, and true when allowed
	CHudMsgFunc(AllowSpec);

	CHudUserCmd(OldStyleMenuClose);
	CHudUserCmd(OldStyleMenuOpen);
	CHudUserCmd(ShowVGUIMenu);

	void ShowVGUIMenu( int menuType ); // cs16client extension

	void SelectMenuItem( int menu_item );

	int m_fMenuDisplayed;
	bool m_bAllowSpec;
	cvar_t *_extended_menus;
	int m_bitsValidSlots;
	float m_flShutoffTime;
	int m_fWaitingForMore;

};

//
//-----------------------------------------------------
//
class CHudSayText : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	CHudMsgFunc(SayText);
	void SayTextPrint( const char *pszBuf, int iBufSize, int clientIndex = -1 );
	void SayTextPrint( char szBuf[3][256] );
	void EnsureTextFitsInOneLineAndWrapIfHaveTo( int line );
	friend class CHudSpectator;

private:

	struct cvar_s *	m_HUD_saytext;
	struct cvar_s *	m_HUD_saytext_time;
};

//
//-----------------------------------------------------
//
class CHudBattery: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void InitHUDData( void );
	CHudMsgFunc(Battery);
	CHudMsgFunc(ArmorType);
	
private:
	enum armortype_t {
		Vest = 0,
		VestHelm
	} m_enArmorType;

	CClientSprite m_hEmpty[VestHelm + 1];
	CClientSprite m_hFull[VestHelm + 1];
	int	  m_iBat;
	float m_fFade;
	int	  m_iHeight;		// width of the battery innards
};


//
//-----------------------------------------------------
//
class CHudFlashlight: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Reset( void );
	CHudMsgFunc(Flashlight);
	CHudMsgFunc(FlashBat);
	
private:
	CClientSprite m_hSprite1;
	CClientSprite m_hSprite2;
	CClientSprite m_hBeam;
	float m_flBat;
	int	  m_iBat;
	int	  m_fOn;
	float m_fFade;
	int	  m_iWidth;		// width of the battery innards
};

//
//-----------------------------------------------------
//
const int maxHUDMessages = 16;
struct message_parms_t
{
	client_textmessage_t	*pMessage;
	float	time;
	int x, y;
	int	totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

//
//-----------------------------------------------------
//

class CHudTextMessage: public CHudBase
{
public:
	int Init( void );
	static char *LocaliseTextString( const char *msg, char *dst_buffer, int buffer_size );
	static char *BufferedLocaliseTextString( const char *msg );
	static char *LookupString( char *msg_name, int *msg_dest = NULL );
	CHudMsgFunc(TextMsg);
};

//
//-----------------------------------------------------
//

class CHudMessage: public CHudBase
{
public:
	friend class CHudTextMessage;
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	CHudMsgFunc(HudText);
	CHudMsgFunc(GameTitle);
	CHudMsgFunc(HudTextArgs);
	CHudMsgFunc(HudTextPro);

	float FadeBlend( float fadein, float fadeout, float hold, float localTime );
	int	XPosition( float x, int width, int lineWidth );
	int YPosition( float y, int height );

	void MessageAdd( const char *pName, float time );
	void MessageAdd(client_textmessage_t * newMessage );
	void MessageDrawScan( client_textmessage_t *pMessage, float time );
	void MessageScanStart( void );
	void MessageScanNextChar( void );
	void Reset( void );

private:
	client_textmessage_t		*m_pMessages[maxHUDMessages];
	float						m_startTime[maxHUDMessages];
	message_parms_t				m_parms;
	float						m_gameTitleTime;
	client_textmessage_t		*m_pGameTitle;

	int m_HUD_title_life;
	int m_HUD_title_half;
};

//
//-----------------------------------------------------
//
#define MAX_SPRITE_NAME_LENGTH	24

class CHudStatusIcons: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	void Reset( void );
	int Draw(float flTime);
	CHudMsgFunc(StatusIcon);

	enum {
		MAX_ICONSPRITENAME_LENGTH = MAX_SPRITE_NAME_LENGTH,
		MAX_ICONSPRITES = 4,
	};

	
	//had to make these public so CHud could access them (to enable concussion icon)
	//could use a friend declaration instead...
	void EnableIcon( const char *pszIconName, unsigned char red, unsigned char green, unsigned char blue );
	void DisableIcon( const char *pszIconName );

	friend class CHudScoreboard;

private:

	typedef struct
	{
		char szSpriteName[MAX_ICONSPRITENAME_LENGTH];
		_HSPRITE spr;
		wrect_t rc;
		unsigned char r, g, b;
		unsigned char secR, secG, secB;
		float flTimeToChange;
	} icon_sprite_t;

	icon_sprite_t m_IconList[MAX_ICONSPRITES];
};


//
//-----------------------------------------------------
//
#define MONEY_YPOS ScreenHeight - 3 * gHUD.m_iFontHeight

class CHudMoney : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	CHudMsgFunc(Money);
	CHudMsgFunc(BlinkAcct);

private:
	int m_iMoneyCount;
	int m_iDelta;
	int m_iBlinkAmt;
	float m_fBlinkTime;
	float m_fFade;
	CClientSprite m_hDollar;
	CClientSprite m_hPlus;
	CClientSprite m_hMinus;
};
//
//-----------------------------------------------------
//
class CHudRadio: public CHudBase
{
public:
	int Init( void );
	void Voice(int entindex, bool bTalking );
	// play a sentence from a radio
	// [byte] unknown (always 1)
	// [string] sentence name
	// [short] unknown. (always 100, it's a volume?)
	CHudMsgFunc(SendAudio);
	CHudMsgFunc(ReloadSound);
	CHudMsgFunc(BotVoice);
};


//
//-----------------------------------------------------
//


class CHudWinImage : public CHudBase
{

  public:
	int Init( void );
	int VidInit( void );
	void Think( float flTime );
	int Draw( float flTime );
	float m_flEndTime;
	int bWinningTeam;
	cvar_t *hud_winimage;

};

//
//-----------------------------------------------------
//


class CHudSpeedometer : public CHudBase
{
	cvar_t *hud_speedometer;
	cvar_t *hud_speedometer_x;
	cvar_t *hud_speedometer_y;
	cvar_t *hud_speedometer_red;
	cvar_t *hud_speedometer_blue;
	cvar_t *hud_speedometer_green;
	cvar_t *hud_speedometer_texture_type;
	cvar_t *hud_speedometer_texture_width;
	cvar_t *hud_speedometer_texture_height;

  public:
	virtual int Init( );
	virtual int VidInit( );
	void DrawNumber( int number, float x, float y, int r, int g, int b, int a, int textureID,int w, int h );
	void DrawNumber2( int number, float x, float y, int r, int g, int b, int a, int textureID, int desiredWidth, int desiredHeight );

	virtual int Draw( float flTime );
	void UpdateSpeed( const float velocity[2] );
	int current_texture_type;
	int textureID;


  private:
	int m_iSpeed;
};


//
//-----------------------------------------------------
//
class CHudTimer: public CHudBase
{
	friend class CHudSpectatorGui;
public:
	int Init( void );
	int VidInit( void );
	int Draw(float fTime);
	// set up the timer.
	// [short]
	CHudMsgFunc(RoundTime);
	// show the timer
	// [empty]
	CHudMsgFunc(ShowTimer);
	int m_HUD_timer;
	int m_iTime;
	float m_fStartTime;
	bool m_bPanicColorChange;
	float m_flPanicTime;


};
//
//-----------------------------------------------------
//
class CHudProgressBar: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );

	// start progress bar
	// [short] Duration
	CHudMsgFunc(BarTime);

	// [short] Duration
	// [short] percent
	CHudMsgFunc(BarTime2);
	CHudMsgFunc(BotProgress);

private:
	int m_iDuration;
	float m_fPercent;
	float m_fStartTime;
	char m_szHeader[256];
	const char *m_szLocalizedHeader;
};

//
//-----------------------------------------------------
//
// class for drawing sniper scope
class CHudSniperScope: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Shutdown( void );

private:
	float left, right, centerx, centery;
	int m_iScopeArc[4];
};

//
//-----------------------------------------------------
//

class CHudNVG: public CHudBase
{
public:
	int Init( void );
	int Draw( float flTime );
	CHudMsgFunc(NVGToggle);
	CHudUserCmd(NVGAdjustUp);
	CHudUserCmd(NVGAdjustDown);
	cvar_t *cl_crosshair_nvg;
private:
	int m_iAlpha;
	cvar_t *cl_fancy_nvg;
	dlight_t *m_pLight;
};

//
//-----------------------------------------------------
//

class CHudSpectatorGui: public CHudBase
{
public:
	int Init();
	int VidInit();
	int Draw( float flTime );
	void InitHUDData();
	void Reset();
	void Shutdown();

	CHudMsgFunc( SpecHealth );
	CHudMsgFunc( SpecHealth2 );

	CHudUserCmd( ToggleSpectatorMenu );
	CHudUserCmd( ToggleSpectatorMenuOptions );
	CHudUserCmd( ToggleSpectatorMenuOptionsSettings );
	CHudUserCmd( ToggleSpectatorMenuSpectateOptions );

	void CalcAllNeededData( );

	bool m_bBombPlanted;
	int m_iPlayerLastPointedAt;
	static float m_fTextScale;

private:	
	// szMapName is 64 bytes only. Removing "maps/" and ".bsp" gived me this result
	class Labels
	{
	public:
		short m_iTerrorists;
		short m_iCounterTerrorists;
		char m_szTimer[64];
		char m_szMap[64];
		char m_szNameAndHealth[80];
	} label;
	int m_hTimerTexture;

	enum {
		ROOT_MENU = (1<<0),
		MENU_OPTIONS = (1<<1),
		MENU_OPTIONS_SETTINGS = (1<<2),
		MENU_SPEC_OPTIONS = (1<<3)
	};
	byte m_menuFlags;
};

//
//-----------------------------------------------------
//



class CHud
{
public:
	CHud() : m_pHudList(NULL), m_iSpriteCount(0)  {}
	~CHud();			// destructor, frees allocated memory // thanks, Captain Obvious

	void Init( void );
	void VidInit( void );
	void Think( void );
	void Shutdown( void );
	int Redraw( float flTime, int intermission );
	int UpdateClientData( client_data_t *cdata, float time );
	void AddHudElem(CHudBase *p);

	inline float GetSensitivity() { return m_flMouseSensitivity; }
	inline _HSPRITE GetSprite( int index )
	{
		assert( index >= -1 && index <= m_iSpriteCount );

		return (index >= 0) ? m_rghSprites[index] : 0;
	}

	inline wrect_t& GetSpriteRect( int index )
	{
		assert( index >= -1 && index <= m_iSpriteCount );

		return (index >= 0) ? m_rgrcRects[index]: nullrc;
	}

	// GetSpriteIndex()
	// searches through the sprite list loaded from hud.txt for a name matching SpriteName
	// returns an index into the gHUD.m_rghSprites[] array
	// returns -1 if sprite not found
	inline int GetSpriteIndex( const char *SpriteName )
	{
		// look through the loaded sprite name list for SpriteName
		for ( int i = 0; i < m_iSpriteCount; i++ )
		{
			if ( strncmp( SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH ) == 0 )
				return i;
		}

		gEngfuncs.Con_Printf( "GetSpriteIndex: %s sprite not found", SpriteName );
		return -1; // invalid sprite
	}

	inline short GetCharWidth ( unsigned char ch )
	{
		return m_scrinfo.charWidths[ ch ];
	}
	inline int GetCharHeight( )
	{
		return m_scrinfo.iCharHeight;
	}

	inline bool IsCZero( )
	{
		return m_bIsCZero;
	}

	int m_iFontHeight;
	int DrawHudNumber( int x, int y, int iFlags, int iNumber, int r, int g, int b );
	int DrawHudNumber( int x, int y, int iNumber, int r, int g, int b );
	int DrawHudNumberCentered( int x, int y, int iNumber, int r, int g, int b );
	int DrawHudString( int x, int y, int iMaxX, const char *szString, int r, int g, int b );
	int DrawHudStringReverse( int xpos, int ypos, int iMinX, const char *szString, int r, int g, int b );
	int DrawChar( int xpos, int ypos, int iMaxX, const char *szIt, int r, int g, int b );
	int DrawCharReverse( int xpos, int ypos, int iMinX, const char *szString, int r, int g, int b );
	int DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b );
	int GetNumWidth( int iNumber, int iFlags );
	int DrawHudStringLen( const char *szIt );
	int GetHudStringWidth( const char *string );
	void DrawDarkRectangle( int x, int y, int wide, int tall );
	void HUEtoRGB( float hue, int &R, int &G, int &B );
	void DrawHudModelName( int x, int y, float topcolor, float bottomcolor, const char *model );
	int DrawText( int x, int y, const char *szString, int r, int g, int b );
	int DrawTextCentered( int x, int y, const char *szString, int r, int g, int b );
	void DrawStringConsole( int x, int y, int r, int g, int b, const char *fmt, ... );




	float   m_flTime;      // the current client time
	float   m_fOldTime;    // the time at which the HUD was last redrawn
	double  m_flTimeDelta; // the difference between flTime and fOldTime
	float   m_flScale;     // hud_scale->value
	Vector	m_vecOrigin;
	Vector	m_vecAngles;
	int		m_iKeyBits;
	int		m_iHideHUDDisplay;
	int		m_iFOV;
	int		m_Teamplay;
	int		m_iRes;
	cvar_t *m_pCvarDraw;
	cvar_t *cl_shadows;
	cvar_t *cl_predict;
	cvar_t *cl_weapon_wallpuff;
	cvar_t *cl_weapon_sparks;
	cvar_t *zoom_sens_ratio;
	cvar_t *cl_lw;
	cvar_t *cl_righthand;
	cvar_t *cl_weather;
	cvar_t *cl_minmodels;
	cvar_t *cl_min_t;
	cvar_t *cl_min_ct;
	cvar_t *cl_gunsmoke;
	cvar_t *default_fov;
	cvar_t *hud_textmode;
	cvar_t *hud_colored;
	cvar_t *sv_skipshield;
#ifdef __ANDROID__
	cvar_t *cl_android_force_defaults;
#endif

	_HSPRITE m_hGasPuff;

	CHudAmmo        m_Ammo;
	CHudHealth      m_Health;
	CHudSpectator   m_Spectator;
	CHudGeiger      m_Geiger;
	CHudBattery	    m_Battery;
	CHudTrain       m_Train;
	CHudFlashlight  m_Flash;
	CHudMessage     m_Message;
	CHudStatusBar   m_StatusBar;
	CHudDeathNotice m_DeathNotice;
	CHudSayText     m_SayText;
	CHudMenu        m_Menu;
	CHudAmmoSecondary m_AmmoSecondary;
	CHudTextMessage m_TextMessage;
	CHudStatusIcons m_StatusIcons;
	CHudScoreboard  m_Scoreboard;
	CHudMOTD        m_MOTD;
	CHudMoney       m_Money;
	CHudTimer       m_Timer;
	CHudRadio       m_Radio;
	CHudProgressBar m_ProgressBar;
	CHudSniperScope m_SniperScope;
	CHudNVG         m_NVG;
	CHudRadar       m_Radar;
	CHudSpectatorGui m_SpectatorGui;
	CHudWinImage m_WinImage;
	CHudSpeedometer m_SpeedoMeter;
	SoundManager m_SoundManager;
	CHudAnnouncerIcon m_AnnouncerIcon;
	CHudCFMarks m_CHudCFMarks;
	TextureManager m_TextureManager;
	// user messages
	CHudMsgFunc(Damage);
	CHudMsgFunc(GameMode);
	CHudMsgFunc(Logo);
	CHudMsgFunc(ResetHUD);
	CHudMsgFunc(InitHUD);
	CHudMsgFunc(ViewMode);
	CHudMsgFunc(SetFOV);
	CHudMsgFunc(Concuss);
	CHudMsgFunc(ShadowIdx);

	// Screen information
	SCREENINFO	m_scrinfo;
	// As Xash3D can fake m_scrinfo for hud scailing
	// we will use a real screen parameters
	SCREENINFO  m_truescrinfo;

	int	m_iWeaponBits;
	int	m_fPlayerDead;
	int m_iIntermission;
	int m_iNoConsolePrint;

	// sprite indexes
	int m_HUD_number_0;

	char m_szServerName[64];

private:
	HUDLIST	*m_pHudList;
	_HSPRITE	m_hsprLogo;
	int	m_iLogo;
	client_sprite_t	*m_pSpriteList;
	int	m_iSpriteCount;
	int	m_iSpriteCountAllRes;
	float m_flMouseSensitivity;
	int	m_iConcussionEffect;
	int	m_iForceCamera;
	int m_iForceChaseCam;
	int m_iFadeToBlack;
	bool m_bIsCZero;

	// the memory for these arrays are allocated in the first call to CHud::VidInit(), when the hud.txt and associated sprites are loaded.
	// freed in ~CHud()
	_HSPRITE *m_rghSprites;	/*[HUD_SPRITE_COUNT]*/			// the sprites loaded from hud.txt
	wrect_t *m_rgrcRects;	/*[HUD_SPRITE_COUNT]*/
	char *m_rgszSpriteNames; /*[HUD_SPRITE_COUNT][MAX_SPRITE_NAME_LENGTH]*/
};

extern CHud gHUD;
extern cvar_t *sensitivity;

extern int g_iTeamNumber;
extern int g_iUser1;
extern int g_iUser2;
extern int g_iUser3;
