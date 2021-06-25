// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "knifebot.h"
#include "..\misc\misc.h"
#include "..\misc\logs.h"
#include "..\autowall\autowall.h"
#include "..\misc\prediction_system.h"
#include "..\fakewalk\slowwalk.h"
#include "..\lagcompensation\local_animations.h"
#include <random>

void knifebot::run(CUserCmd* cmd)
{
	final_target.reset();

	if (!g_cfg.ragebot.enable)
		return;

	if (!g_cfg.ragebot.knife_bot)
		return;

	if (!g_ctx.globals.weapon->is_knife())
		return;

	scan_targets();

	if (!final_target.record)
		return;

	fire(cmd);
}

void knifebot::scan_targets()
{
	if (aim::get().targets.empty())
		return;

	for (auto& target : aim::get().targets)
	{
		if (target.history_record->valid())
		{
			if (target.last_record->valid())
			{
				auto last_distance = g_ctx.globals.eye_pos.DistTo(target.last_record->origin);
				auto history_distance = g_ctx.globals.eye_pos.DistTo(target.history_record->origin);

				final_target.record = last_distance > history_distance ? target.history_record : target.last_record;
				final_target.record->adjust_player();
			}
			else
			{
				final_target.record = target.history_record;
				final_target.record->adjust_player();
			}
		}
		else
		{
			final_target.record = target.last_record;
			final_target.record->adjust_player();
		}
	}
}

int GetMinimalHp() {
	if (TICKS_TO_TIME(g_ctx.globals.fixed_tickbase) > (g_ctx.globals.weapon->m_flNextPrimaryAttack() + 0.4f))
		return 34;

	return 21;
}

void knifebot::fire(CUserCmd* cmd)
{
	if (!g_ctx.globals.weapon->can_fire(false))
		return;

	auto vecOrigin = final_target.record->player->m_vecOrigin();

	auto vecOBBMins = final_target.record->player->GetCollideable()->OBBMins();
	auto vecOBBMaxs = final_target.record->player->GetCollideable()->OBBMaxs();

	auto vecMins = vecOBBMins + vecOrigin;
	auto vecMaxs = vecOBBMaxs + vecOrigin;

	auto vecEyePos = final_target.record->player->get_shoot_position();

	if (vecMins < vecEyePos)
		vecMins = vecEyePos;

	if (vecMins > vecMaxs)
		vecMins = vecMaxs;

	auto vecDelta = vecMins - g_ctx.globals.eye_pos;

	if (vecDelta.Length() > 60.0f)
		return;

	vecDelta.Normalize();
	auto delta = fabs(math::normalize_yaw(final_target.record->angles.y - math::calculate_angle(final_target.record->player->get_shoot_position(), g_ctx.local()->GetAbsOrigin()).y));

	if (final_target.record->player->m_iHealth() > 46 && delta < 120.0f)
	{	
		cmd->m_viewangles = vecDelta.ToEulerAngles();
		cmd->m_buttons |= IN_ATTACK;
		cmd->m_tickcount = TIME_TO_TICKS(final_target.record->simulation_time + util::get_interpolation());
	}

	if (!determinate_hit_type(1, vecDelta))
		return;

	cmd->m_viewangles = vecDelta.ToEulerAngles();
	cmd->m_buttons |= IN_ATTACK2;
	cmd->m_tickcount = TIME_TO_TICKS(final_target.record->simulation_time + util::get_interpolation());
}

int knifebot::determinate_hit_type(bool stab_type, const Vector& delta) 
{
	auto minimum_distance = stab_type ? 32.0f : 48.0f;
	auto end = g_ctx.globals.eye_pos + delta * minimum_distance;

	CTraceFilter filter;
	filter.pSkip = g_ctx.local();

	trace_t trace;
	Ray_t ray;

	ray.Init(g_ctx.globals.eye_pos, end, Vector(-16.0f, -16.0f, -18.0f), Vector(16.0f, 16.0f, 18.0f));
	m_trace()->TraceRay(ray, 0x200400B, &filter, &trace);

	if (trace.hit_entity != final_target.record->player)
		return 0;

	auto cos_pitch = cos(DEG2RAD(final_target.record->angles.x));

	auto sin_yaw = 0.0f;
	auto cos_yaw = 0.0f;

	DirectX::XMScalarSinCos(&sin_yaw, &cos_yaw, DEG2RAD(final_target.record->angles.y));

	auto final_delta = final_target.record->origin - g_ctx.globals.eye_pos;
	return (int)(cos_yaw * cos_pitch * final_delta.x + sin_yaw * cos_pitch * final_delta.y >= 0.475f) + 1;
}