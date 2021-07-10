#pragma once

#define NDEBUG
#define DIRECTINPUT_VERSION 0x0800

#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <deque>
#include <array>
#include <ctime>

#include <d3d9.h>
#include <d3dx9.h>

#include <dinput.h>
#include <tchar.h>

#include "cheats\misc\key_binds.h"
#include "utils\util.hpp"
#include "utils\csgo.hpp"
#include "utils\draw_manager.h"
#include "utils\ctx.hpp"
#include "utils\netmanager.hpp"
#include "utils\math.hpp"
#include "utils\singleton.h"
#include "utils\md5.h"
#include "utils\crypt_str.h"

#include "lua\Clua.h"
#include "detours.h"
#include "hooks\hooks.hpp"
#include "configs\configs.h"

#include "resources\custom_sounds.hpp"
#include "resources\sounds.hpp"

#include "sdk\math\Vector.hpp"
#include "sdk\math\VMatrix.hpp"

#include "sdk\misc\UtlVector.hpp"
#include "sdk\misc\EHandle.hpp"
#include "sdk\misc\CUserCmd.hpp"
#include "sdk\misc\Color.hpp"
#include "sdk\misc\KeyValues.hpp"
#include "sdk\misc\datamap.h"

#include "sdk\interfaces\IClientEntity.hpp"
#include "sdk\structs.hpp"

#define m_client g_csgo.m_client
#define m_clientmode g_csgo.m_clientmode
#define m_clientstate g_csgo.m_clientstate
#define m_cvar g_csgo.m_cvar
#define m_debugoverlay g_csgo.m_debugoverlay
#define m_device g_csgo.m_device
#define m_engine g_csgo.m_engine
#define m_enginesound g_csgo.m_enginesound
#define m_entitylist g_csgo.m_entitylist
#define m_eventmanager g_csgo.m_eventmanager
#define m_gamemovement g_csgo.m_gamemovement
#define m_gamerules g_csgo.m_gamerules
#define m_globals g_csgo.m_globals
#define m_glow g_csgo.m_glow
#define m_input g_csgo.m_input
#define m_inputinternal g_csgo.m_inputinternal
#define m_inputsys g_csgo.m_inputsys
#define m_localize g_csgo.m_localize
#define m_materialsystem g_csgo.m_materialsystem
#define m_memalloc g_csgo.m_memalloc
#define m_modelcache g_csgo.m_modelcache
#define m_modelinfo g_csgo.m_modelinfo
#define m_modelrender g_csgo.m_modelrender
#define m_movehelper g_csgo.m_movehelper
#define m_panel g_csgo.m_panel
#define m_physsurface g_csgo.m_physsurface
#define m_playerresource g_csgo.m_playerresource
#define m_postprocessing g_csgo.m_postprocessing
#define m_prediction g_csgo.m_prediction
#define m_renderview g_csgo.m_renderview
#define m_surface g_csgo.m_surface
#define m_trace g_csgo.m_trace
#define m_viewrenderbeams g_csgo.m_viewrenderbeams
#define m_soundservices g_csgo.m_soundservices
#define m_basefilesys g_csgo.m_basefilesys
#define m_Enginetool g_csgo.m_enginetool
#define m_weaponsystem g_csgo.m_WeaponSystem

#include "steam_sdk/steam_api.h"