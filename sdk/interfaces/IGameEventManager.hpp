#pragma once

#include <cstdint>

#define EVENT_DEBUG_ID_INIT 42 
#define EVENT_DEBUG_ID_SHUTDOWN 13  

#define VirtualFn (cast) typedef cast(__thiscall* OriginalFn)

class bf_write;
class bf_read;
class IGameEvent
{
public:
	virtual					~IGameEvent() = 0;
	virtual const char*     GetName() const = 0;

	virtual bool            IsReliable() const = 0;
	virtual bool            IsLocal() const = 0;
	virtual bool            IsEmpty(const char *keyName = nullptr) = 0;

	virtual bool            _GetBool(const char *keyName = nullptr, bool defaultValue = false) = 0;
	virtual int             _GetInt(const char *keyName = nullptr, int defaultValue = 0) = 0;
	virtual uint64_t        _GetUint64(const char *keyName = nullptr, unsigned long defaultValue = 0) = 0;
	virtual float           _GetFloat(const char *keyName = nullptr, float defaultValue = 0.0f) = 0;
	virtual const char*     _GetString(const char *keyName = nullptr, const char *defaultValue = "") = 0;
	virtual const wchar_t*  _GetWString(const char *keyName, const wchar_t *defaultValue = L"") = 0;

	virtual void            _SetBool(const char *keyName, bool value) = 0;
	virtual void            _SetInt(const char *keyName, int value) = 0;
	virtual void            _SetUint64(const char *keyName, unsigned long value) = 0;
	virtual void            _SetFloat(const char *keyName, float value) = 0;

	bool GetBool(const char* keyName)
	{
		return _GetBool(keyName);
	}

	int GetInt(const char* keyName)
	{
		return _GetInt(keyName);
	}

	float GetFloat(const char* keyName)
	{
		return _GetFloat(keyName);
	}

	std::string GetString(const char* keyName)
	{
		return _GetString(keyName);
	}

	void SetBool(const char* keyName, bool value)
	{
		return _SetBool(keyName, value);
	}

	void SetInt(const char* keyName, int value)
	{
		return _SetInt(keyName, value);
	}

	void SetFloat(const char* keyName, float value)
	{
		return _SetFloat(keyName, value);
	}

	void SetString(const char* keyName, const char* value)
	{
		typedef void(__thiscall* OriginalFn)(void*, const char*, const char*);
		call_virtual<OriginalFn>(this, 16)(this, keyName, value);
	}
};

class IGameEventListener2
{
public:
	virtual ~IGameEventListener2(void)
	{

	}

	virtual void FireGameEvent(IGameEvent *event) = 0;
	int m_iDebugId;

	virtual int  GetEventDebugID(void) 
	{ 
		return m_iDebugId;
	};
};

class IGameEventManager2
{
public:
	virtual             ~IGameEventManager2() = 0;
	virtual int         LoadEventsFromFile(const char *filename) = 0;
	virtual void        Reset() = 0;
	virtual bool        AddListener(IGameEventListener2 *listener, const char *name, bool bServerSide) = 0;
	virtual bool        FindListener(IGameEventListener2 *listener, const char *name) = 0;
	virtual int         RemoveListener(IGameEventListener2 *listener) = 0;
	virtual IGameEvent* CreateEvent(const char *name, bool bForce, unsigned int dwUnknown) = 0;
	virtual bool        FireEvent(IGameEvent *event, bool bDontBroadcast = false) = 0;
	virtual bool        FireEventClientSide(IGameEvent *event) = 0;
	virtual IGameEvent* DuplicateEvent(IGameEvent *event) = 0;
	virtual void        FreeEvent(IGameEvent *event) = 0;
	virtual bool        SerializeEvent(IGameEvent *event, bf_write *buf) = 0;
	virtual IGameEvent* UnserializeEvent(bf_read *buf) = 0;
};