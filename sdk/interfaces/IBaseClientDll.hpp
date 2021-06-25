#pragma once

#include "IAppSystem.hpp"
#include "..\misc\GlobalVars.hpp"
#include "..\misc\ClientClass.hpp"
#include "..\math\Vector.hpp"

enum ClientFrameStage_t
{
    FRAME_UNDEFINED = -1,
    FRAME_START,
    FRAME_NET_UPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    FRAME_NET_UPDATE_END,
    FRAME_RENDER_START,
    FRAME_RENDER_END
};

enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,
	//	DATA_UPDATE_ENTERED_PVS,
	DATA_UPDATE_DATATABLE_CHANGED
	//	DATA_UPDATE_LEFT_PVS,
	//DATA_UPDATE_DESTROYED,
};

// Used by RenderView
enum RenderViewInfo_t
{
    RENDERVIEW_UNSPECIFIED = 0,
    RENDERVIEW_DRAWVIEWMODEL = (1 << 0),
    RENDERVIEW_DRAWHUD = (1 << 1),
    RENDERVIEW_SUPPRESSMONITORRENDERING = (1 << 2),
};

// Used by firebullets hook, move somewhere else if u want to cleanup
class C_TEFireBullets
{
public:
	char pad[12];
	int		m_iPlayer; //12
	int _m_iItemDefinitionIndex;
	Vector	_m_vecOrigin;
	Vector	m_vecAngles;
	int		_m_iWeaponID;
	int		m_iMode;
	int		m_iSeed;
	float	m_flSpread;
};

class IBaseClientDLL
{
public:
    virtual int              Connect(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
    virtual int              Disconnect(void) = 0;
    virtual int              Init(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
    virtual void             PostInit() = 0;
    virtual void             Shutdown(void) = 0;
    virtual void             LevelInitPreEntity(char const* pMapName) = 0;
    virtual void             LevelInitPostEntity() = 0;
    virtual void             LevelShutdown(void) = 0;
    virtual ClientClass*     GetAllClasses(void) = 0;

	bool DispatchUserMessage(int messageType, int arg, int arg1, void* data)
	{
		using DispatchUserMessage_t = bool* (__thiscall*)(void*, int, int, int, void*);
		return call_virtual<DispatchUserMessage_t>(this, 38)(this, messageType, arg, arg1, data);
	}
};