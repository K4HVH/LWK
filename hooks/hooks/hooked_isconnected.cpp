// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"

using IsConnected_t = bool(__fastcall*)(void*);

bool __fastcall hooks::hooked_isconnected(void* ecx, void* edx)
{
	static auto original_fn = engine_hook->get_func_address <IsConnected_t> (27);

	if (!g_cfg.misc.inventory_access || !m_engine()->IsInGame())
		return original_fn(ecx);

	static auto inventory_access = util::FindSignature(crypt_str("client.dll"), crypt_str("84 C0 75 04 B0 01 5F"));

	if ((DWORD)_ReturnAddress() != inventory_access)
		return original_fn(ecx);

	return false;
}

using GetScreenAspectRatio_t = float(__thiscall*)(void*, int, int);

float __fastcall hooks::hooked_getscreenaspectratio(void* ecx, void* edx, int width, int height)
{
	static auto original_fn = engine_hook->get_func_address <GetScreenAspectRatio_t> (101);

	if (!g_cfg.misc.aspect_ratio)
		return original_fn(ecx, width, height);

	return g_cfg.misc.aspect_ratio_amount;
}