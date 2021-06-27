#pragma once

#include "IAppSystem.hpp"
#include "..\misc\vfunc.hpp"

class ILocalizeTextQuery
{
public:
	virtual int ComputeTextWidth(const wchar_t* pString) = 0;
};

class ILocalizationChangeCallback
{
public:
	virtual void OnLocalizationChanged() = 0;
};

class ILocalize : public IAppSystem
{
public:
	virtual bool					AddFile(const char* fileName, const char* pPathID = nullptr, bool bIncludeFallbackSearchPaths = false) = 0;
	virtual void					RemoveAll() = 0;
	virtual wchar_t*				Find(const char* tokenName) = 0;
	virtual const wchar_t*			FindSafe(const char* tokenName) = 0;
	virtual int						ConvertANSIToUnicode(const char* ansi, wchar_t* unicode, int unicodeBufferSizeInBytes) = 0;
	virtual int						ConvertUnicodeToANSI(const wchar_t* unicode, char* ansi, int ansiBufferSize) = 0;
	virtual unsigned	FindIndex(const char* tokenName) = 0;
	virtual void					ConstructString(wchar_t* unicodeOuput, int unicodeBufferSizeInBytes, const wchar_t* formatString, int numFormatParameters, ...) = 0;
	virtual const char*				GetNameByIndex(unsigned index) = 0;
	virtual wchar_t*				GetValueByIndex(unsigned index) = 0;
};

struct IBaseFileSystem
{

	int Read(void* pOutput, int size, void* file)
	{
		using Fn = int(__thiscall*)(void*, void*, int, void*);
		return call_virtual<Fn>(this, 0)(this, pOutput, size, file);
	}

	// if pathID is NULL, all paths will be searched for the file
	void* Open(const char* pFileName, const char* pOptions, const char* pathID = nullptr)
	{
		using Fn = void* (__thiscall*)(void*, const char*, const char*, const char*);
		return call_virtual<Fn>(this, 2)(this, pFileName, pOptions, pathID);
	}

	void Close(void* file)
	{
		using Fn = void(__thiscall*)(void*, void*);
		return call_virtual<Fn>(this, 3)(this, file);
	}

	unsigned int Size(void* file)
	{
		using Fn = unsigned int(__thiscall*)(void*, void*);
		return call_virtual<Fn>(this, 7)(this, file);
	}
};

class IBaseInterface
{
public:
	virtual	~IBaseInterface() {}
};
struct soundlevel_t;
class IEngineToolFramework : public IBaseInterface
{
public:
	// Input system overrides TBD
	// Something like this
	//virtual void		AddMessageHandler( int wm_message, bool (*pfnCallback)( int wm_message, int wParam, int lParam ) ) = 0;
	//virtual void		RemoveMessageHanlder( int wm_message, bool (*pfnCallbackToRemove)( int wm_message, int wParam, int lParam ) ) = 0;

	// Helpers for implementing a tool switching UI
	virtual int			GetToolCount() const = 0;
	virtual char const* GetToolName(int index) const = 0;
	virtual void		SwitchToTool(int index) = 0;

	virtual bool		IsTopmostTool(void* sys) const = 0;

	virtual void* GetToolSystem(int index) const = 0;
	virtual void* GetTopmostTool() = 0;

	// Take over input
	virtual void		ShowCursor(bool show) = 0;
	virtual bool		IsCursorVisible() const = 0;

	// If module not already loaded, loads it and optionally switches to first tool in module.  Returns false if load failed or tool already loaded
	virtual bool		LoadToolModule(char const* pToolModule, bool bSwitchToFirst) = 0;
};

class IEngineTool : public IEngineToolFramework
{
public:
	virtual void		GetServerFactory(CreateInterfaceFn& factory) = 0;
	virtual void		GetClientFactory(CreateInterfaceFn& factory) = 0;
	virtual float		GetSoundDuration(const char* pszName) = 0;
	virtual bool		IsSoundStillPlaying(int guid) = 0;
	// Returns the guid of the sound
	virtual int			StartSound(
		int iUserData,
		bool staticsound,
		int iEntIndex,
		int iChannel,
		const char* pSample,
		float flVolume,
		soundlevel_t iSoundlevel,
		const Vector& origin,
		const Vector& direction,
		int iFlags = 0,
		int iPitch = 0,
		bool bUpdatePositions = true,
		float delay = 0.0f,
		int speakerentity = -1) = 0;

	virtual void		StopSoundByGuid(int guid) = 0;

	virtual void		SetVolumeByGuid(int guid, float flVolume) = 0;

	// Returns how long the sound is
	virtual float		GetSoundDuration(int guid) = 0;

	// Returns if the sound is looping
	virtual bool		IsLoopingSound(int guid) = 0;
	virtual void		ReloadSound(const char* pSample) = 0;
	virtual void		StopAllSounds() = 0;
	virtual float		GetMono16Samples(const char* pszName, CUtlVector< short >& sampleList) = 0;
	virtual void		SetAudioState(const AudioState_t& audioState) = 0;

	// Issue a console command
	virtual void		Command(char const* cmd) = 0;
	// Flush console command buffer right away
	virtual void		Execute() = 0;

	virtual char const* GetCurrentMap() = 0;
	virtual void		ChangeToMap(char const* mapname) = 0;
	virtual bool		IsMapValid(char const* mapname) = 0;

	// Method for causing engine to call client to render scene with no view model or overlays
	// See cdll_int.h for enum RenderViewInfo_t for specifying whatToRender
	virtual void		RenderView(CViewSetup& view, int nFlags, int whatToRender) = 0;

	// Returns true if the player is fully connected and active in game (i.e, not still loading)
	virtual bool		IsInGame() = 0;
	// Returns true if the player is connected, but not necessarily active in game (could still be loading)
	virtual bool		IsConnected() = 0;

	virtual int			GetMaxClients() = 0; // Tools might want to ensure single player, e.g.

	virtual bool		IsGamePaused() = 0;
	virtual void		SetGamePaused(bool paused) = 0;

	virtual float		GetTimescale() = 0; // Could do this via ConVar system, too
	virtual void		SetTimescale(float scale) = 0;

	// Real time is unscaled, but is updated once per frame
	virtual float		GetRealTime() = 0;
	virtual float		GetRealFrameTime() = 0; // unscaled

	// Get high precision timer (for profiling?)
	virtual float		Time() = 0;

	// Host time is scaled
	virtual float		HostFrameTime() = 0; // host_frametime
	virtual float		HostTime() = 0; // host_time
	virtual int			HostTick() = 0; // host_tickcount
	virtual int			HostFrameCount() = 0; // total famecount

	virtual float		ServerTime() = 0; // gpGlobals->curtime on server
	virtual float		ServerFrameTime() = 0; // gpGlobals->frametime on server
	virtual int			ServerTick() = 0; // gpGlobals->tickcount on server
	virtual float		ServerTickInterval() = 0; // tick interval on server

	virtual float		ClientTime() = 0; // gpGlobals->curtime on client
	virtual float		ClientFrameTime() = 0; // gpGlobals->frametime on client
	virtual int			ClientTick() = 0; // gpGlobals->tickcount on client

	virtual void		SetClientFrameTime(float frametime) = 0; // gpGlobals->frametime on client

	// Currently the engine doesn't like to do networking when it's paused, but if a tool changes entity state, it can be useful to force 
	//  a network update to get that state over to the client
	virtual void		ForceUpdateDuringPause() = 0;

	// Maybe through modelcache???
	virtual model_t* GetModel(void* hEntity) = 0;
	// Get the .mdl file used by entity (if it's a cbaseanimating)
	virtual studiohdr_t* GetStudioModel(void* hEntity) = 0;

	// SINGLE PLAYER/LISTEN SERVER ONLY (just matching the client .dll api for this)
	// Prints the formatted string to the notification area of the screen ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void		Con_NPrintf(int pos, const char* fmt, ...) = 0;
	// SINGLE PLAYER/LISTEN SERVER ONLY(just matching the client .dll api for this)
	// Similar to Con_NPrintf, but allows specifying custom text color and duration information
	virtual void		Con_NXPrintf(const struct con_nprint_s* info, const char* fmt, ...) = 0;

	// Get the current game directory (hl2, tf2, hl1, cstrike, etc.)
	virtual void        GetGameDir(char* szGetGameDir, int maxlength) = 0;

	// Do we need separate rects for the 3d "viewport" vs. the tools surface??? and can we control viewports from
	virtual void		GetScreenSize(int& width, int& height) = 0;

	// GetRootPanel(VPANEL)

	// Sets the location of the main view
	virtual void		SetMainView(const Vector& vecOrigin, const QAngle& angles) = 0;

	// Gets the player view
	virtual bool		GetPlayerView(CViewSetup& playerView, int x, int y, int w, int h) = 0;

	// From a location on the screen, figure out the vector into the world
	virtual void		CreatePickingRay(const CViewSetup& viewSetup, int x, int y, Vector& org, Vector& forward) = 0;

	// precache methods
	virtual bool		PrecacheSound(const char* pName, bool bPreload = false) = 0;
	virtual bool		PrecacheModel(const char* pName, bool bPreload = false) = 0;

	//deleted
};