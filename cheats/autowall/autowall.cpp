// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "autowall.h"

bool autowall::is_breakable_entity(IClientEntity* e)
{
	if (!e || !e->EntIndex())
		return false;

	static auto is_breakable = util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 51 56 8B F1 85 F6 74 68"));

	auto take_damage = *(uintptr_t*)((uintptr_t)is_breakable + 0x26);
	auto backup = *(uint8_t*)((uintptr_t)e + take_damage);

	auto client_class = e->GetClientClass();
	auto network_name = client_class->m_pNetworkName;

	if (!strcmp(network_name, "CBreakableSurface"))
		*(uint8_t*)((uintptr_t)e + take_damage) = DAMAGE_YES;
	else if (!strcmp(network_name, "CBaseDoor") || !strcmp(network_name, "CDynamicProp"))
		*(uint8_t*)((uintptr_t)e + take_damage) = DAMAGE_NO;

	using Fn = bool(__thiscall*)(IClientEntity*);
	auto result = ((Fn)is_breakable)(e);

	*(uint8_t*)((uintptr_t)e + take_damage) = backup;
	return result;
}

void autowall::scale_damage(player_t* e, CGameTrace &enterTrace, weapon_info_t *weaponData, float& currentDamage)
{
	if (!e->is_player())
		return;

	auto is_armored = [&]()->bool
	{
		auto has_helmet = e->m_bHasHelmet();
		auto armor_value = e->m_ArmorValue();

		if (armor_value > 0)
		{
			switch (enterTrace.hitgroup) 
			{
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
			case HITGROUP_LEFTLEG:
			case HITGROUP_RIGHTLEG:
			case HITGROUP_GEAR:
				return true;
			case HITGROUP_HEAD:
				return has_helmet || e->m_bHasHeavyArmor();
			default:
				return e->m_bHasHeavyArmor();
			}
		}

		return false;
	};

	static auto mp_damage_scale_ct_head = m_cvar()->FindVar(crypt_str("mp_damage_scale_ct_head")); //-V807
	static auto mp_damage_scale_t_head = m_cvar()->FindVar(crypt_str("mp_damage_scale_t_head"));

	static auto mp_damage_scale_ct_body = m_cvar()->FindVar(crypt_str("mp_damage_scale_ct_body"));
	static auto mp_damage_scale_t_body = m_cvar()->FindVar(crypt_str("mp_damage_scale_t_body"));

	auto head_scale = e->m_iTeamNum() == 3 ? mp_damage_scale_ct_head->GetFloat() : mp_damage_scale_t_head->GetFloat();
	auto body_scale = e->m_iTeamNum() == 3 ? mp_damage_scale_ct_body->GetFloat() : mp_damage_scale_t_body->GetFloat();

	auto armor_heavy = e->m_bHasHeavyArmor();
	auto armor_value = (float)e->m_ArmorValue();

	if (armor_heavy)
		head_scale *= 0.5f;

	switch (enterTrace.hitgroup) 
	{
	case HITGROUP_HEAD:
		currentDamage *= 4.0f * head_scale;
		break;
	case HITGROUP_STOMACH:
		currentDamage *= 1.25f * body_scale;
		break;
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
	case HITGROUP_GEAR:
		currentDamage *= body_scale;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		currentDamage *= 0.75f * body_scale;
		break;
	}

	if (is_armored()) 
	{
		auto armor_scale = 1.0f;
		auto armor_ratio = weaponData->flArmorRatio * 0.5f;
		auto armor_bonus_ratio = 0.5f;

		if (armor_heavy)
		{
			armor_ratio *= 0.2f;
			armor_bonus_ratio = 0.33f;
			armor_scale = 0.25f;
		}

		auto new_damage = currentDamage * armor_ratio;
		auto estiminated_damage = (currentDamage - currentDamage * armor_ratio) * armor_scale * armor_bonus_ratio;

		if (estiminated_damage > armor_value)
			new_damage = currentDamage - armor_value / armor_bonus_ratio;

		currentDamage = new_damage;
	}
}

bool autowall::trace_to_exit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, const Vector& direction)
{
	auto enter_point_contents = 0;
	auto point_contents = 0;

	auto is_window = 0;
	auto flag = 0;

	auto fDistance = 0.0f;
	Vector start, end;

	do 
	{
		fDistance += 4.0f;

		end = startPosition + direction * fDistance;
		start = end - direction * 4.0f;

		if (!enter_point_contents) 
		{
			enter_point_contents = m_trace()->GetPointContents(end, 0x4600400B);
			point_contents = enter_point_contents;
		}
		else 
			point_contents = m_trace()->GetPointContents(end, 0x4600400B);

		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX) || enter_point_contents == point_contents))
			continue;

		static auto trace_filter_simple = util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F0 83 EC 7C 56 52")) + 0x3D;

		uint32_t filter_[4] = 
		{ 
			*(uint32_t*)(trace_filter_simple),
			(uint32_t)g_ctx.local(),
			0, 
			0 
		};

		util::trace_line(end, start, MASK_SHOT_HULL | CONTENTS_HITBOX, (CTraceFilter*)filter_, &exitTrace); //-V641

		if (exitTrace.startsolid && exitTrace.surface.flags & SURF_HITBOX) 
		{
			CTraceFilter filter;
			filter.pSkip = exitTrace.hit_entity;

			filter_[1] = (uint32_t)exitTrace.hit_entity;
			util::trace_line(end, startPosition, MASK_SHOT_HULL, (CTraceFilter*)filter_, &exitTrace); //-V641

			if (exitTrace.DidHit() && !exitTrace.startsolid)
				return true;

			continue;
		}

		auto name = (int*)enterTrace.surface.name; //-V206

		if (name)
		{
			if (*name == 1936744813 && name[1] == 1601397551 && name[2] == 1768318575 && name[3] == 1731159395 && name[4] == 1936941420 && name[5] == 1651668271 && name[6] == 1734307425 && name[7] == 1936941420)
				is_window = 1;
			else 
			{
				is_window = 0;

				if (*name != 1936744813)
					goto LABEL_34;
			}

			if (name[1] == 1600480303 && name[2] == 1701536108 && name[3] == 1634494255 && name[4] == 1731162995 && name[5] == 1936941420)
			{
				flag = 1;

			LABEL_35:
				if (is_window || flag)
				{
					exitTrace = enterTrace;
					exitTrace.endpos = end + direction;
					return true;
				}

				goto LABEL_37;
			}
		LABEL_34:
			flag = 0;
			goto LABEL_35;
		}

	LABEL_37:
		if (!exitTrace.DidHit() || exitTrace.startsolid)
		{
			if (enterTrace.hit_entity && enterTrace.hit_entity->EntIndex() && is_breakable_entity(enterTrace.hit_entity)) 
			{
				exitTrace = enterTrace;
				exitTrace.endpos = startPosition + direction;
				return true;
			}

			continue;
		}

		if (exitTrace.surface.flags & SURF_NODRAW)
		{
			if (is_breakable_entity(exitTrace.hit_entity) && is_breakable_entity(enterTrace.hit_entity))
				return true;

			if (!(enterTrace.surface.flags & SURF_NODRAW))
				continue;
		}

		if (exitTrace.plane.normal.Dot(direction) <= 1.0)
			return true;

	} 
	while (fDistance <= 90.0f);

	return false;
}

bool autowall::handle_bullet_penetration(weapon_info_t* weaponData, CGameTrace& enterTrace, Vector& eyePosition, const Vector& direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration, bool draw_impact)
{
	if (weaponData->flPenetration <= 0.0f)
		return false;

	if (possibleHitsRemaining <= 0)
		return false;

	auto contents_grate = enterTrace.contents & CONTENTS_GRATE;
	auto surf_nodraw = enterTrace.surface.flags & SURF_NODRAW;

	auto enterSurfaceData = m_physsurface()->GetSurfaceData(enterTrace.surface.surfaceProps);
	auto enter_material = enterSurfaceData->game.material;

	auto is_solid_surf = enterTrace.contents >> 3 & CONTENTS_SOLID;
	auto is_light_surf = enterTrace.surface.flags >> 7 & SURF_LIGHT;

	trace_t exit_trace;

	if (!trace_to_exit(enterTrace, exit_trace, enterTrace.endpos, direction) && !(m_trace()->GetPointContents(enterTrace.endpos, MASK_SHOT_HULL) & MASK_SHOT_HULL))
		return false;

	auto enter_penetration_modifier = enterSurfaceData->game.flPenetrationModifier;
	auto exit_surface_data = m_physsurface()->GetSurfaceData(exit_trace.surface.surfaceProps);

	if (!exit_surface_data)
		return false;

	auto exit_material = exit_surface_data->game.material;
	auto exit_penetration_modifier = exit_surface_data->game.flPenetrationModifier;

	auto combined_damage_modifier = 0.16f;
	auto combined_penetration_modifier = (enter_penetration_modifier + exit_penetration_modifier) * 0.5f;

	if (enter_material == CHAR_TEX_GLASS || enter_material == CHAR_TEX_GRATE)
	{
		combined_penetration_modifier = 3.0f;
		combined_damage_modifier = 0.05f;
	}
	else if (contents_grate || surf_nodraw)
		combined_penetration_modifier = 1.0f;
	else if (enter_material == CHAR_TEX_FLESH && ((player_t*)enterTrace.hit_entity)->m_iTeamNum() == g_ctx.local()->m_iTeamNum() && !ff_damage_reduction_bullets) 
	{
		if (!ff_damage_bullet_penetration) //-V550
			return false;

		combined_penetration_modifier = ff_damage_bullet_penetration;
		combined_damage_modifier = 0.16f;
	}

	if (enter_material == exit_material) 
	{
		if (exit_material == CHAR_TEX_WOOD || exit_material == CHAR_TEX_CARDBOARD)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == CHAR_TEX_PLASTIC)
			combined_penetration_modifier = 2.0f;
	}

	auto penetration_modifier = std::fmaxf(0.0f, 1.0f / combined_penetration_modifier);
	auto penetration_distance = (exit_trace.endpos - enterTrace.endpos).Length();

	penetration_distance = penetration_distance * penetration_distance * penetration_modifier * 0.041666668f;

	auto damage_modifier = max(0.0f, 3.0f / weaponData->flPenetration * 1.25f) * penetration_modifier * 3.0f + currentDamage * combined_damage_modifier + penetration_distance;
	auto damage_lost = max(0.0f, damage_modifier);

	if (damage_lost > currentDamage)
		return false;

	currentDamage -= damage_lost;

	if (currentDamage < 1.0f)
		return false;

	eyePosition = exit_trace.endpos;
	--possibleHitsRemaining;

	return true;
}

bool autowall::fire_bullet(weapon_t* pWeapon, Vector& direction, bool& visible, float& currentDamage, int& hitbox, IClientEntity* e, float length, const Vector& pos)
{
	if (!pWeapon)
		return false;

	auto weaponData = pWeapon->get_csweapon_info();

	if (!weaponData)
		return false;

	CGameTrace enterTrace;
	CTraceFilter filter;

	filter.pSkip = g_ctx.local();
	currentDamage = weaponData->iDamage;

	auto eyePosition = pos;
	auto currentDistance = 0.0f;
	auto maxRange = weaponData->flRange;
	auto penetrationDistance = 3000.0f;
	auto penetrationPower = weaponData->flPenetration;
	auto possibleHitsRemaining = 4;

	while (currentDamage >= 1.0f)
	{
		maxRange -= currentDistance;
		auto end = eyePosition + direction * maxRange;

		CTraceFilter filter;
		filter.pSkip = g_ctx.local();

		util::trace_line(eyePosition, end, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enterTrace);
		util::clip_trace_to_players(e, eyePosition, end + direction * 40.0f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enterTrace);

		auto enterSurfaceData = m_physsurface()->GetSurfaceData(enterTrace.surface.surfaceProps);
		auto enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
		auto enterMaterial = enterSurfaceData->game.material;

		if (enterTrace.fraction == 1.0f)  //-V550
			break;

		currentDistance += enterTrace.fraction * maxRange;
		currentDamage *= pow(weaponData->flRangeModifier, currentDistance / 500.0f);

		if (currentDistance > penetrationDistance && weaponData->flPenetration || enterSurfPenetrationModifier < 0.1f)  //-V1051
			break;

		auto canDoDamage = enterTrace.hitgroup != HITGROUP_GEAR && enterTrace.hitgroup != HITGROUP_GENERIC;
		auto isPlayer = ((player_t*)enterTrace.hit_entity)->is_player();
		auto isEnemy = ((player_t*)enterTrace.hit_entity)->m_iTeamNum() != g_ctx.local()->m_iTeamNum();

		if (canDoDamage && isPlayer && isEnemy)
		{
			scale_damage((player_t*)enterTrace.hit_entity, enterTrace, weaponData, currentDamage);
			hitbox = enterTrace.hitbox;
			return true;
		}

		if (!possibleHitsRemaining)
			break;

		static auto damageReductionBullets = m_cvar()->FindVar(crypt_str("ff_damage_reduction_bullets"));
		static auto damageBulletPenetration = m_cvar()->FindVar(crypt_str("ff_damage_bullet_penetration"));

		if (!handle_bullet_penetration(weaponData, enterTrace, eyePosition, direction, possibleHitsRemaining, currentDamage, penetrationPower, damageReductionBullets->GetFloat(), damageBulletPenetration->GetFloat(), !e))
			break;

		visible = false;
	}

	return false;
}

autowall::returninfo_t autowall::wall_penetration(const Vector& eye_pos, Vector& point, IClientEntity* e)
{
	g_ctx.globals.autowalling = true;
	auto tmp = point - eye_pos;

	auto angles = ZERO;
	math::vector_angles(tmp, angles);

	auto direction = ZERO;
	math::angle_vectors(angles, direction);

	direction.NormalizeInPlace();

	auto visible = true;
	auto damage = -1.0f;
	auto hitbox = -1;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (fire_bullet(weapon, direction, visible, damage, hitbox, e, 0.0f, eye_pos))
	{
		g_ctx.globals.autowalling = false;
		return returninfo_t(visible, (int)damage, hitbox); //-V2003
	}
	else
	{
		g_ctx.globals.autowalling = false;
		return returninfo_t(false, -1, -1);
	}
}