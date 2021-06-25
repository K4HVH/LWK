// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\includes.hpp"
#include "recv.h"
#include <cstdint>

namespace fnv
{
	constexpr uint32_t offsetBasis = 0x811c9dc5;
	constexpr uint32_t prime = 0x1000193;

	constexpr uint32_t hash(const char* str, const uint32_t value = offsetBasis) noexcept
	{
		return *str ? hash(str + 1, (value ^ *str) * static_cast<unsigned long long>(prime)) : value;
	}

	constexpr uint32_t hashRuntime(const char* str) noexcept
	{
		auto value = offsetBasis;

		while (*str) 
		{
			value ^= *str++;
			value *= prime;
		}

		return value;
	}
}

static int random(int min, int max) noexcept
{
	return math::random_int(min, max);
}

static std::unordered_map <uint32_t, recvProxy> proxies;

static int get_new_animation(const uint32_t model, const int sequence) noexcept
{
	enum Sequence
	{
		SEQUENCE_DEFAULT_DRAW = 0,
		SEQUENCE_DEFAULT_IDLE1 = 1,
		SEQUENCE_DEFAULT_IDLE2 = 2,
		SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
		SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
		SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
		SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
		SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
		SEQUENCE_DEFAULT_LOOKAT01 = 12,

		SEQUENCE_BUTTERFLY_DRAW = 0,
		SEQUENCE_BUTTERFLY_DRAW2 = 1,
		SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
		SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

		SEQUENCE_FALCHION_IDLE1 = 1,
		SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
		SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
		SEQUENCE_FALCHION_LOOKAT01 = 12,
		SEQUENCE_FALCHION_LOOKAT02 = 13,

		SEQUENCE_DAGGERS_IDLE1 = 1,
		SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
		SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
		SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
		SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

		SEQUENCE_BOWIE_IDLE1 = 1,
	};
	

	switch (model) 
	{
	case fnv::hash("models/weapons/v_knife_butterfly.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
		default:
			return sequence + 1;
		}
	}
	case fnv::hash("models/weapons/v_knife_falchion_advanced.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	}
	case fnv::hash("models/weapons/v_knife_push.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return random(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	}
	case fnv::hash("models/weapons/v_knife_survival_bowie.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	}
	case fnv::hash("models/weapons/v_knife_ursus.mdl"):
	case fnv::hash("models/weapons/v_knife_skeleton.mdl"):
	case fnv::hash("models/weapons/v_knife_outdoor.mdl"):
	case fnv::hash("models/weapons/v_knife_cord.mdl"):
	case fnv::hash("models/weapons/v_knife_canis.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	}
	case fnv::hash("models/weapons/v_knife_stiletto.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random(12, 13);
		}
	} //-V796
	case fnv::hash("models/weapons/v_knife_widowmaker.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random(14, 15);
		}
	}
	default:
		return sequence;
	}
}

static void viewModelSequence(recvProxyData& data, void* arg2, void* arg3) noexcept
{
	if (g_ctx.local())
	{
		if (const auto activeWeapon = g_ctx.local()->m_hActiveWeapon())
		{
			if (const auto weapon_info = game_data::get_weapon_info(activeWeapon->m_iItemDefinitionIndex()))
				data.value._int = get_new_animation(fnv::hashRuntime(weapon_info->model), data.value._int);
			else if (g_cfg.skins.rare_animations && activeWeapon->m_iItemDefinitionIndex() == WEAPON_DEAGLE && data.value._int == 7)
				data.value._int = 8;
			else if (g_cfg.skins.rare_animations && activeWeapon->m_iItemDefinitionIndex() == WEAPON_KNIFE_FALCHION && data.value._int == 12)
				data.value._int = 13;
		}
	}

	constexpr auto hash{ fnv::hash("CBaseViewModel->m_nSequence") };
	proxies[hash](data, arg2, arg3);
}

static void spottedHook(recvProxyData& data, void* arg2, void* arg3) noexcept
{
	data.value._int = g_cfg.ragebot.enable;

	constexpr auto hash{ fnv::hash("CBaseEntity->m_bSpotted") };
	proxies[hash](data, arg2, arg3);
}

static void m_flSimulationTimeHook(recvProxyData& data, void* arg2, void* arg3) noexcept
{
	if (!data.value._int)
		return;

	constexpr auto hash{ fnv::hash("CBaseEntity->m_flSimulationTime") };
	proxies[hash](data, arg2, arg3);
}

Netvars::Netvars() noexcept
{
	for (auto clientClass = m_client()->GetAllClasses(); clientClass; clientClass = clientClass->m_pNext)
		walkTable(false, clientClass->m_pNetworkName, clientClass->m_pRecvTable);
}

void Netvars::restore() noexcept
{
	for (auto clientClass = m_client()->GetAllClasses(); clientClass; clientClass = clientClass->m_pNext)
		walkTable(true, clientClass->m_pNetworkName, clientClass->m_pRecvTable);

	proxies.clear();
	offsets.clear();
}

void Netvars::walkTable(bool unload, const char* networkName, RecvTable* recvTable, const std::size_t offset) noexcept
{
	for (auto i = 0; i < recvTable->propCount; i++)
	{
		auto& prop = recvTable->props[i];

		if (isdigit(prop.name[0]))
			continue;

		if (fnv::hashRuntime(prop.name) == fnv::hash(crypt_str("baseclass")))
			continue;

		if (prop.type == 6 && prop.dataTable && prop.dataTable->netTableName[0] == 'D')
			walkTable(unload, networkName, prop.dataTable, prop.offset + offset);

		const auto hash{ fnv::hashRuntime((networkName + std::string { crypt_str("->") } + prop.name).c_str()) };

		constexpr auto getHook
		{ 
			[](uint32_t hash) noexcept -> recvProxy 
			{
				 if (hash == fnv::hash(crypt_str("CBaseViewModel->m_nSequence")))
					 return viewModelSequence;

				 if (hash == fnv::hash(crypt_str("CBaseEntity->m_bSpotted")))
					 return spottedHook;

				 if (hash == fnv::hash(crypt_str("CBaseEntity->m_flSimulationTime")))
					 return m_flSimulationTimeHook;

				 return nullptr;
			} 
		};

		offsets[hash] = uint16_t(offset + prop.offset);

		constexpr auto hookProperty
		{
			[](uint32_t hash, recvProxy& originalProxy, recvProxy proxy) noexcept
			{
				if (originalProxy != proxy)
				{
					proxies[hash] = originalProxy;
					originalProxy = proxy;
				}
			}
		};

		if (auto hook{ getHook(hash) })
			hookProperty(hash, prop.proxy, hook);
	}
}
