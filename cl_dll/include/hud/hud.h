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
	int DrawTeams( float listslot );
	int DrawPlayers( float listslot, int nameoffset = 0, const char *team = NULL ); // returns the ypos where it finishes drawing

	void DeathMsg( int killer, int victim );
	int MsgFunc_HealthInfo( const char *pszName, int iSize, void *pbuf );
	int MsgFunc_Account( const char *pszName, int iSize, void *pbuf );
	void SetScoreboardDefaults( void );
	void GetAllPlayersInfo( void );

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
	int m_killNums, m_multiKills;
	cvar_t *hud_deathnotice_time;
	cvar_t *hud_killicon_display_time;
	cvar_t *hud_killeffect_display_time;


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
		char command[MAX_SOUND_LENGTH + 10];
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
	};

	std::vector< TextureInfo > textureList;
	float lastDrawTime;
	const float ALARM_DURATION = 0.5f; // Base duration for each alarm
	const float ALARM_FADE_DURATION    = 0.1f; // Fade in/out duration

  public:
	TextureManager( )
	    : lastDrawTime( 0 ) {}

	void AddTexture( int textureID, float currentTime )
	{
		float duration = ALARM_DURATION;
		if ( !textureList.empty( ) )
		{
			duration += textureList.back( ).duration; 
		}
		textureList.push_back( { textureID, duration, currentTime, 0.0f } );
	}

	void Update( float currentTime )	
	{
		
		if ( !textureList.empty( ) )
		{
			TextureInfo &front = textureList.front( );
			float elapsedTime  = currentTime - front.startTime;

			if ( elapsedTime <= ALARM_FADE_DURATION )
			{
				front.alpha = elapsedTime / ALARM_FADE_DURATION; // Fade in
			}
			else if ( elapsedTime > front.duration - ALARM_FADE_DURATION )
			{
				front.alpha = ( front.duration - elapsedTime ) / ALARM_FADE_DURATION; // Fade out
			}
			else
			{
				front.alpha = 1.0f;
			}

			if ( elapsedTime > front.duration )
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

	TextureInfo GetCurrentTexture( ) const
	{
		return textureList.front( );
	}
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
private:
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
