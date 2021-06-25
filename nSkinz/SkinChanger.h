#pragma once

#include "../includes.hpp"
#include "../sdk/Memory.h"
#include "../configs/configs.h"

class GameEvent;	
extern Memory memory;

namespace SkinChanger 
{
    void run(ClientFrameStage_t stage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(IGameEvent* event) noexcept;

	struct PaintKit
	{
		int id;
		std::string name;
		std::string skin_name;

		PaintKit(int id, std::string&& name, std::string&& skin_name) noexcept : id(id), name(name), skin_name(skin_name)
		{

		}

		auto operator<(const PaintKit& other) const noexcept
		{
			return name < other.name;
		}
	};

	extern std::unordered_map <std::string, int> model_indexes;
	extern std::unordered_map <std::string, int> player_model_indexes;

	extern std::vector <PaintKit> skinKits;
	extern std::vector <PaintKit> gloveKits;
	extern std::vector <PaintKit> displayKits;
}