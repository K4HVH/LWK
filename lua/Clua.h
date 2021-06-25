#pragma once

//#pragma comment(lib, "Lua.lib")
//#pragma comment(lib, "lua51.lib")
//#pragma comment(lib, "luajit.lib")

#include "CLuaHook.h"
#include "menu_item.h"
#include "..\utils\singleton.h"
#include <filesystem>

class c_lua : public singleton <c_lua>
{
public:
	void initialize();
	void refresh_scripts();

	void load_script(int id);
	void unload_script(int id);

	void reload_all_scripts();
	void unload_all_scripts();

	int get_script_id(const std::string& name);
	int get_script_id_by_path(const std::string& path);

	std::vector <bool> loaded;
	std::vector <std::string> scripts;
	std::vector <std::vector <std::pair <std::string, menu_item>>> items;
	std::unordered_map <int, std::unordered_map <std::string, sol::protected_function>> events;

	c_lua_hookManager hooks;
private:
	std::string get_script_path(const std::string& name);
	std::string get_script_path(int id);

	std::vector <std::filesystem::path> pathes;
};