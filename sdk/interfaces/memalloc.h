#pragma once

class IMemAlloc {
public:
	auto Alloc(int nSize) -> void* {
		using Fn = void*(__thiscall*)(void*, int);
		return call_virtual< Fn >(this, 1)(this, nSize);
	}

	auto Realloc(void* pMem, int nSize) -> void* {
		using Fn = void*(__thiscall*)(void*, void*, int);
		return call_virtual< Fn >(this, 3)(this, pMem, nSize);
	}

	auto Free(void* pMem) -> void {
		using Fn = void(__thiscall*)(void*, void*);
		return call_virtual< Fn >(this, 5)(this, pMem);
	}
};