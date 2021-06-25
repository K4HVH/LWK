#pragma once
#include "sol.hpp"

#include <map>

struct c_lua_hook {
	int scriptId;
	sol::protected_function func;
};

class c_lua_hookManager {
public:
	void registerHook(std::string eventName, int scriptId, sol::protected_function func);
	void unregisterHooks(int scriptId);

	std::vector<c_lua_hook> getHooks(std::string eventName);

private:
	std::map<std::string, std::vector<c_lua_hook>> hooks;
};