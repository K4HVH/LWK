// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "legitbot.h"
#include "..\lagcompensation\animation_system.h"
#include "..\autowall\autowall.h"

bool IsNotTarget(player_t* e)
{
	if (!e)
		return true;

	if (e == g_ctx.local())
		return true;

	if (e->m_iHealth() <= 0)
		return true;

	if (e->m_bGunGameImmunity())
		return true;

	if (e->m_fFlags() & FL_FROZEN)
		return true;

	return false;
}

void legit_bot::do_smooth(const Vector& currentAngle, const Vector& aimAngle, Vector& angle)
{
	auto smooth_value = smooth_t;
	if (smooth_value <= 1) {
		return;
	}

	static auto recoil_scale = m_cvar()->FindVar("weapon_recoil_scale")->GetFloat();
	aim_punch = g_ctx.local()->m_aimPunchAngle() * recoil_scale;
	aim_punch *= (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].rcs / 100.f);

	Vector delta = aimAngle - (currentAngle + aim_punch);
	math::normalize_angles(delta);

	if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].smooth_type == 1) {
		float randomize = math::random_float(-0.2f, 0.2f);
		smooth_value = (m_globals()->m_intervalpertick * 64.0f) * (smooth_value + (randomize * smooth_value));
		smooth_value = math::clamp(smooth_value, m_globals()->m_intervalpertick * 64.0f * smooth_t, 13.f);
	}
	else {
		smooth_value = (m_globals()->m_intervalpertick * 64.0f) * smooth_value;
	}

	delta /= smooth_value;
	angle = currentAngle + delta;
	math::normalize_angles(angle);
}


float dynamic_fov_to(float distance, const Vector& angle, const Vector& viewangles) {
	Vector aimingAt;
	math::angle_vectors(viewangles, aimingAt);
	aimingAt *= distance;
	Vector aimAt;
	math::angle_vectors(angle, aimAt);
	aimAt *= distance;
	return aimingAt.DistTo(aimAt) / 5;
}

float static_fov_to(const Vector& viewAngle, const Vector& aimAngle) {
	Vector delta = aimAngle - viewAngle;
	math::normalize_angles(delta);
	return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
}

bool in_smoke(const Vector& in, const Vector& out)
{
	static auto LineGoesThroughSmokeFn = (bool(*)(Vector vStartPos, Vector vEndPos))util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"));
	return LineGoesThroughSmokeFn(in, out);
}

void legit_bot::createmove(CUserCmd* cmd)
{
	if (!g_cfg.legitbot.enabled)
		return;

	// rata-ta-ta-ta range
	calc_fov();

	bool should_do_rcs = false;
	static bool started_scope = false;
	static bool started_unscope = false;
	static int scope_delay = 0;
	static int autofire_delay = 0;

	// my gun can't shoot nigga >:(
	if (g_ctx.globals.weapon->is_non_aim() || g_ctx.globals.weapon->m_iClip1() <= 0)
	{
		target = nullptr;
		point = Vector(0, 0, 0);
		started_scope = false;
		started_unscope = false;
		scope_delay = 0;
		autofire_delay = 0;
		return;
	}

	if (!g_ctx.globals.weapon->is_sniper())
	{
		started_scope = false;
		started_unscope = false;
		scope_delay = 0;
	}

	if (g_ctx.globals.weapon->is_sniper() && started_unscope && g_ctx.globals.weapon->can_fire(false) && g_ctx.local()->m_bIsScoped())
	{
		scope_delay = 0;
		cmd->m_buttons |= IN_ATTACK2;

		if (g_ctx.globals.weapon->m_zoomLevel() == 3)
			started_unscope = false;

		return;
	}

	// rata-ta-ta-ta speed
	calc_smooth();

	// my pistols do pew-pew
	if (g_cfg.legitbot.autopistol)
		auto_pistol(cmd);

	if (target_switch_delay > m_globals()->m_realtime)
	{
		point = Vector(0, 0, 0);
		target = nullptr;
		autofire_delay = 0;

		if ((key_binds::get().get_key_bind_state(1) || key_binds::get().get_key_bind_state(0)) && !g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].rcs_type)
			do_rcs(cmd);

		return;
	}

	// find the closest enemy to our crosshair
	target_index = -1;
	target = nullptr;
	find_target();

	if (!target->valid(false, true))
		target = nullptr;

	// rata-ta-ta-ta control
	if ((key_binds::get().get_key_bind_state(1) || key_binds::get().get_key_bind_state(0)) && !g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].rcs_type)
		should_do_rcs = true;

	if (!target)
	{
		point = Vector(0, 0, 0);
		autofire_delay = 0;

		if (should_do_rcs)
			do_rcs(cmd);

		return;
	}

	// legitbot key check
	if (!key_binds::get().get_key_bind_state(1) && !key_binds::get().get_key_bind_state(0))
	{
		autofire_delay = 0;

		if (should_do_rcs)
			do_rcs(cmd);

		return;
	}

	// point to rata-ta-ta-ta
	auto is_silent = !g_ctx.local()->m_iShotsFired() && g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].silent_fov;
	point = Vector(0, 0, 0);
	find_best_point(cmd, fov_t);

	if (point == Vector(0, 0, 0))
	{
		autofire_delay = 0;

		if (should_do_rcs)
			do_rcs(cmd);

		if (is_silent)
		{
			find_best_point(cmd, g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].silent_fov);
			if (point == Vector(0, 0, 0))
				return;
		}
		else
			return;
	}

	// is enemy in smoke?
	if (!g_cfg.legitbot.do_if_enemy_in_smoke && in_smoke(g_ctx.globals.eye_pos, target->hitbox_position(0)))
	{
		autofire_delay = 0;

		if (should_do_rcs)
			do_rcs(cmd);

		return;
	}

	// are we flashed?
	if (!g_cfg.legitbot.do_if_local_flashed && g_ctx.local()->m_flFlashDuration() >= 0.1f)
	{
		autofire_delay = 0;

		if (should_do_rcs)
			do_rcs(cmd);

		return;
	}

	// is local in air?
	if (!g_cfg.legitbot.do_if_local_in_air && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
	{
		if (should_do_rcs)
			do_rcs(cmd);

		return;
	}

	if (!g_ctx.local()->m_iShotsFired() && key_binds::get().get_key_bind_state(0) && autofire_delay < g_cfg.legitbot.autofire_delay)
	{
		autofire_delay += 1;
		return;
	}
	else if (!g_ctx.local()->m_iShotsFired())
		autofire_delay = 0;

	auto weapon = g_ctx.globals.weapon;

	if (g_cfg.legitbot.autoscope && (!g_ctx.local()->m_bIsScoped() || (started_scope && scope_delay > 9)) && weapon->is_sniper())
	{
		cmd->m_buttons |= IN_ATTACK2;
		cmd->m_buttons &= ~IN_ATTACK;

		started_scope = true;
		scope_delay += 1;

		return;
	}

	scope_delay = 0;
	started_scope = false;

	// are we not in scope or not ready to shoot after scope and we dont use autoscope?
	if (weapon->is_sniper() &&
		g_cfg.legitbot.sniper_in_zoom_only && !g_cfg.legitbot.autoscope && !g_ctx.local()->m_bIsScoped())
	{
		if (should_do_rcs)
			do_rcs(cmd);

		return;
	}

	auto is_silent_s = !g_ctx.local()->m_iShotsFired() && g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].silent_fov && target_fov <= g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].silent_fov;

	if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].auto_stop)
		cmd->m_forwardmove = cmd->m_sidemove = 0;

	aim_angle = math::calculate_angle(g_ctx.globals.eye_pos, point).Clamp();

	if (!is_silent_s)
		do_smooth(cmd->m_viewangles, aim_angle, aim_angle);

	math::normalize_angles(aim_angle);

	cmd->m_viewangles = aim_angle;
	cmd->m_tickcount = TIME_TO_TICKS(target->m_flSimulationTime() + util::get_interpolation());

	if (!is_silent_s)
		m_engine()->SetViewAngles(cmd->m_viewangles);

	if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].rcs_type || should_do_rcs)
		do_rcs(cmd);

	auto hitchanced = hitchance(target, aim_angle, point, targets[target->EntIndex()].hitbox) >= g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].autofire_hitchance;

	if (key_binds::get().get_key_bind_state(0) && g_ctx.globals.weapon->can_fire(true) && hitchanced)
		cmd->m_buttons |= IN_ATTACK;

	if (cmd->m_buttons & IN_ATTACK && !started_unscope)
		if (g_ctx.globals.weapon->is_sniper() && g_cfg.legitbot.unscope)
			started_unscope = true;
}

void legit_bot::calc_fov()
{
	if (g_ctx.local()->m_iShotsFired() < 2)
		fov_t = g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].fov;

	else if (g_ctx.local()->m_iShotsFired() >= 2 && g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].custom_rcs_fov > 0.f)
		fov_t = g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].custom_rcs_fov;

	else
		fov_t = g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].fov;
}

void legit_bot::calc_smooth()
{
	if (g_ctx.local()->m_iShotsFired() < 2)
		smooth_t = g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].smooth;

	else if (g_ctx.local()->m_iShotsFired() >= 2 && g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].custom_rcs_smooth >= 1.f)
		smooth_t = g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].custom_rcs_smooth;

	else
		smooth_t = g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].smooth;
}

void legit_bot::do_rcs(CUserCmd* cmd)
{
	auto weapon = g_ctx.globals.weapon;
	static Vector prev_punch = { 0.0f, 0.0f, 0.0f };

	if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER
		|| g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].rcs <= 0
		|| weapon->is_non_aim()
		|| g_ctx.local()->m_iShotsFired() < 2)
	{
		prev_punch = Vector(0, 0, 0);
		return;
	}


	static auto recoil_scale = m_cvar()->FindVar("weapon_recoil_scale")->GetFloat();
	aim_punch = g_ctx.local()->m_aimPunchAngle() * recoil_scale;

	// rcs in %
	aim_punch *= (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].rcs / 100.f);

	auto rcs = cmd->m_viewangles += (prev_punch - aim_punch);
	math::normalize_angles(rcs);
	m_engine()->SetViewAngles(rcs);

	prev_punch = aim_punch;
}

void legit_bot::auto_pistol(CUserCmd* cmd)
{
	auto weapon = g_ctx.globals.weapon;

	if (!weapon->is_pistol()
		|| weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER
		|| weapon->m_iClip1() <= 0)
		return;

	static bool is_firing = false;

	if (cmd->m_buttons & IN_ATTACK) {
		if (is_firing) {
			cmd->m_buttons &= ~IN_ATTACK;
		}
	}

	is_firing = cmd->m_buttons & IN_ATTACK;
}

void legit_bot::find_best_point(CUserCmd* cmd, float fov_v)
{
	static auto recoil_scale = m_cvar()->FindVar("weapon_recoil_scale")->GetFloat();
	aim_punch = g_ctx.local()->m_aimPunchAngle() * recoil_scale;
	aim_punch *= (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].rcs / 100.f);

	auto best_fov = fov_v;

	// if nearest aimbot method
	if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].priority == 0)
	{
		for (auto bone : { HITBOX_HEAD, HITBOX_PELVIS, HITBOX_CHEST, HITBOX_UPPER_CHEST, HITBOX_STOMACH })
		{
			float distance = 0.f;
			float fov = FLT_MAX;
			Vector ang = Vector(0, 0, 0);

			auto bone_pos = target->hitbox_position(bone);
			math::vector_angles(bone_pos - g_ctx.globals.eye_pos, ang);
			math::normalize_angles(ang);
			distance = g_ctx.globals.eye_pos.DistTo(bone_pos);

			if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].fov_type == 1)
				fov = dynamic_fov_to(distance, ang, cmd->m_viewangles + aim_punch);
			else
				fov = static_fov_to(cmd->m_viewangles + aim_punch, ang);

			if (fov > best_fov)
				continue;

			auto fire_data = autowall::get().wall_penetration(g_ctx.globals.eye_pos, bone_pos, target);

			if (!fire_data.visible && !g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].awall_dmg)
				continue;

			if (!fire_data.visible && g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].awall_dmg > 0 && fire_data.damage < g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].awall_dmg)
				continue;

			best_fov = fov;
			point = bone_pos;
			target_fov = fov;
			targets[target->EntIndex()].hitbox = bone;
		}
	}
	else if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].priority == 1) // head only
	{
		float distance = 0.f;
		float fov = FLT_MAX;
		Vector ang = Vector(0, 0, 0);

		auto bone_pos = target->hitbox_position(0);
		math::vector_angles(bone_pos - g_ctx.globals.eye_pos, ang);
		math::normalize_angles(ang);
		distance = g_ctx.globals.eye_pos.DistTo(bone_pos);

		if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].fov_type == 1)
			fov = dynamic_fov_to(distance, ang, cmd->m_viewangles + aim_punch);
		else
			fov = static_fov_to(cmd->m_viewangles + aim_punch, ang);

		if (fov > best_fov)
			return;

		auto fire_data = autowall::get().wall_penetration(g_ctx.globals.eye_pos, bone_pos, target);

		if (!fire_data.visible && !g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].awall_dmg)
			return;

		if (!fire_data.visible && g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].awall_dmg > 0 && fire_data.damage < g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].awall_dmg)
			return;

		best_fov = fov;
		point = bone_pos;
		target_fov = fov;
		targets[target->EntIndex()].hitbox = 0;
	}
	else if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].priority == 2) // nearest body
	{
		for (auto bone : { HITBOX_PELVIS, HITBOX_CHEST, HITBOX_UPPER_CHEST, HITBOX_STOMACH }) {
			float distance = 0.f;
			float fov = FLT_MAX;
			Vector ang = Vector(0, 0, 0);

			auto bone_pos = target->hitbox_position(bone);
			math::vector_angles(bone_pos - g_ctx.globals.eye_pos, ang);
			math::normalize_angles(ang);
			distance = g_ctx.globals.eye_pos.DistTo(bone_pos);

			if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].fov_type == 1)
				fov = dynamic_fov_to(distance, ang, cmd->m_viewangles + aim_punch);
			else
				fov = static_fov_to(cmd->m_viewangles + aim_punch, ang);

			if (fov > best_fov)
				continue;

			auto fire_data = autowall::get().wall_penetration(g_ctx.globals.eye_pos, bone_pos, target);

			if (!fire_data.visible && !g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].awall_dmg)
				continue;

			if (!fire_data.visible && g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].awall_dmg > 0 && fire_data.damage < g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].awall_dmg)
				continue;

			best_fov = fov;
			point = bone_pos;
			target_fov = fov;
			targets[target->EntIndex()].hitbox = bone;
		}
	}
}

void legit_bot::find_target()
{
	for (auto i = 0; i < 65; ++i)
	{
		targets[i].fov = 0.0f;
		targets[i].hitbox = -1;
	}

	auto best_fov = FLT_MAX;

	for (auto i = 1; i <= m_globals()->m_maxclients; ++i)
	{
		auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

		if (IsNotTarget(e))
			continue;

		if (!g_cfg.legitbot.friendly_fire && e->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			continue;

		Vector engine_angles;
		m_engine()->GetViewAngles(engine_angles);
		targets[i].fov = math::get_fov(engine_angles, math::calculate_angle(g_ctx.globals.eye_pos, e->hitbox_position(HITBOX_CHEST)));

		if (targets[i].fov < best_fov)
		{
			best_fov = targets[i].fov;
			target = e;
			target_index = i;
		}
	}
}

int legit_bot::hitchance(player_t* target, const Vector& aim_angle, const Vector& point, int hitbox) //-V688
{
	auto final_hitchance = 0;
	auto weapon_info = g_ctx.globals.weapon->get_csweapon_info();

	if (!weapon_info)
		return final_hitchance;

	if ((g_ctx.globals.eye_pos - point).Length() > weapon_info->flRange)
		return final_hitchance;

	auto forward = ZERO;
	auto right = ZERO;
	auto up = ZERO;

	math::angle_vectors(aim_angle, &forward, &right, &up);

	math::fast_vec_normalize(forward);
	math::fast_vec_normalize(right);
	math::fast_vec_normalize(up);

	auto is_special_weapon = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_AWP || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SSG08;
	auto inaccuracy = weapon_info->flInaccuracyStand;

	if (g_ctx.local()->m_fFlags() & FL_DUCKING)
	{
		if (is_special_weapon)
			inaccuracy = weapon_info->flInaccuracyCrouchAlt;
		else
			inaccuracy = weapon_info->flInaccuracyCrouch;
	}
	else if (is_special_weapon)
		inaccuracy = weapon_info->flInaccuracyStandAlt;

	if (g_ctx.globals.inaccuracy - 0.000001f < inaccuracy)
		final_hitchance = 101;
	else
	{
		static auto setup_spread_values = true;
		static float spread_values[256][6];

		if (setup_spread_values)
		{
			setup_spread_values = false;

			for (auto i = 0; i < 256; ++i)
			{
				math::random_seed(i + 1);

				auto a = math::random_float(0.0f, 1.0f);
				auto b = math::random_float(0.0f, DirectX::XM_2PI);
				auto c = math::random_float(0.0f, 1.0f);
				auto d = math::random_float(0.0f, DirectX::XM_2PI);

				spread_values[i][0] = a;
				spread_values[i][1] = c;

				auto sin_b = 0.0f, cos_b = 0.0f;
				DirectX::XMScalarSinCos(&sin_b, &cos_b, b);

				auto sin_d = 0.0f, cos_d = 0.0f;
				DirectX::XMScalarSinCos(&sin_d, &cos_d, d);

				spread_values[i][2] = sin_b;
				spread_values[i][3] = cos_b;
				spread_values[i][4] = sin_d;
				spread_values[i][5] = cos_d;
			}
		}

		auto hits = 0;

		for (auto i = 0; i < 256; ++i)
		{
			auto inaccuracy = spread_values[i][0] * g_ctx.globals.inaccuracy;
			auto spread = spread_values[i][1] * g_ctx.globals.spread;

			auto spread_x = spread_values[i][3] * inaccuracy + spread_values[i][5] * spread;
			auto spread_y = spread_values[i][2] * inaccuracy + spread_values[i][4] * spread;

			auto direction = ZERO;

			direction.x = forward.x + right.x * spread_x + up.x * spread_y;
			direction.y = forward.y + right.y * spread_x + up.y * spread_y;
			direction.z = forward.z + right.z * spread_x + up.z * spread_y; //-V778

			auto end = g_ctx.globals.eye_pos + direction * weapon_info->flRange;

			if (hitbox_intersection(target, target->m_CachedBoneData().Base(), hitbox, g_ctx.globals.eye_pos, end))
				++hits;
		}

		final_hitchance = (int)((float)hits / 2.56f);
	}

	return final_hitchance;
}

static int clip_ray_to_hitbox(const Ray_t& ray, mstudiobbox_t* hitbox, matrix3x4_t& matrix, trace_t& trace)
{
	static auto fn = util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 F3 0F 10 42"));

	trace.fraction = 1.0f;
	trace.startsolid = false;

	return reinterpret_cast <int(__fastcall*)(const Ray_t&, mstudiobbox_t*, matrix3x4_t&, trace_t&)> (fn)(ray, hitbox, matrix, trace);
}

bool legit_bot::hitbox_intersection(player_t* e, matrix3x4_t* matrix, int hitbox, const Vector& start, const Vector& end)
{
	auto model = e->GetModel();

	if (!model)
		return false;

	auto studio_model = m_modelinfo()->GetStudioModel(model);

	if (!studio_model)
		return false;

	auto studio_set = studio_model->pHitboxSet(e->m_nHitboxSet());

	if (!studio_set)
		return false;

	auto studio_hitbox = studio_set->pHitbox(hitbox);

	if (!studio_hitbox)
		return false;

	trace_t trace;

	Ray_t ray;
	ray.Init(start, end);

	return clip_ray_to_hitbox(ray, studio_hitbox, matrix[studio_hitbox->bone], trace) >= 0;
}