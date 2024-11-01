#include "hud.h"
#include "cl_util.h"
#include "legview.h"
#include "studio_util.h"
#include "r_studioint.h"
#include "com_model.h"
#include <string.h>
#include <GameStudioModelRenderer.h>
CClientLegs g_ClientLegs;
CGameStudioModelRenderer g_StudioRenderer;

CClientLegs::CClientLegs( )
{
	m_pLegsEntity     = nullptr;
	m_bInitialized    = false;
	m_iSequence       = 0;
	m_flSequenceSpeed = 0.0f;
}

CClientLegs::~CClientLegs( )
{
	Reset( );
}

void CClientLegs::Init( )
{
	if ( m_bInitialized )
		return;

	CreateLegsModel( );
	m_bInitialized = true;
}

void CClientLegs::CreateLegsModel( )
{
	cl_entity_t *localPlayer = gEngfuncs.GetLocalPlayer( );
	if ( !localPlayer )
		return;

	// Create legs entity
	m_pLegsEntity = gEngfuncs.GetEntityByIndex( 0 ); // Use free index
	if ( !m_pLegsEntity )
		return;

	// Copy model from local player
	memcpy( m_pLegsEntity, localPlayer, sizeof( cl_entity_t ) );

	m_pLegsEntity->curstate.rendermode = kRenderNormal;
	m_pLegsEntity->curstate.renderamt  = 255;
	m_pLegsEntity->curstate.sequence   = 3; // Standing sequence
	m_pLegsEntity->curstate.framerate  = 1.0;
}

void CClientLegs::UpdateLegs( float time, ref_params_t *pparams )
{
	if ( !m_pLegsEntity || !pparams )
		return;

	cl_entity_t *localPlayer = gEngfuncs.GetLocalPlayer( );
	if ( !localPlayer || !localPlayer->model )
		return;

	// Calculate new position
	Vector forward;
	AngleVectors( pparams->viewangles, forward, nullptr, nullptr );
	forward.z = 0;
	VectorNormalize( forward );

	Vector newOrigin      = pparams->simorg - ( forward * 18.0f );
	m_pLegsEntity->origin = newOrigin;

	// Set angles
	m_pLegsEntity->angles[1] = pparams->viewangles[1];

	// Update animation speed based on velocity
	float speed = sqrt( pow( pparams->simvel[0], 2 ) + pow( pparams->simvel[1], 2 ) );
	if ( speed < 1.0f )
	{
		m_pLegsEntity->curstate.framerate = 0.0f;
	}
	else
	{
		m_pLegsEntity->curstate.framerate = speed / 125.0f;
	}
}

void CClientLegs::Render( float time, ref_params_t *pparams )
{
	if ( !m_bInitialized )
		Init( );

	if ( !pparams || !m_pLegsEntity )
		return;

	// Don't render legs in third person or when dead
	if ( pparams->viewentity != pparams->playernum + 1 || !gEngfuncs.GetLocalPlayer( )->index  )
		return;

	UpdateLegs( time, pparams );

	// Save original vieworg and angles
	Vector oldOrg    = pparams->vieworg;
	Vector oldAngles = pparams->viewangles;

	// Render legs
	model_t *model = m_pLegsEntity->model;
	if ( model && model->type == mod_studio )
	{
		entity_state_t *state = &m_pLegsEntity->curstate;

		//// Studio model rendering
		//gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
		//IEngineStudio.StudioSetupLighting( &state->rendermode );
		//IEngineStudio.StudioDrawModel( 0, m_pLegsEntity );
	}

	// Restore original view
	pparams->vieworg    = oldOrg;
	pparams->viewangles = oldAngles;
}

void CClientLegs::Reset( )
{
	m_pLegsEntity  = nullptr;
	m_bInitialized = false;
}
