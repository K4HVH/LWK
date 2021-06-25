#pragma once

#include <Windows.h>
#include "..\..\utils\netmanager.hpp"
#include "..\..\utils\crypt_str.h"

#define OFFSET(type, name, offset)\
type &name##() const\
{\
	return *(type*)(uintptr_t(this) + offset);\
}

class CPlayerResource 
{

};

class C_CSPlayerResource : public CPlayerResource
{
public:
	int GetPing(int index)
	{
		static auto m_iPing = netvars::get().get_offset(crypt_str("CCSPlayerResource"), crypt_str("m_iPing"));
		return *(int*)((uintptr_t)this + m_iPing + index * sizeof(int));
	}

	int GetKills(int index)
	{
		static auto m_iKills = netvars::get().get_offset(crypt_str("CCSPlayerResource"), crypt_str("m_iKills"));
		return *(int*)((uintptr_t)this + m_iKills + index * sizeof(int));
	}

	int GetAssists(int index)
	{
		static auto m_iAssists = netvars::get().get_offset(crypt_str("CCSPlayerResource"), crypt_str("m_iAssists"));
		return *(int*)((uintptr_t)this + m_iAssists + index * sizeof(int));
	}

	int GetDeaths(int index)
	{
		static auto m_iDeaths = netvars::get().get_offset(crypt_str("CCSPlayerResource"), crypt_str("m_iDeaths"));
		return *(int*)((uintptr_t)this + m_iDeaths + index * sizeof(int));
	}

	bool GetConnected(int index)
	{
		static auto m_bConnected = netvars::get().get_offset(crypt_str("CCSPlayerResource"), crypt_str("m_bConnected"));
		return *(bool*)((uintptr_t)this + m_bConnected + index * sizeof(bool));
	}
};

class CSGameRulesProxy
{
public:
	OFFSET(int, m_iRoundTime, 0x44);
	OFFSET(float, m_fRoundStartTime, 0x4C);
	OFFSET(bool, m_bIsValveDS, 0x75);
};