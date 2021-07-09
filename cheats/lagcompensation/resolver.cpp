// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "animation_system.h"
#include "..\ragebot\aim.h"

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

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}

void resolver::resolve_yaw()
{
	player_info_t player_info;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	if (player_info.fakeplayer || !g_ctx.local()->is_alive() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum()) //-V807
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	if (g_ctx.globals.missed_shots[player->EntIndex()] >= 2 || g_ctx.globals.missed_shots[player->EntIndex()] && aim::get().last_target[player->EntIndex()].record.type != LBY)
	{
		switch (last_side)
		{
		case RESOLVER_ORIGINAL:
			g_ctx.globals.missed_shots[player->EntIndex()] = 0;
			fake = true;
			break;
		case RESOLVER_ZERO:
			player_record->type = BRUTEFORCE;
			player_record->side = RESOLVER_LOW_FIRST;

			was_first_bruteforce = false;
			was_second_bruteforce = false;
			return;
		case RESOLVER_FIRST:
			player_record->type = BRUTEFORCE;
			player_record->side = was_second_bruteforce ? RESOLVER_ZERO : RESOLVER_SECOND;

			was_first_bruteforce = true;
			return;
		case RESOLVER_SECOND:
			player_record->type = BRUTEFORCE;
			player_record->side = was_first_bruteforce ? RESOLVER_ZERO : RESOLVER_FIRST;

			was_second_bruteforce = true;
			return;
		case RESOLVER_LOW_FIRST:
			player_record->type = BRUTEFORCE;
			player_record->side = RESOLVER_LOW_SECOND;
			return;
		case RESOLVER_LOW_SECOND:
			player_record->type = BRUTEFORCE;
			player_record->side = RESOLVER_FIRST;
			return;
		}
	}

	auto animstate = player->get_animation_state();

	if (!animstate)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	if (fabs(original_pitch) > 85.0f)
		fake = true;
	else if (!fake)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	auto delta = math::normalize_yaw(player->m_angEyeAngles().y - original_goal_feet_yaw);
	auto valid_lby = true;

	if (animstate->m_velocity > 0.1f || fabs(animstate->flUpVelocity) > 100.f)
		valid_lby = animstate->m_flTimeSinceStartedMoving < 0.22f;

	if (fabs(delta) > 30.0f && valid_lby)
	{
		if (g_ctx.globals.missed_shots[player->EntIndex()])
			delta = -delta;

		if (delta > 30.0f)
		{
			player_record->type = LBY;
			player_record->side = RESOLVER_FIRST;
		}
		else if (delta < -30.0f)
		{
			player_record->type = LBY;
			player_record->side = RESOLVER_SECOND;
		}
	}
	else
	{
		auto trace = false;

		if (m_globals()->m_curtime - lock_side > 2.0f)
		{
			auto first_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player, g_ctx.local());
			auto second_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player, g_ctx.local());

			if (first_visible != second_visible)
			{
				trace = true;
				side = second_visible;
				lock_side = m_globals()->m_curtime;
			}
			else
			{
				auto first_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first));
				auto second_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second));

				if (fabs(first_position - second_position) > 1.0f)
					side = first_position > second_position;
			}
		}
		else
			trace = true;

		if (side)
		{
			player_record->type = trace ? TRACE: DIRECTIONAL;
			player_record->side = RESOLVER_FIRST;
		}
		else
		{
			player_record->type = trace ? TRACE : DIRECTIONAL;
			player_record->side = RESOLVER_SECOND;
		}
	}
}

float resolver::resolve_pitch()
{
	return original_pitch;
}