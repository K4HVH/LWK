#pragma once

#include "..\includes.hpp"
#include "..\sdk\math\Vector.hpp"
#include "..\sdk\misc\CUserCmd.hpp"
#include "..\..\utils\crypt_str.h"

class entity_t;
class player_t;
class matrix3x4_t;
class IClientEntity;
class CGameTrace;
class IMaterial;
class CTraceFilter;
class c_baseplayeranimationstate;

struct datamap_t;

struct Box 
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
};

struct HPInfo
{
	int hp = -1;
	int hp_difference = 0;
	float hp_difference_time = 0.0f;
};

struct SoundInfo
{
	float last_time = FLT_MIN;
	Vector origin = ZERO;
};

namespace util
{
	uintptr_t find_pattern(const char* module_name, const char* pattern, const char* mask);
	uint64_t FindSignature(const char* szModule, const char* szSignature);

	bool visible(const Vector & start, const Vector & end, entity_t * entity, player_t * from);
	bool is_button_down(int code);
	
	int epoch_time();
	void RotateMovement(CUserCmd* cmd, float yaw);
	bool get_bbox(entity_t* e, Box & box, bool player_esp);
	void trace_line(Vector& start, Vector& end, unsigned int mask, CTraceFilter* filter, CGameTrace* tr);
	void clip_trace_to_players(IClientEntity* e, const Vector& start, const Vector& end, unsigned int mask, CTraceFilter* filter, CGameTrace* tr);
	void movement_fix(Vector & wish_angle, CUserCmd * m_pcmd);
	unsigned int find_in_datamap(datamap_t * map, const char *name);
	void color_modulate(float color[3], IMaterial* material);
	bool get_backtrack_matrix(player_t* e, matrix3x4_t* matrix);
	void create_state(c_baseplayeranimationstate* state, player_t* e);
	void update_state(c_baseplayeranimationstate* state, const Vector& angles);
	void reset_state(c_baseplayeranimationstate* state);
	void copy_command(CUserCmd* cmd, int tickbase_shift);
	float get_interpolation();

	template <class T>
	static T* FindHudElement(const char* name)
	{
		static DWORD* pThis = nullptr;

		if (!pThis)
		{
			static auto pThisSignature = util::FindSignature(crypt_str("client.dll"), crypt_str("B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 5D 08"));

			if (pThisSignature)
				pThis = *reinterpret_cast<DWORD**>(pThisSignature + 0x1);
		}

		if (!pThis)
			return 0;

		static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
		return (T*)find_hud_element(pThis, name); //-V204
	}

	template <class Type>
	static Type hook_manual(uintptr_t* vftable, uint32_t index, Type fnNew) 
	{
		DWORD OldProtect;
		Type fnOld = (Type)vftable[index]; //-V108 //-V202

		VirtualProtect((void*)(vftable + index * sizeof(Type)), sizeof(Type), PAGE_EXECUTE_READWRITE, &OldProtect); //-V2001 //-V104 //-V206
		vftable[index] = (uintptr_t)fnNew; //-V108
		VirtualProtect((void*)(vftable + index * sizeof(Type)), sizeof(Type), OldProtect, &OldProtect); //-V2001 //-V104 //-V206

		return fnOld;
	}
}