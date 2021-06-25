#pragma once

#include "..\sdk\interfaces\IVEngineClient.hpp"
#include "..\sdk\interfaces\IInputSystem.hpp"
#include "..\sdk\interfaces\IBaseClientDll.hpp"
#include "..\sdk\interfaces\IClientEntityList.hpp"
#include "..\sdk\interfaces\IClientMode.hpp"
#include "..\sdk\interfaces\ICvar.hpp"
#include "..\sdk\interfaces\IEngineTrace.hpp"
#include "..\sdk\interfaces\IEngineSound.hpp"
#include "..\sdk\interfaces\IRenderView.hpp"
#include "..\sdk\interfaces\IVModelRender.hpp"
#include "..\sdk\interfaces\IMaterialSystem.hpp"
#include "..\sdk\interfaces\IPanel.hpp"
#include "..\sdk\interfaces\IVModelInfoClient.hpp"
#include "..\sdk\interfaces\IMDLCache.hpp"
#include "..\sdk\interfaces\memalloc.h"
#include "..\sdk\interfaces\CClientState.hpp"
#include "..\sdk\interfaces\IPrediction.hpp"
#include "..\sdk\interfaces\IMoveHelper.hpp"
#include "..\sdk\interfaces\CInput.hpp"
#include "..\sdk\interfaces\ISurface.hpp"
#include "..\sdk\interfaces\IVDebugOverlay.hpp"
#include "..\sdk\interfaces\IViewRenderBeams.hpp"
#include "..\sdk\interfaces\IPhysics.hpp"
#include "..\sdk\interfaces\ILocalize.hpp"
#include "..\sdk\interfaces\ISoundServices.hpp"
#include "..\sdk\misc\GlobalVars.hpp"
#include "..\sdk\misc\glow_outline_effect.hpp"
#include "..\sdk\interfaces\IGameEventManager.hpp"
#include "..\sdk\misc\C_CSPlayerResource.h"

#include <d3dx9.h>
#include <d3d9.h>

class InterfaceReg
{
private:
	using InstantiateInterfaceFn = void*(*)();
public:
	InstantiateInterfaceFn m_CreateFn;
	const char *m_pName;
	InterfaceReg *m_pNext;
};

class C_CSGO
{
public:
	IDirect3DDevice9 * m_device();
	IVEngineClient * m_engine();
	IInputSystem * m_inputsys();
	IBaseClientDLL * m_client();
	IClientEntityList * m_entitylist();
	ICvar * m_cvar();
	IEngineTrace * m_trace();
	IEngineSound * m_enginesound();
	DWORD * m_inputinternal();
	IVRenderView * m_renderview();
	IMDLCache * m_modelcache();
	IVModelRender * m_modelrender();
	IMaterialSystem * m_materialsystem();
	IPanel * m_panel();
	IVModelInfoClient * m_modelinfo();
	IPrediction * m_prediction();
	IGameMovement * m_gamemovement();
	ISurface * m_surface();
	IVDebugOverlay * m_debugoverlay();
	IPhysicsSurfaceProps * m_physsurface();
	IGameEventManager2 * m_eventmanager();
	IViewRenderBeams * m_viewrenderbeams();
	IMemAlloc * m_memalloc();
	IClientMode * m_clientmode();
	CGlobalVarsBase * m_globals();
	CGlowObjectManager * m_glow();
	CClientState * m_clientstate();
	IMoveHelper * m_movehelper();
	CInput * m_input();
	C_CSPlayerResource * m_playerresource();
	CSGameRulesProxy * m_gamerules();
	ILocalize * m_localize();
	IBaseFileSystem* m_basefilesys();

	DWORD m_postprocessing();
	DWORD m_ccsplayerrenderablevftable();
private:
	IDirect3DDevice9 * p_device = nullptr;
	IVEngineClient * p_engine = nullptr;
	IInputSystem * p_inputsys = nullptr;
	IBaseClientDLL * p_client = nullptr;
	IClientEntityList * p_entitylist = nullptr;
	IEngineTrace * p_trace = nullptr;
	IEngineSound * p_enginesound = nullptr;
	ICvar * p_cvar = nullptr;
	DWORD * p_inputinternal = nullptr;
	IVRenderView * p_renderview = nullptr;
	IMDLCache* p_modelcache = nullptr;
	IVModelRender * p_modelrender = nullptr;
	IMaterialSystem * p_materialsystem = nullptr;
	IPanel * p_panel = nullptr;
	IVModelInfoClient * p_modelinfo = nullptr;
	IPrediction * p_prediciton = nullptr;
	IGameMovement * p_gamemovement = nullptr;
	ISurface * p_surface = nullptr;
	IVDebugOverlay * p_debugoverlay = nullptr;
	IPhysicsSurfaceProps * p_physsurface = nullptr;
	IGameEventManager2 * p_eventmanager = nullptr;
	IViewRenderBeams * p_viewrenderbeams = nullptr;
	IMemAlloc * p_memalloc = nullptr;
	IClientMode * p_clientmode = nullptr;
	CGlobalVarsBase * p_globals = nullptr;
	CGlowObjectManager * p_glow = nullptr;
	CClientState * p_clientstate = nullptr;
	IMoveHelper * p_movehelper = nullptr;
	CInput * p_input = nullptr;
	C_CSPlayerResource * p_playerresource = nullptr;
	CSGameRulesProxy * p_gamerules = nullptr;
	ILocalize * p_localize = nullptr;
	IBaseFileSystem* p_basefilesys = nullptr;

	DWORD p_postprocessing = 0;
	DWORD p_ccsplayerrenderablevftable = 0;
};

extern C_CSGO g_csgo;