// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "vfunc_hook.hpp"
#include "hooks.hpp"

vmthook::vmthook() 
{
	memset(this, NULL, sizeof(vmthook));
}

vmthook::vmthook(PDWORD* ppdwClassBase) 
{
	initialize(ppdwClassBase);
}

vmthook::~vmthook() 
{
	unhook();
}

bool vmthook::initialize(PDWORD* ppdwClassBase) 
{
	m_ClassBase = ppdwClassBase;
	m_OldVT = *ppdwClassBase;
	m_VTSize = get_vt_count(*ppdwClassBase);

	m_NewVT = new DWORD[m_VTSize + 1]; //-V121
	memcpy(&m_NewVT[1], m_OldVT, sizeof(DWORD) * m_VTSize);
	m_NewVT[0] = (uintptr_t)m_OldVT[-1]; //-V201 //-V103

	DWORD old;
	VirtualProtect(ppdwClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
	*ppdwClassBase = &m_NewVT[1];
	VirtualProtect(ppdwClassBase, sizeof(DWORD), old, &old);

	return true;
}

bool vmthook::initialize(PDWORD** pppdwClassBase) 
{
	return initialize(*pppdwClassBase);
}

void vmthook::clear_class_base() 
{
	m_ClassBase = NULL;
}

void vmthook::unhook() 
{
	if (m_ClassBase)
	{
		DWORD old;
		VirtualProtect(m_ClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
		*m_ClassBase = m_OldVT;
		VirtualProtect(m_ClassBase, sizeof(DWORD), old, &old);
	}
}

void vmthook::rehook()
{
	if (m_ClassBase) 
	{
		DWORD old;
		VirtualProtect(m_ClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
		*m_ClassBase = &m_NewVT[1];
		VirtualProtect(m_ClassBase, sizeof(DWORD), old, &old);
	}
}

int vmthook::get_func_count() {

	return (int)m_VTSize;
}

PDWORD vmthook::get_old_vt()
{
	return m_OldVT;
}

DWORD vmthook::hook_function(DWORD dwNewFunc, unsigned int iIndex) 
{
	if (m_NewVT && m_OldVT && iIndex <= m_VTSize) 
	{
		m_NewVT[iIndex + 1] = dwNewFunc;
		return m_OldVT[iIndex];
	}

	return NULL;
}

DWORD vmthook::get_vt_count(PDWORD pdwVMT) 
{
	DWORD dwIndex = NULL;

	for (dwIndex = NULL; pdwVMT[dwIndex]; dwIndex++)
		if (IS_INTRESOURCE((FARPROC)pdwVMT[dwIndex])) 
			break;

	return dwIndex;
}