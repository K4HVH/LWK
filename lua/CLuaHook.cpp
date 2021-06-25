// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "CLuaHook.h"

void c_lua_hookManager::registerHook(std::string eventName, int scriptId, sol::protected_function func) { //-V813
	c_lua_hook hk = { scriptId, func };

	this->hooks[eventName].push_back(hk);
}

void c_lua_hookManager::unregisterHooks(int scriptId) {
	for (auto& ev : this->hooks) {
		int pos = 0;

		for (auto& hk : ev.second) {
			if (hk.scriptId == scriptId)
				ev.second.erase(ev.second.begin() + pos);

			pos++;
		}
	}
}

std::vector<c_lua_hook> c_lua_hookManager::getHooks(std::string eventName) { //-V813
	return this->hooks[eventName];
}
