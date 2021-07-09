// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "animation_system.h"
#include "..\ragebot\aim.h"

#define TIME_TO_TICKS(t) ((int)(0.5f + (float)(t) / m_globals()->m_intervalpertick))

int resolver::GetChokedPackets(player_t* entity)
{
	int last_ticks[65];

	auto ticks = TIME_TO_TICKS(entity->m_flSimulationTime() - entity->m_flOldSimulationTime());
	if (ticks == 0 && last_ticks[entity->EntIndex()] > 0)
	{
		return last_ticks[entity->EntIndex()] - 1;
	}
	else
	{
		last_ticks[entity->EntIndex()] = ticks;
		return ticks;
	}
}

bool resolver::IsLBYdesync(player_t* record) {

	if (GetChokedPackets(record) >= 1 && record->m_vecVelocity().Length2D() <= 0.15)
		return true;
	else //наху€ это ???
		return false;
}

void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
	player = e;
	player_record = record;

	original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
	original_pitch = math::normalize_pitch(pitch);
}

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	side = false;
	fake = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	m_flBrute = false;

	IsLbyDesync = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
	BrutSide = 0.0f;
}

void resolver::resolve_yaw()
{
	player_info_t player_info;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	switch (g_ctx.globals.missed_shots[player->EntIndex()] % 7)
	{
	case 0:
		player_record->type = ORIGINAL;
		player_record->side = RESOLVER_ORIGINAL;
		break;
	case 1:
		player_record->type = ZARO;
		player_record->side = RESOLVER_ZERO;
		return;
	case 2:
		player_record->type = FIRST2;
		player_record->side = RESOLVER_FIRST;
		return;
	case 3:
		player_record->type = SECOND2;
		player_record->side = RESOLVER_SECOND;
		return;
	case 4:
		player_record->type = THIRD;
		player_record->side = RESOLVER_THIRD;
		return;
	case 5:
		player_record->type = LOW_FIRST;
		player_record->side = RESOLVER_LOW_FIRST;
		return;
	case 6:
		player_record->type = LOW_SECOND;
		player_record->side = RESOLVER_LOW_SECOND;
		return;
	}
}

void resolver::good_resik(player_t* player)
{
	auto animstate = player->get_animation_state();
	auto last_anims_update_time = player->get_animation_state()->m_flLastClientSideAnimationUpdateTime;
	int m_Side;

	if (!animstate)
		return;

	AnimationLayer layers[13];
	AnimationLayer moveLayers[3][13];

	AnimationLayer preserver_anim_layers[13];
	auto speed = player->m_vecVelocity().Length2D();

	if (speed < 0.1f)
	{
		auto delta = std::remainder(math::normalize_yaw(player->m_angEyeAngles().y - player->m_flLowerBodyYawTarget()), 360.f) <= 0.f;

		if (layers[3].m_flWeight == 0.0f
			&& layers[3].m_flCycle == 0.0f)
		{
			m_Side = math::clamp((2 * (delta <= 0.f) - 1), -1, 1);
		}
	}
	else if (!int(layers[12].m_flWeight * 1000.f))
	{
		if (int(layers[6].m_flWeight * 1000.f) == int(layers[6].m_flWeight * 1000.f))
		{
			float delta1 = abs(layers[6].m_flPlaybackRate - moveLayers[0][6].m_flPlaybackRate);
			float delta2 = abs(layers[6].m_flPlaybackRate - moveLayers[1][6].m_flPlaybackRate);
			float delta3 = abs(layers[6].m_flPlaybackRate - moveLayers[2][6].m_flPlaybackRate);

			if (delta1 < delta3 || delta2 <= delta3 || (int)(float)(delta3 * 1000.0f)) {
				if (delta1 >= delta2 && delta3 > delta2 && !(int)(float)(delta2 * 1000.0f))
				{
					m_Side = 1;
					last_anims_update_time = m_globals()->m_realtime;
				}
			}
			else
			{
				m_Side = -1;
				last_anims_update_time = m_globals()->m_realtime;
			}
		}
	}

	if (m_Side = 1)
	{
		animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 58.f);
	}
	else if (m_Side = -1)
	{
		animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 58.f);
	}

	if (g_ctx.globals.missed_shots[player->EntIndex()] > 0)
	{
		switch (g_cfg.ragebot.enable && player->is_alive()) {
		case 1:
			player_record->angles.x = player->m_angEyeAngles().x = 90.f;
			break;
		case 2:
			player_record->angles.x = player->m_angEyeAngles().x = 0.f;
			break;
		case 3:
			player_record->angles.x = player->m_angEyeAngles().x = -90.f;
			break;
		case 4:
			animstate->m_flGoalFeetYaw += 59.0f;
			break;
		case 5:
			animstate->m_flGoalFeetYaw -= 59.0f;
			break;
		case 6:
			animstate->m_flGoalFeetYaw -= 78.0f;
			break;
		case 7:
			animstate->m_flGoalFeetYaw += 78.0f;
			break;
		case 8:
			animstate->m_flGoalFeetYaw -= 23.0f;
			break;
		case 9:
			animstate->m_flGoalFeetYaw += 23.0f;
			break;
		case 10:
			animstate->m_flGoalFeetYaw -= 17.0f;
			break;
		case 11:
			animstate->m_flGoalFeetYaw += 17.0f;
			break;
		case 12:
			animstate->m_flGoalFeetYaw -= 58.0f;
			break;
		case 13:
			animstate->m_flGoalFeetYaw += 58.0f;
			break;
		case 14:
			animstate->m_flGoalFeetYaw -= 60.0f;
			break;
		case 15:
			animstate->m_flGoalFeetYaw += 60.0f;
			break;
		case 16:
			animstate->m_flGoalFeetYaw -= 45.0f;
			break;
		case 17:
			animstate->m_flGoalFeetYaw += 45.0f;
			break;
		case 18:
			animstate->m_flGoalFeetYaw -= 25.0f;
			break;
		case 19:
			animstate->m_flGoalFeetYaw += 25.0f;
			break;
		case 20:
			animstate->m_flGoalFeetYaw -= 35.0f;
			break;
		case 21:
			animstate->m_flGoalFeetYaw += 35.0f;
			break;
		case 22:
			animstate->m_flGoalFeetYaw -= animstate->m_flGoalFeetYaw ? -180 : 180;
			break;
		case 23:
			animstate->m_flGoalFeetYaw += animstate->m_flGoalFeetYaw ? -180 : 180;
			break;
		}
	}
}

float AngleDiff(float destAngle, float srcAngle) {
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.0f);
	if (destAngle > srcAngle) {
		if (delta >= 180)
			delta -= 360;
	}
	else {
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}

void resolver::resolver1(player_t* player) {
	auto animstate = player->get_animation_state();

	if (g_ctx.globals.missed_shots[player->EntIndex()] > 0) {

		//a wise old man once told me that the max desync angle is not 60, its a server setting, the max is 90 cuz logicaly a player has 360 angle fov 
		//so 1/2 of a 360 is 180 and a 1/2 of 180 is 90

		switch (math::IsNearEqual(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, 60.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 60);
			animstate->m_flGoalFeetYaw += 60.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 63);
			animstate->m_flGoalFeetYaw += 63.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 65);
			animstate->m_flGoalFeetYaw += 65.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 67);
			animstate->m_flGoalFeetYaw += 67.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 69);//the funniest and wankiest angle of all time
			animstate->m_flGoalFeetYaw += 69.f;
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(60.0f, 69.0f));
			break;
		}

		switch (math::IsNearEqual(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, -60.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 60);
			animstate->m_flGoalFeetYaw -= 60.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 63);
			animstate->m_flGoalFeetYaw -= 63.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 65);
			animstate->m_flGoalFeetYaw -= 65.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 67);
			animstate->m_flGoalFeetYaw -= 67.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 69);//the funniest and wankiest angle of all time
			animstate->m_flGoalFeetYaw -= 69.f;
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-69.0f, -60.0f));
			break;
		}

		switch (math::IsNearEqual(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, 90.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 90);
			animstate->m_flGoalFeetYaw += 90.f;
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(0.0f, 90.0f));
			break;
		}

		switch (math::IsNearEqual(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, -90.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 90);
			animstate->m_flGoalFeetYaw -= 90.f;
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-90.0f, 0.0f));
			break;
		}

		switch (math::IsNearEqual(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, 50.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 50);
			animstate->m_flGoalFeetYaw += 50.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 53);
			animstate->m_flGoalFeetYaw += 53.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 57);
			animstate->m_flGoalFeetYaw += 57.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 58);
			animstate->m_flGoalFeetYaw += 58.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 59);
			animstate->m_flGoalFeetYaw += 59.f;
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(50.0f, 59.0f));
			break;
		}

		switch (math::IsNearEqual(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, -50.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 50);
			animstate->m_flGoalFeetYaw -= 50.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 53);
			animstate->m_flGoalFeetYaw -= 53.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 57);
			animstate->m_flGoalFeetYaw -= 57.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 58);
			animstate->m_flGoalFeetYaw -= 58.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 59);
			animstate->m_flGoalFeetYaw -= 59.f;
			break;

		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-59.0f, -50.0f));
			break;
		}

		if (animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y < 50.f)) {
			switch (g_cfg.ragebot.enable && player->is_alive()) {
			case 1:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(40.f, 49.f));
				break;
			case 2:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-40.f, -49.f));
				break;
			case 3:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(30.f, 39.f));
				break;
			case 4:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-20.f, -29.f));
				break;
			case 5:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(20.f, 29.f));
				break;
			case 6:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-10.f, -19.f));
				break;
			}
		}
	}
}

void resolver::resolve_logic(player_t* player) {

	player_info_t player_info;

	auto animstate = player->get_animation_state();

	if (math::normalize_yaw(math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y + 90.f)) {
		RightSide = true;

		LeftSide = false;
	}
	else if (math::normalize_yaw(math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y - 90.f)) {

		RightSide = false;

		LeftSide = true;
	}

	if (RightSide) {
		switch (g_cfg.ragebot.enable && player->is_alive()) {
		case 1:
			math::normalize_yaw(math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y + 60.f);
			break;
		case 2:
			math::normalize_yaw(math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y + 180.f);
			break;
		}
	}
	else if (LeftSide) {
		switch (g_cfg.ragebot.enable && player->is_alive()) {
		case 1:
			math::normalize_yaw(math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y - 60.f);
			break;
		case 2:
			math::normalize_yaw(math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y - 180.f);
			break;
		}
	}
}

//pretty much the same thing as resolver::resolver1, but with goalfeet yaw,
//which is used to resolve desync better

void resolver::goal_feet_yaw_bruteforce(player_t* player, bool smt) {

	auto animstate = player->get_animation_state();

	if (g_cfg.ragebot.enable && player->is_alive()) {
		if (math::IsNearEqual(animstate->m_flGoalFeetYaw, smt, 90.f && -90.f)) {
			switch (g_ctx.globals.missed_shots[player->EntIndex()]) {
			case 0:
				animstate->m_flGoalFeetYaw += 90;
				break;
			case 1:
				animstate->m_flGoalFeetYaw -= 90;
				break;
			}
		}
		else if (math::IsNearEqual(animstate->m_flGoalFeetYaw, smt, 60.f && -60.f)) {
			switch (g_ctx.globals.missed_shots[player->EntIndex()]) {
			case 0:
				animstate->m_flGoalFeetYaw += 60;
				break;
			case 1:
				animstate->m_flGoalFeetYaw -= 60;
				break;
			case 2:
				animstate->m_flGoalFeetYaw += 63;
				break;
			case 3:
				animstate->m_flGoalFeetYaw -= 63;
				break;
			case 4:
				animstate->m_flGoalFeetYaw += 65;
				break;
			case 5:
				animstate->m_flGoalFeetYaw -= 65;
				break;
			case 6:
				animstate->m_flGoalFeetYaw += 67;
				break;
			case 7:
				animstate->m_flGoalFeetYaw -= 67;
				break;
			case 8:
				animstate->m_flGoalFeetYaw += 69;
				break;
			case 9:
				animstate->m_flGoalFeetYaw -= 69;
				break;
			}
		}
		else if (math::IsNearEqual(animstate->m_flGoalFeetYaw, smt, 50.f && -50.f)) {
			switch (g_ctx.globals.missed_shots[player->EntIndex()]) {
			case 0:
				animstate->m_flGoalFeetYaw += 50;
				break;
			case 1:
				animstate->m_flGoalFeetYaw -= 50;
				break;
			case 2:
				animstate->m_flGoalFeetYaw += 53;
				break;
			case 3:
				animstate->m_flGoalFeetYaw -= 53;
				break;
			case 4:
				animstate->m_flGoalFeetYaw += 54;
				break;
			case 5:
				animstate->m_flGoalFeetYaw -= 54;
				break;
			case 6:
				animstate->m_flGoalFeetYaw += 55;
				break;
			case 7:
				animstate->m_flGoalFeetYaw -= 55;
				break;
			case 8:
				animstate->m_flGoalFeetYaw += 57;
				break;
			case 9:
				animstate->m_flGoalFeetYaw -= 57;
				break;
			case 10:
				animstate->m_flGoalFeetYaw += 58;
				break;
			case 11:
				animstate->m_flGoalFeetYaw -= 58;
				break;
			}
		}
	}
}

float resolver::resolve_pitch()
{
	return original_pitch;
}