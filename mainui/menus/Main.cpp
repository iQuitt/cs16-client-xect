/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/


#include "Framework.h"
#include "Action.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "YesNoMessageBox.h"
#include "keydefs.h"
#include "MenuStrings.h"
#include "PlayerIntroduceDialog.h"

#if !(defined(__ANDROID__) || defined(__SAILFISH__))
#define ART_MINIMIZE_N	"gfx/shell/min_n"
#define ART_MINIMIZE_F	"gfx/shell/min_f"
#define ART_MINIMIZE_D	"gfx/shell/min_d"
#endif

#define ART_CLOSEBTN_N	"gfx/shell/cls_n"
#define ART_CLOSEBTN_F	"gfx/shell/cls_f"
#define ART_CLOSEBTN_D	"gfx/shell/cls_d"

class CMenuMain: public CMenuFramework
{
public:
	CMenuMain() : CMenuFramework( "CMenuMain" ) { }

	bool KeyDown( int key ) override;

private:
	void _Init() override;
	void _VidInit( ) override;

	void QuitDialog( void *pExtra = NULL );
	void DisconnectDialogCb();
	void JoinDiscordCb();
	void JoinGithubCb();

	CMenuPicButton	console;
	class CMenuMainBanner : public CMenuBannerBitmap
	{
	public:
		virtual void Draw();
	} banner;

	CMenuPicButton	resumeGame;
	CMenuPicButton	disconnect;
	CMenuPicButton	configuration;
	CMenuPicButton	multiPlayer;
	CMenuPicButton	customGame;
	CMenuPicButton	quit;
	CMenuPicButton  discord;
	CMenuPicButton  github;

	// quit dialog
	CMenuYesNoMessageBox dialog;

	bool bCustomGame;
};

static CMenuMain uiMain;

void CMenuMain::CMenuMainBanner::Draw()
{
	if( !uiMain.background.ShouldDrawLogoMovie() )
		return; // no logos for steam background

	if( EngFuncs::GetLogoLength() <= 0.05f || EngFuncs::GetLogoWidth() <= 32 )
		return;	// don't draw stub logo (GoldSrc rules)

	float	logoWidth, logoHeight, logoPosY;
	float	scaleX, scaleY;

	scaleX = ScreenWidth / 640.0f;
	scaleY = ScreenHeight / 480.0f;

	// a1ba: multiply by height scale to look better on widescreens
	logoWidth = EngFuncs::GetLogoWidth() * scaleX;
	logoHeight = EngFuncs::GetLogoHeight() * scaleY * uiStatic.scaleY;
	logoPosY = 70 * scaleY * uiStatic.scaleY;	// 70 it's empirically determined value (magic number)

	EngFuncs::DrawLogo( "logo.avi", 0, logoPosY, logoWidth, logoHeight );
}

void CMenuMain::QuitDialog(void *pExtra)
{
	if( CL_IsActive() && EngFuncs::GetCvarFloat( "host_serverstate" ) && EngFuncs::GetCvarFloat( "maxplayers" ) == 1.0f )
		dialog.SetMessage( L( "StringsList_235" ) );
	else
		dialog.SetMessage( L( "StringsList_236" ) );

	dialog.onPositive.SetCommand( FALSE, "quit\n" );
	dialog.Show();
}

void CMenuMain::DisconnectDialogCb()
{
	dialog.onPositive.SetCommand( FALSE, "cmd disconnect;endgame disconnect;wait;wait;wait;menu_options;menu_main\n" );
	dialog.SetMessage( L( "Really disconnect?" ) );
	dialog.Show();
}
#undef ShellExecute// sakamisiniz amk
void CMenuMain::JoinDiscordCb()
{
	EngFuncs::ShellExecute("https://discord.gg/FdHtuNCWX9", NULL, FALSE);
}

void CMenuMain::JoinGithubCb()
{
	EngFuncs::ShellExecute("https://github.com/iQuitt/cs16-client-xect", NULL, FALSE);
}

/*
=================
CMenuMain::Key
=================
*/
bool CMenuMain::KeyDown( int key )
{
	if( UI::Key::IsEscape( key ) )
	{
		if ( CL_IsActive( ))
		{
			if( !dialog.IsVisible() )
				UI_CloseMenu();
		}
		else
		{
			QuitDialog( );
		}
		return true;
	}
	return CMenuFramework::KeyDown( key );
}

void CMenuMain::_Init( void )
{
	if( EngFuncs::GetCvarFloat( "host_allow_changegame" ))
		bCustomGame = true;
	else bCustomGame = false;

	// console
	console.SetNameAndStatus( L( "GameUI_Console" ), L( "Show console" ) );
	console.iFlags |= QMF_NOTIFY;
	console.SetPicture( PC_CONSOLE );
	SET_EVENT_MULTI( console.onReleased,
	{
		UI_SetActiveMenu( FALSE );
		EngFuncs::KEY_SetDest( KEY_CONSOLE );
	});

	resumeGame.SetNameAndStatus( L( "GameUI_GameMenu_ResumeGame" ), L( "StringsList_188" ) );
	resumeGame.SetPicture( PC_RESUME_GAME );
	resumeGame.iFlags |= QMF_NOTIFY;
	resumeGame.onReleased = UI_CloseMenu;

	disconnect.SetNameAndStatus( L( "GameUI_GameMenu_Disconnect" ), L( "Disconnect from server" ) );
	disconnect.SetPicture( PC_DISCONNECT );
	disconnect.iFlags |= QMF_NOTIFY;
	disconnect.onReleased = VoidCb( &CMenuMain::DisconnectDialogCb );

	multiPlayer.SetNameAndStatus( L( "GameUI_Multiplayer" ), L( "StringsList_198" ) );
	multiPlayer.SetPicture( PC_MULTIPLAYER );
	multiPlayer.iFlags |= QMF_NOTIFY;
	multiPlayer.onReleased = UI_MultiPlayer_Menu;

	configuration.SetNameAndStatus( L( "GameUI_Options" ), L( "StringsList_193" ) );
	configuration.SetPicture( PC_CONFIG );
	configuration.iFlags |= QMF_NOTIFY;
	configuration.onReleased = UI_Options_Menu;

	customGame.SetNameAndStatus( L( "GameUI_ChangeGame" ), L( "StringsList_530" ) );
	customGame.SetPicture( PC_CUSTOM_GAME );
	customGame.iFlags |= QMF_NOTIFY;
	customGame.onReleased = UI_CustomGame_Menu;

	quit.SetNameAndStatus( L( "GameUI_GameMenu_Quit" ), L( "StringsList_236" ) );
	quit.SetPicture( PC_QUIT );
	quit.iFlags |= QMF_NOTIFY;
	quit.onReleased = MenuCb( &CMenuMain::QuitDialog );

	discord.SetNameAndStatus( "Join Discord", "Join XecT Discord Server" );
	discord.SetPicture( NULL );
	discord.iFlags |= QMF_NOTIFY;
	discord.onReleased = VoidCb( &CMenuMain::JoinDiscordCb );

	github.SetNameAndStatus( "Github", "XecT Client Repository" );
	github.SetPicture( NULL );
	github.iFlags |= QMF_NOTIFY;
	github.onReleased = VoidCb( &CMenuMain::JoinGithubCb );

	dialog.Link( this );

	AddItem( background );
	AddItem( banner );

	if ( EngFuncs::GetCvarFloat( "developer" ))
		AddItem( console );

	AddItem( github );
	AddItem( discord );
	AddItem( disconnect );
	AddItem( resumeGame );

	AddItem( configuration );
	AddItem( multiPlayer );

	if ( bCustomGame )
		AddItem( customGame );

	AddItem( quit );
}

/*
=================
UI_Main_Init
=================
*/
void CMenuMain::_VidInit( void )
{
	if ( CL_IsActive( ))
	{
		resumeGame.Show();
		disconnect.Show();
	}
	else
	{
		resumeGame.Hide();
		disconnect.Hide();
	}

	if( EngFuncs::GetCvarFloat( "developer" ) )
	{
		if ( CL_IsActive( ) && bCustomGame )
		{
			console.pos.y = 300;
		}
		else if ( ( CL_IsActive( ) && !bCustomGame ) )
		{
			console.pos.y = 350;
		}
		else if ((!CL_IsActive() && bCustomGame))
		{
			console.pos.y = 400;
		}
		else if (!CL_IsActive() && !bCustomGame)
		{
			console.pos.y = 450;
		}
	}

	CMenuPicButton::ClearButtonStack();

	console.pos.x = 24;
	github.SetCoord( 24, 200 );
	discord.SetCoord( 24, 250 );
	resumeGame.SetCoord( 24, bCustomGame ? 400 : 450 );
	disconnect.SetCoord( 24, bCustomGame ? 450 : 500 );

	configuration.SetCoord( 24, bCustomGame ? 500 : 550 );
	multiPlayer.SetCoord( 24, bCustomGame ? 550 : 600 );

	customGame.SetCoord( 24, 600 );
	quit.SetCoord( 24, 650 );
}

/*
=================
UI_Main_Precache
=================
*/
void UI_Main_Precache( void )
{
#if !(defined(__ANDROID__) || defined(__SAILFISH__))
	EngFuncs::PIC_Load( ART_MINIMIZE_N );
	EngFuncs::PIC_Load( ART_MINIMIZE_F );
	EngFuncs::PIC_Load( ART_MINIMIZE_D );
#endif

	EngFuncs::PIC_Load( ART_CLOSEBTN_N );
	EngFuncs::PIC_Load( ART_CLOSEBTN_F );
	EngFuncs::PIC_Load( ART_CLOSEBTN_D );

	// precache .avi file and get logo width and height
	EngFuncs::PrecacheLogo( "logo.avi" );
}

/*
=================
UI_Main_Menu
=================
*/
void UI_Main_Menu( void )
{
	uiMain.Show();
}
ADD_MENU( menu_main, UI_Main_Precache, UI_Main_Menu );
