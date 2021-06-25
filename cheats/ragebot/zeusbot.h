#pragma once
#include "..\..\includes.hpp"
#include "..\lagcompensation\animation_system.h"
#include "aim.h"

class zeusbot : public singleton <zeusbot>
{
	void scan_targets();
	void scan(adjust_data* record, scan_data& data);
	void find_best_target();
	void fire(CUserCmd* cmd);
	int hitchance(const Vector& aim_angle);
	bool hitbox_intersection(player_t* e, matrix3x4_t* matrix, int hitbox, const Vector& start, const Vector& end, float* safe = nullptr);

	std::vector <scanned_target> scanned_targets;
	scanned_target final_target;
public:
	void run(CUserCmd* cmd);
};