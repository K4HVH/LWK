// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"

using FindMDL_t = void(__thiscall*)(void*, char*);

void __fastcall hooks::hooked_findmdl(void* ecx, void* edx, char* FilePath)
{
	static auto original_fn = modelcache_hook->get_func_address <FindMDL_t> (10);
	return original_fn(ecx, FilePath);
}