// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"

using BeginFrame_t = void(__thiscall*)(void*, float);

void __fastcall hooks::hooked_beginframe(void* ecx, void* edx, float ft) 
{
	static auto original_fn = materialsys_hook->get_func_address <BeginFrame_t> (42);
	return original_fn(ecx, ft);
}

_declspec(noinline)const char* hooks::getforeignfallbackfontname_detour(void* ecx, uint32_t i)
{
	if (g_ctx.last_font_name.empty())
		return ((GetForeignFallbackFontNameFn)original_getforeignfallbackfontname)(ecx);

	return g_ctx.last_font_name.c_str();
}

const char* __fastcall hooks::hooked_getforeignfallbackfontname(void* ecx, uint32_t i)
{
	return getforeignfallbackfontname_detour(ecx, i);
}