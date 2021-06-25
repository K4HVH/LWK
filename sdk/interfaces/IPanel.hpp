#pragma once
#include "..\misc\vfunc.hpp"

class IPanel
{
public:
	const char* GetName(unsigned int vguiPanel)
	{
		typedef const char *(__thiscall* tGetName)(void*, unsigned int);
		return call_virtual<tGetName>(this, 36)(this, vguiPanel);
	}
};
