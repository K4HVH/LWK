#pragma once
#define NOMINMAX

#include <Windows.h>
#include <cstdint>
#include <stdexcept>
#include <assert.h>

class vmthook
{
public:
	vmthook();

	vmthook(PDWORD* ppdwClassBase);

	~vmthook();

	bool initialize(PDWORD* ppdwClassBase);
	bool initialize(PDWORD** pppdwClassBase);

	void clear_class_base();

	void unhook();

	void rehook();

	int get_func_count();

	template <typename Fn>
	Fn get_func_address(int Index) 
	{
		if (Index >= 0 && Index <= (int)m_VTSize && m_OldVT != NULL) 
			return reinterpret_cast<Fn>(m_OldVT[Index]);

		return NULL;
	}

	PDWORD get_old_vt();

	DWORD hook_function(DWORD dwNewFunc, unsigned int iIndex);

private:
	DWORD get_vt_count(PDWORD pdwVMT);

	PDWORD*	m_ClassBase;
	PDWORD	m_NewVT, m_OldVT;
	DWORD	m_VTSize;
};

class ProtectGuard
{
public:

	ProtectGuard(void *base, uint32_t len, uint32_t protect) //-V688
	{
		this->base = base;
		this->len = len;

		VirtualProtect(base, len, protect, (PDWORD)&old_protect); //-V2001
	}

	~ProtectGuard()
	{
		VirtualProtect(base, len, old_protect, (PDWORD)&old_protect); //-V2001
	}

private:

	void *base;
	uint32_t len;
	uint32_t old_protect;
};

class ShadowVTManager 
{

public:

	ShadowVTManager() : class_base(nullptr), method_count(0), shadow_vtable(nullptr), original_vtable(nullptr) {}
	ShadowVTManager(void *base) : class_base(base), method_count(0), shadow_vtable(nullptr), original_vtable(nullptr) {}
	~ShadowVTManager()
	{
		RestoreTable();

		delete[] shadow_vtable;
	}

	inline void Setup(void *base = nullptr)
	{
		if (base != nullptr)
			class_base = base;

		if (class_base == nullptr)
			return;

		original_vtable = *(uintptr_t**)class_base;
		method_count = GetMethodCount(original_vtable);

		if (method_count == 0)
			return;

		shadow_vtable = new uintptr_t[method_count + 1]();

		shadow_vtable[0] = original_vtable[-1];
		std::memcpy(&shadow_vtable[1], original_vtable, method_count * sizeof(uintptr_t));

		try
		{
			auto guard = ProtectGuard{ class_base, sizeof(uintptr_t), PAGE_READWRITE };
			*(uintptr_t**)class_base = &shadow_vtable[1];
		}
		catch (...)
		{
			delete[] shadow_vtable;
		}
	}

	template<typename T>
	inline void Hook(uint32_t index, T method)
	{
		shadow_vtable[index + 1] = reinterpret_cast<uintptr_t>(method);
	}

	inline void Unhook(uint32_t index)
	{
		shadow_vtable[index + 1] = original_vtable[index];
	}

	template<typename T>
	inline T GetOriginal(uint32_t index)
	{
		return (T)original_vtable[index];
	}

	inline void RestoreTable()
	{
		try
		{
			if (original_vtable != nullptr)
			{
				auto guard = ProtectGuard{ class_base, sizeof(uintptr_t), PAGE_READWRITE };
				*(uintptr_t**)class_base = original_vtable;
				original_vtable = nullptr;
			}
		}
		catch (...) {} //-V565
	}

private:

	inline uint32_t GetMethodCount(uintptr_t *vtable_start) //-V2009
	{
		uint32_t len = -1;

		do ++len; while (vtable_start[len]);

		return len;
	}

	void *class_base;
	uint32_t method_count;
	uintptr_t *shadow_vtable;
	uintptr_t *original_vtable;
};