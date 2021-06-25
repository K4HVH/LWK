// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"

struct RenderableInfo_t
{
	IClientRenderable* m_pRenderable;
	void* m_pAlphaProperty;
	int m_EnumCount;
	int m_nRenderFrame;
	unsigned short m_FirstShadow;
	unsigned short m_LeafList;
	short m_Area;
	uint16_t m_Flags;
	uint16_t m_Flags2;
	Vector m_vecBloatedAbsMins;
	Vector m_vecBloatedAbsMaxs;
	Vector m_vecAbsMins;
	Vector m_vecAbsMaxs;
	int pad;
};

using ListLeavesInBox_t = int(__thiscall*)(void*, const Vector&, const Vector&, unsigned short*, int);

int __fastcall hooks::hooked_listleavesinbox(void* ecx, void* edx, Vector& mins, Vector& maxs, unsigned short* list, int list_max)
{
	static auto original_fn = bspquery_hook->get_func_address <ListLeavesInBox_t> (6);
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

	if (!g_ctx.local())
		return original_fn(ecx, mins, maxs, list, list_max);

	if (!g_cfg.player.type[ENEMY].chams[PLAYER_CHAMS_VISIBLE] && !g_cfg.player.type[TEAM].chams[PLAYER_CHAMS_VISIBLE] && !g_cfg.player.type[LOCAL].chams[PLAYER_CHAMS_VISIBLE] && !g_cfg.player.fake_chams_enable && !g_cfg.player.backtrack_chams)
		return original_fn(ecx, mins, maxs, list, list_max);

	if (*(uint32_t*)_ReturnAddress() != 0x14244489) //-V206
		return original_fn(ecx, mins, maxs, list, list_max);

	auto info = *(RenderableInfo_t**)((uintptr_t)_AddressOfReturnAddress() + 0x14);

	if (!info)
		return original_fn(ecx, mins, maxs, list, list_max);

	if (!info->m_pRenderable)
		return original_fn(ecx, mins, maxs, list, list_max);

	auto e = info->m_pRenderable->GetIClientUnknown()->GetBaseEntity();

	if (!e->is_player())
		return original_fn(ecx, mins, maxs, list, list_max);

	info->m_Flags &= ~0x100;
	info->m_Flags2 |= 0xC0;

	return original_fn(ecx, Vector(-32768.0f, -32768.0f, -32768.0f), Vector(32768.0f, 32768.0f, 32768.0f), list, list_max);
}