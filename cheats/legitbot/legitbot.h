#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

struct target_info
{
	float fov;
	int hitbox;
};

class legit_bot : public singleton <legit_bot>
{
public:
	void createmove(CUserCmd* cmd);

	void find_target();
	void find_best_point(CUserCmd* cmd, float fov);

	void auto_pistol(CUserCmd* cmd);
	void do_rcs(CUserCmd* cmd);
	void calc_fov();
	void calc_smooth();
	void do_smooth(const Vector& currentAngle, const Vector& aimAngle, Vector& angle);
	int hitchance(player_t* target, const Vector& aim_angle, const Vector& point, int hitbox);
	bool hitbox_intersection(player_t* e, matrix3x4_t* matrix, int hitbox, const Vector& start, const Vector& end);
	player_t* target;
	int tickcount = -1;
	int target_index = -1;
	float target_switch_delay;
private:
	target_info targets[65];
	float target_fov;
	float fov_t;
	float smooth_t;
	float reaction_t;
	Vector point = Vector(0, 0, 0);
	Vector aim_punch = Vector(0, 0, 0);
	Vector aim_angle = Vector(0, 0, 0);
};
