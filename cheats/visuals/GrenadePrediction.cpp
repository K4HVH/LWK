// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "GrenadePrediction.h"

void GrenadePrediction::Tick(int buttons)
{
	act = ACT_NONE;

	auto in_attack = buttons & IN_ATTACK;
	auto in_attack2 = buttons & IN_ATTACK2;

	if (in_attack || in_attack2)
	{
		if (in_attack && in_attack2)
			act = ACT_LOB;
		else if (!in_attack)
			act = ACT_DROP;
		else
			act = ACT_THROW;
	}
	else if (!g_cfg.esp.on_click)
		act = ACT_THROW;
}

void GrenadePrediction::View(CViewSetup* setup, weapon_t* weapon)
{
	if (g_ctx.local()->is_alive() && g_ctx.get_command())
	{
		if (!antiaim::get().freeze_check && act != ACT_NONE)
		{
			type = weapon->m_iItemDefinitionIndex();
			Simulate(setup);
		}
		else
			type = 0;
	}
}

inline float CSGO_Armor(float flDamage, int ArmorValue) {
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

void GrenadePrediction::Paint()
{
	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (path.size() < 2)
		return;

	CTraceFilter filter;
	filter.pSkip = g_ctx.local();

	std::pair <float, player_t*> target{ 0.f, nullptr };

	Vector prev = path[0];

	for (int i{ 1 }; i < m_globals()->m_maxclients; ++i) {
		player_t* player = (player_t*)m_entitylist()->GetClientEntity(i);
		if (!player->valid(true))
			continue;

		// get center of mass for player.
		auto origin = player->m_vecOrigin();
		auto collideable = player->GetCollideable();

		auto min = collideable->OBBMins() + origin;
		auto max = collideable->OBBMaxs() + origin;
	
		auto center = min + (max - min) * 0.5f;

		// get delta between center of mass and final nade pos.
		auto delta = center - path[path.size() - 1];

		if (weapon->m_iItemDefinitionIndex() == WEAPON_HEGRENADE) {

			// is within damage radius?
			if (delta.Length() > 350.f)
				continue;

			Ray_t ray;
			Vector NadeScreen;
			math::world_to_screen(path[path.size() - 1], NadeScreen);

			// main hitbox, that takes damage
			Vector vPelvis = player->hitbox_position(HITBOX_PELVIS);
			ray.Init(path[path.size() - 1], vPelvis);
			trace_t ptr;
			m_trace()->TraceRay(ray, MASK_SHOT, &filter, &ptr);
			//trace to it

			if (ptr.hit_entity == player) {
				Vector PelvisScreen;

				math::world_to_screen(vPelvis, PelvisScreen);

				// some magic values by VaLvO
				static float a = 105.0f;
				static float b = 25.0f;
				static float c = 140.0f;

				float d = ((delta.Length() - b) / c);
				float flDamage = a * exp(-d * d);

				// do main damage calculation here
				auto dmg = max(static_cast<int>(ceilf(CSGO_Armor(flDamage, player->m_ArmorValue()))), 0);

				// clip max damage.
				dmg = min(dmg, (player->m_ArmorValue() > 0) ? 57 : 98);
				// max damage proof - https://counterstrike.fandom.com/wiki/HE_Grenade 

				// if we have target with more damage
				if (dmg > target.first) {
					target.first = dmg;
					target.second = player;
				}
			}
		}
	}

	// we have a target for damage.
	if (target.second && weapon->m_iItemDefinitionIndex() == WEAPON_HEGRENADE && (!g_cfg.esp.on_click || g_ctx.get_command()->m_buttons & IN_ATTACK || g_ctx.get_command()->m_buttons & IN_ATTACK2)) {
		Vector screen;

		// replace the last bounce with green.
		if (!others.empty())
			others.back().second = Color::Green;

		if (math::world_to_screen(target.second->hitbox_position(HITBOX_HEAD), screen))
			render::get().text(fonts[ESP], screen.x - 4, screen.y - 28, (int)target.first >= target.second->m_iHealth() ? Color::Red : Color::White, HFONT_CENTERED_X | HFONT_CENTERED_Y, std::to_string((int)target.first).c_str());
	}

	if (type && path.size() > 1)
	{
		Vector nadeStart, nadeEnd;

		for (auto it = path.begin(); it != path.end(); ++it)
		{
			if (math::world_to_screen(prev, nadeStart) && math::world_to_screen(*it, nadeEnd))
				render::get().line((int)nadeStart.x, (int)nadeStart.y, (int)nadeEnd.x, (int)nadeEnd.y, g_cfg.esp.grenade_prediction_tracer_color);

			prev = *it;
		}

		for (auto it = others.begin(); it != others.end(); ++it)
		{
			Vector screen;

			if (math::world_to_screen(it->first, screen))
				render::get().rect(screen.x - 2, screen.y - 2, 4, 4, it->second == Color::Green ? Color::Green : g_cfg.esp.grenade_prediction_color);
		}

		if (math::world_to_screen(prev, nadeEnd))
			render::get().text(fonts[GRENADES], nadeEnd.x - 8, nadeEnd.y - 5, g_cfg.esp.grenade_prediction_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, weapon->get_icon());

		Vector endpos = path[path.size() - 1];

		if (weapon->m_iItemDefinitionIndex() == WEAPON_SMOKEGRENADE)
			render::get().Draw3DFilledCircle(endpos, 144, g_cfg.esp.grenade_prediction_tracer_color);
		else if (weapon->m_iItemDefinitionIndex() == WEAPON_MOLOTOV || weapon->m_iItemDefinitionIndex() == WEAPON_INCGRENADE)
			render::get().Draw3DFilledCircle(endpos, 150, g_cfg.esp.grenade_prediction_tracer_color);
	}
}

void GrenadePrediction::Setup(Vector& vecSrc, Vector& vecThrow, const Vector& viewangles)
{
	Vector angThrow = viewangles;
	float pitch = math::normalize_pitch(angThrow.x);

	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	angThrow.x = a;

	float flVel = 750.0f * 0.9f;
	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	b = b * 0.7f; b = b + 0.3f;
	flVel *= b;

	Vector vForward, vRight, vUp;
	math::angle_vectors(angThrow, &vForward, &vRight, &vUp);

	vecSrc = g_ctx.globals.eye_pos;
	float off = power[act] * 12.0f - 12.0f;
	vecSrc.z += off;

	trace_t tr;
	Vector vecDest = vecSrc;
	vecDest += vForward * 22.0f;

	TraceHull(vecSrc, vecDest, tr);

	Vector vecBack = vForward; vecBack *= 6.0f;
	vecSrc = tr.endpos;
	vecSrc -= vecBack;

	vecThrow = g_ctx.local()->m_vecVelocity(); vecThrow *= 1.25f;
	vecThrow += vForward * flVel;
}

void GrenadePrediction::Simulate(CViewSetup* setup)
{
	Vector vecSrc, vecThrow;
	Vector angles; m_engine()->GetViewAngles(angles);
	Setup(vecSrc, vecThrow, angles);

	float interval = m_globals()->m_intervalpertick;
	int logstep = (int)(0.05f / interval);
	int logtimer = 0;

	path.clear();
	others.clear();

	for (auto i = 0; i < 4096; ++i)
	{
		if (!logtimer)
			path.push_back(vecSrc);

		int s = Step(vecSrc, vecThrow, i, interval);

		if (s & 1)
			break;

		if (s & 2 || logtimer >= logstep)
			logtimer = 0;
		else
			++logtimer;

		if (vecThrow.IsZero())
			break;
	}

	path.push_back(vecSrc);
	others.emplace_back(std::make_pair(vecSrc, Color::Red));
}

void VectorAngles(const Vector& forward, QAngle& angles)
{
	if (forward[1] == 0.0f && forward[0] == 0.0f) //-V550
	{
		angles[0] = (forward[2] > 0.0f) ? 270.0f : 90.0f;
		angles[1] = 0.0f;
	}
	else
	{
		angles[0] = atan2(-forward[2], forward.Length2D()) * -180 / M_PI;
		angles[1] = atan2(forward[1], forward[0]) * 180 / M_PI;

		if (angles[1] > 90) angles[1] -= 180;
		else if (angles[1] < 90) angles[1] += 180;
		else if (angles[1] == 90) angles[1] = 0; //-V550
	}

	angles[2] = 0.0f;
}

int GrenadePrediction::Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval)
{
	Vector move; AddGravityMove(move, vecThrow, interval, false);
	trace_t tr; PushEntity(vecSrc, move, tr);

	int result = 0;

	if (CheckDetonate(vecThrow, tr, tick, interval))
		result |= 1;

	if (tr.fraction != 1.0f) //-V550
	{
		result |= 2;
		ResolveFlyCollisionCustom(tr, vecThrow, move, interval);

		QAngle angles;
		VectorAngles((tr.endpos - tr.startpos).Normalized(), angles);
		others.emplace_back(std::make_pair(tr.endpos, Color::White));
	}

	vecSrc = tr.endpos;
	return result;
}

bool GrenadePrediction::CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval)
{
	static auto molotov_throw_detonate_time = m_cvar()->FindVar("molotov_throw_detonate_time");
	static auto weapon_molotov_maxdetonateslope = m_cvar()->FindVar("weapon_molotov_maxdetonateslope");

	auto time = TICKS_TO_TIME(tick);

	switch (type)
	{
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		return time >= 1.5f && !(tick % TIME_TO_TICKS(0.2f));

	case WEAPON_SMOKEGRENADE:
		return vecThrow.Length() <= 0.1f && !(tick % TIME_TO_TICKS(0.2f));

	case WEAPON_DECOY:
		return vecThrow.Length() <= 0.2f && !(tick % TIME_TO_TICKS(0.2f));

	case WEAPON_MOLOTOV:
	case WEAPON_FIREBOMB:
		// detonate when hitting the floor.
		if (tr.fraction != 1.f && (std::cos(DEG2RAD(weapon_molotov_maxdetonateslope->GetFloat())) <= tr.plane.normal.z)) //-V550
			return true;

		// detonate if we have traveled for too long.
		// checked every 0.1s
		return time >= molotov_throw_detonate_time->GetFloat() && !(tick % TIME_TO_TICKS(0.1f));

	default:
		return false;
	}

	return false;
}

void GrenadePrediction::TraceHull(Vector& src, Vector& end, trace_t& tr)
{
	Ray_t ray;
	CTraceFilterWorldAndPropsOnly filter;

	ray.Init(src, end, Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f));
	m_trace()->TraceRay(ray, 0x200400B, &filter, &tr);
}

void GrenadePrediction::AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground)
{
	static auto sv_gravity = m_cvar()->FindVar(crypt_str("sv_gravity"));

	// gravity for grenades.
	float gravity = sv_gravity->GetFloat() * 0.4f;

	// move one tick using current velocity.
	move.x = vel.x * m_globals()->m_intervalpertick; //-V807
	move.y = vel.y * m_globals()->m_intervalpertick;

	// apply linear acceleration due to gravity.
	// calculate new z velocity.
	float z = vel.z - (gravity * m_globals()->m_intervalpertick);

	// apply velocity to move, the average of the new and the old.
	move.z = ((vel.z + z) / 2.f) * m_globals()->m_intervalpertick;

	// write back new gravity corrected z-velocity.
	vel.z = z;
}

void GrenadePrediction::PushEntity(Vector& src, const Vector& move, trace_t& tr)
{
	Vector vecAbsEnd = src;
	vecAbsEnd += move;
	TraceHull(src, vecAbsEnd, tr);
}

void GrenadePrediction::ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, const Vector& move, float interval)
{
	if (tr.hit_entity) 
	{
		if (autowall::get().is_breakable_entity(tr.hit_entity))
		{
			auto client_class = tr.hit_entity->GetClientClass();

			if (!client_class)
				return;

			auto network_name = client_class->m_pNetworkName;

			if (strcmp(network_name, "CFuncBrush") && strcmp(network_name, "CBaseDoor") && strcmp(network_name, "CCSPlayer") && strcmp(network_name, "CBaseEntity")) //-V526
			{
				// move object.
				PushEntity(tr.endpos, move, tr);

				// deduct velocity penalty.
				vecVelocity *= 0.4f;
				return;
			}
		}
	}

	float flSurfaceElasticity = 1.0, flGrenadeElasticity = 0.45f;
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity > 0.9f) flTotalElasticity = 0.9f;
	if (flTotalElasticity < 0.0f) flTotalElasticity = 0.0f;

	Vector vecAbsVelocity;
	PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;

	float flSpeedSqr = vecAbsVelocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f;

	if (flSpeedSqr < flMinSpeedSqr)
	{
		vecAbsVelocity.x = 0.0f;
		vecAbsVelocity.y = 0.0f;
		vecAbsVelocity.z = 0.0f;
	}

	if (tr.plane.normal.z > 0.7f)
	{
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity *= ((1.0f - tr.fraction) * interval);
		PushEntity(tr.endpos, vecAbsVelocity, tr);
	}
	else
		vecVelocity = vecAbsVelocity;
}

int GrenadePrediction::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	float backoff, change, angle;
	int   i, blocked;

	blocked = 0;
	angle = normal[2];

	if (angle > 0) blocked |= 1;
	if (!angle) blocked |= 2; //-V550

	backoff = in.Dot(normal) * overbounce;
	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}
	return blocked;
}