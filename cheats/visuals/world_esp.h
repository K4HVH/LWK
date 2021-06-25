#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

class weapon_t;
class Box;

class worldesp : public singleton <worldesp> 
{
public:
	void paint_traverse();
	void skybox_changer();
	void fog_changer();
	void world_modulation(entity_t* entity);
	void molotov_timer(entity_t* entity);
	void smoke_timer(entity_t* entity);
	void grenade_projectiles(entity_t* entity);
	void bomb_timer(entity_t* entity);
	void dropped_weapons(entity_t* entity);

	bool changed = false;
	std::string backup_skybox = "";
};