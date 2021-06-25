#pragma once

#include "..\autowall\autowall.h"
#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

class antiaim : public singleton <antiaim>
{
public:
	void create_move(CUserCmd* m_pcmd);
	float get_pitch(CUserCmd* m_pcmd);
	float get_yaw(CUserCmd* m_pcmd);
	bool condition(CUserCmd* m_pcmd, bool dynamic_check = true);

	bool should_break_lby(CUserCmd* m_pcmd, int lby_type);
	float at_targets();
	bool automatic_direction();
	void freestanding(CUserCmd* m_pcmd);

	int type = 0;
	int manual_side = -1;
	int final_manual_side = -1;
	bool flip = false;
	bool freeze_check = false;
	bool breaking_lby = false;
	float desync_angle = 0.0f;
};

enum
{
	SIDE_NONE = -1,
	SIDE_BACK,
	SIDE_LEFT,
	SIDE_RIGHT
};