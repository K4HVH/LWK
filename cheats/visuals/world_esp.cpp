// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "world_esp.h"

void worldesp::paint_traverse()
{
	skybox_changer();

	for (int i = 1; i <= m_entitylist()->GetHighestEntityIndex(); i++)  //-V807
	{
		auto e = static_cast<entity_t*>(m_entitylist()->GetClientEntity(i));

		if (!e)
			continue;

		if (e->is_player())
			continue;

		if (e->IsDormant())
			continue;

		auto client_class = e->GetClientClass();

		if (!client_class)
			continue;

		switch (client_class->m_ClassID)
		{
		case CEnvTonemapController:
			world_modulation(e);
			break;
		case CInferno:
			molotov_timer(e);
			break;
		case CSmokeGrenadeProjectile:
			smoke_timer(e);
			break;
		case CPlantedC4:
			bomb_timer(e);
			break;
		case CC4:
			if (g_cfg.player.type[ENEMY].flags[FLAGS_C4] || g_cfg.player.type[TEAM].flags[FLAGS_C4] || g_cfg.player.type[LOCAL].flags[FLAGS_C4] || g_cfg.esp.bomb_timer)
			{
				auto owner = (player_t*)m_entitylist()->GetClientEntityFromHandle(e->m_hOwnerEntity());

				if ((g_cfg.player.type[ENEMY].flags[FLAGS_C4] || g_cfg.player.type[TEAM].flags[FLAGS_C4] || g_cfg.player.type[LOCAL].flags[FLAGS_C4]) && owner->valid(false, false))
					g_ctx.globals.bomb_carrier = owner->EntIndex();
				else if (g_cfg.esp.bomb_timer && !owner->is_player())
				{
					auto screen = ZERO;

					if (math::world_to_screen(e->GetAbsOrigin(), screen))
						render::get().text(fonts[ESP], screen.x, screen.y, Color(215, 20, 20), HFONT_CENTERED_X | HFONT_CENTERED_Y, "BOMB");
				}
			}

			break;
		default:
			grenade_projectiles(e);

			if (client_class->m_ClassID == CAK47 || client_class->m_ClassID == CDEagle || client_class->m_ClassID >= CWeaponAug && client_class->m_ClassID <= CWeaponZoneRepulsor) //-V648
				dropped_weapons(e);

			break;
		}
	}
}

void worldesp::skybox_changer()
{
	static auto load_skybox = reinterpret_cast<void(__fastcall*)(const char*)>(util::FindSignature(crypt_str("engine.dll"), crypt_str("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45")));
	auto skybox_name = backup_skybox;

	switch (g_cfg.esp.skybox)
	{
	case 1:
		skybox_name = "cs_tibet";
		break;
	case 2:
		skybox_name = "cs_baggage_skybox_";
		break;
	case 3:
		skybox_name = "italy";
		break;
	case 4:
		skybox_name = "jungle";
		break;
	case 5:
		skybox_name = "office";
		break;
	case 6:
		skybox_name = "sky_cs15_daylight01_hdr";
		break;
	case 7:
		skybox_name = "sky_cs15_daylight02_hdr";
		break;
	case 8:
		skybox_name = "vertigoblue_hdr";
		break;
	case 9:
		skybox_name = "vertigo";
		break;
	case 10:
		skybox_name = "sky_day02_05_hdr";
		break;
	case 11:
		skybox_name = "nukeblank";
		break;
	case 12:
		skybox_name = "sky_venice";
		break;
	case 13:
		skybox_name = "sky_cs15_daylight03_hdr";
		break;
	case 14:
		skybox_name = "sky_cs15_daylight04_hdr";
		break;
	case 15:
		skybox_name = "sky_csgo_cloudy01";
		break;
	case 16:
		skybox_name = "sky_csgo_night02";
		break;
	case 17:
		skybox_name = "sky_csgo_night02b";
		break;
	case 18:
		skybox_name = "sky_csgo_night_flat";
		break;
	case 19:
		skybox_name = "sky_dust";
		break;
	case 20:
		skybox_name = "vietnam";
		break;
	case 21:
		skybox_name = g_cfg.esp.custom_skybox;
		break;
	}

	static auto skybox_number = 0;
	static auto old_skybox_name = skybox_name;

	static auto color_r = (unsigned char)255;
	static auto color_g = (unsigned char)255;
	static auto color_b = (unsigned char)255;

	if (skybox_number != g_cfg.esp.skybox)
	{
		changed = true;
		skybox_number = g_cfg.esp.skybox;
	}
	else if (old_skybox_name != skybox_name)
	{
		changed = true;
		old_skybox_name = skybox_name;
	}
	else if (color_r != g_cfg.esp.skybox_color[0])
	{
		changed = true;
		color_r = g_cfg.esp.skybox_color[0];
	}
	else if (color_g != g_cfg.esp.skybox_color[1])
	{
		changed = true;
		color_g = g_cfg.esp.skybox_color[1];
	}
	else if (color_b != g_cfg.esp.skybox_color[2])
	{
		changed = true;
		color_b = g_cfg.esp.skybox_color[2];
	}

	if (changed)
	{
		changed = false;
		load_skybox(skybox_name.c_str());

		auto materialsystem = m_materialsystem();

		for (auto i = materialsystem->FirstMaterial(); i != materialsystem->InvalidMaterial(); i = materialsystem->NextMaterial(i))
		{
			auto material = materialsystem->GetMaterial(i);

			if (!material)
				continue;

			if (strstr(material->GetTextureGroupName(), crypt_str("SkyBox")))
				material->ColorModulate(g_cfg.esp.skybox_color[0] / 255.0f, g_cfg.esp.skybox_color[1] / 255.0f, g_cfg.esp.skybox_color[2] / 255.0f);
		}
	}
}

void worldesp::fog_changer()
{
	static auto fog_override = m_cvar()->FindVar(crypt_str("fog_override")); //-V807

	if (!g_cfg.esp.fog)
	{
		if (fog_override->GetBool())
			fog_override->SetValue(FALSE);

		return;
	}

	if (!fog_override->GetBool())
		fog_override->SetValue(TRUE);

	static auto fog_start = m_cvar()->FindVar(crypt_str("fog_start"));

	if (fog_start->GetInt())
		fog_start->SetValue(0);

	static auto fog_end = m_cvar()->FindVar(crypt_str("fog_end"));

	if (fog_end->GetInt() != g_cfg.esp.fog_distance)
		fog_end->SetValue(g_cfg.esp.fog_distance);

	static auto fog_maxdensity = m_cvar()->FindVar(crypt_str("fog_maxdensity"));

	if (fog_maxdensity->GetFloat() != (float)g_cfg.esp.fog_density * 0.01f) //-V550
		fog_maxdensity->SetValue((float)g_cfg.esp.fog_density * 0.01f);

	char buffer_color[12];
	sprintf_s(buffer_color, 12, "%i %i %i", g_cfg.esp.fog_color.r(), g_cfg.esp.fog_color.g(), g_cfg.esp.fog_color.b());

	static auto fog_color = m_cvar()->FindVar(crypt_str("fog_color"));

	if (strcmp(fog_color->GetString(), buffer_color)) //-V526
		fog_color->SetValue(buffer_color);
}

void worldesp::world_modulation(entity_t* entity)
{
	if (!g_cfg.esp.world_modulation)
		return;

	entity->set_m_bUseCustomBloomScale(TRUE);
	entity->set_m_flCustomBloomScale(g_cfg.esp.bloom * 0.01f);

	entity->set_m_bUseCustomAutoExposureMin(TRUE);
	entity->set_m_flCustomAutoExposureMin(g_cfg.esp.exposure * 0.001f);

	entity->set_m_bUseCustomAutoExposureMax(TRUE);
	entity->set_m_flCustomAutoExposureMax(g_cfg.esp.exposure * 0.001f);
}

void worldesp::molotov_timer(entity_t* entity)
{
	if (!g_cfg.esp.molotov_timer)
		return;

	auto inferno = reinterpret_cast<inferno_t*>(entity);
	auto origin = inferno->GetAbsOrigin();

	Vector screen_origin;

	if (!math::world_to_screen(origin, screen_origin))
		return;

	auto spawn_time = inferno->get_spawn_time();
	auto factor = (spawn_time + inferno_t::get_expiry_time() - m_globals()->m_curtime) / inferno_t::get_expiry_time();

	static auto size = Vector2D(35.0f, 5.0f);
	render::get().circle_filled(screen_origin.x, screen_origin.y - size.y * 0.5f, 60, 20, Color(15, 15, 15, 187));

	render::get().rect_filled(screen_origin.x - size.x * 0.5f, screen_origin.y - size.y * 0.5f - 1.0f, size.x, size.y, Color(37, 37, 37, g_cfg.esp.molotov_timer_color.a()));
	render::get().rect_filled(screen_origin.x - size.x * 0.5f + 2.0f, screen_origin.y - size.y * 0.5f, (size.x - 4.0f) * factor, size.y - 2.0f, g_cfg.esp.molotov_timer_color);

	render::get().rect(screen_origin.x - size.x * 0.5f, screen_origin.y - size.y * 0.5f, size.x, size.y, Color(7, 7, 7, g_cfg.esp.molotov_timer_color.a()));
	render::get().text(fonts[ESP], screen_origin.x, screen_origin.y - size.y * 0.5f + 12.0f, g_cfg.esp.molotov_timer_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "FIRE");
	render::get().text(fonts[GRENADES], screen_origin.x + 1.0f, screen_origin.y - size.y * 0.5f - 9.0f, g_cfg.esp.molotov_timer_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "l");
}

void worldesp::smoke_timer(entity_t* entity)
{
	if (!g_cfg.esp.smoke_timer)
		return;

	auto smoke = reinterpret_cast<smoke_t*>(entity);

	if (!smoke->m_nSmokeEffectTickBegin() || !smoke->m_bDidSmokeEffect())
		return;

	auto origin = smoke->GetAbsOrigin();

	Vector screen_origin;

	if (!math::world_to_screen(origin, screen_origin))
		return;

	auto spawn_time = TICKS_TO_TIME(smoke->m_nSmokeEffectTickBegin());
	auto factor = (spawn_time + smoke_t::get_expiry_time() - m_globals()->m_curtime) / smoke_t::get_expiry_time();

	static auto size = Vector2D(35.0f, 5.0f);
	render::get().circle_filled(screen_origin.x, screen_origin.y - size.y * 0.5f, 60, 20, Color(15, 15, 15, 187));

	render::get().rect_filled(screen_origin.x - size.x * 0.5f, screen_origin.y - size.y * 0.5f - 1.0f, size.x, size.y, Color(37, 37, 37, g_cfg.esp.smoke_timer_color.a()));
	render::get().rect_filled(screen_origin.x - size.x * 0.5f + 2.0f, screen_origin.y - size.y * 0.5f, (size.x - 4.0f) * factor, size.y - 2.0f, g_cfg.esp.smoke_timer_color);

	render::get().rect(screen_origin.x - size.x * 0.5f, screen_origin.y - size.y * 0.5f, size.x, size.y, Color(7, 7, 7, g_cfg.esp.smoke_timer_color.a()));
	render::get().text(fonts[ESP], screen_origin.x, screen_origin.y - size.y * 0.5f + 12.0f, g_cfg.esp.smoke_timer_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "SMOKE");
	render::get().text(fonts[GRENADES], screen_origin.x + 1.0f, screen_origin.y - size.y * 0.5f - 9.0f, g_cfg.esp.smoke_timer_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "k");
}

void worldesp::grenade_projectiles(entity_t* entity)
{
	if (!g_cfg.esp.projectiles)
		return;

	auto client_class = entity->GetClientClass();

	if (!client_class)
		return;

	auto model = entity->GetModel();

	if (!model)
		return;

	auto studio_model = m_modelinfo()->GetStudioModel(model);

	if (!studio_model)
		return;

	auto name = (std::string)studio_model->szName;

	if (name.find("thrown") != std::string::npos ||
		client_class->m_ClassID == CBaseCSGrenadeProjectile || client_class->m_ClassID == CDecoyProjectile || client_class->m_ClassID == CMolotovProjectile)
	{
		auto grenade_origin = entity->GetAbsOrigin();
		auto grenade_position = ZERO;

		if (!math::world_to_screen(grenade_origin, grenade_position))
			return;

		std::string grenade_name, grenade_icon;

		if (name.find("flashbang") != std::string::npos)
		{
			grenade_name = "FLASHBANG";
			grenade_icon = "i";
		}
		else if (name.find("smokegrenade") != std::string::npos)
		{
			grenade_name = "SMOKE";
			grenade_icon = "k";
		}
		else if (name.find("incendiarygrenade") != std::string::npos)
		{
			grenade_name = "INCENDIARY";
			grenade_icon = "n";
		}
		else if (name.find("molotov") != std::string::npos)
		{
			grenade_name = "MOLOTOV";
			grenade_icon = "l";
		}
		else if (name.find("fraggrenade") != std::string::npos)
		{
			grenade_name = "HE GRENADE";
			grenade_icon = "j";
		}
		else if (name.find("decoy") != std::string::npos)
		{
			grenade_name = "DECOY";
			grenade_icon = "m";
		}
		else
			return;

		Box box;

		if (util::get_bbox(entity, box, false))
		{
			if (g_cfg.esp.grenade_esp[GRENADE_BOX])
			{
				render::get().rect(box.x, box.y, box.w, box.h, g_cfg.esp.grenade_box_color);

				if (g_cfg.esp.grenade_esp[GRENADE_ICON])
					render::get().text(fonts[GRENADES], box.x + box.w / 2, box.y - 21, g_cfg.esp.projectiles_color, HFONT_CENTERED_X, grenade_icon.c_str());

				if (g_cfg.esp.grenade_esp[GRENADE_TEXT])
					render::get().text(fonts[ESP], box.x + box.w / 2, box.y + box.h + 2, g_cfg.esp.projectiles_color, HFONT_CENTERED_X, grenade_name.c_str());
			}
			else
			{
				if (g_cfg.esp.grenade_esp[GRENADE_ICON] && g_cfg.esp.grenade_esp[GRENADE_TEXT])
				{
					render::get().text(fonts[GRENADES], box.x + box.w / 2, box.y + box.h / 2 - 10, g_cfg.esp.projectiles_color, HFONT_CENTERED_X, grenade_icon.c_str());
					render::get().text(fonts[ESP], box.x + box.w / 2, box.y + box.h / 2 + 7, g_cfg.esp.projectiles_color, HFONT_CENTERED_X, grenade_name.c_str());
				}
				else
				{
					if (g_cfg.esp.grenade_esp[GRENADE_ICON])
						render::get().text(fonts[GRENADES], box.x + box.w / 2, box.y + box.h / 2, g_cfg.esp.projectiles_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, grenade_icon.c_str());

					if (g_cfg.esp.grenade_esp[GRENADE_TEXT])
						render::get().text(fonts[ESP], box.x + box.w / 2, box.y + box.h / 2, g_cfg.esp.projectiles_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, grenade_name.c_str());
				}
			}
		}
	}
	else
	{
		auto model = entity->GetModel();

		if (!model)
			return;

		auto studio_model = m_modelinfo()->GetStudioModel(model);

		if (!studio_model)
			return;

		auto name = (std::string)studio_model->szName;

		if (name.find("dropped") != std::string::npos)
		{
			auto weapon = (weapon_t*)entity; //-V1027
			Box box;

			if (util::get_bbox(weapon, box, false))
			{
				auto offset = 0;

				if (g_cfg.esp.weapon[WEAPON_BOX])
				{
					render::get().rect(box.x, box.y, box.w, box.h, g_cfg.esp.box_color);

					if (g_cfg.esp.weapon[WEAPON_ICON])
					{
						render::get().text(fonts[SUBTABWEAPONS], box.x + box.w / 2, box.y - 14, g_cfg.esp.weapon_color, HFONT_CENTERED_X, weapon->get_icon());
						offset = 14;
					}

					if (g_cfg.esp.weapon[WEAPON_TEXT])
						render::get().text(fonts[ESP], box.x + box.w / 2, box.y + box.h + 2, g_cfg.esp.weapon_color, HFONT_CENTERED_X, weapon->get_name().c_str());

					if (g_cfg.esp.weapon[WEAPON_DISTANCE])
					{
						auto distance = g_ctx.local()->GetAbsOrigin().DistTo(weapon->GetAbsOrigin()) / 12.0f;
						render::get().text(fonts[ESP], box.x + box.w / 2, box.y - 13 - offset, g_cfg.esp.weapon_color, HFONT_CENTERED_X, "%i FT", (int)distance);
					}
				}
				else
				{
					if (g_cfg.esp.weapon[WEAPON_ICON])
						render::get().text(fonts[SUBTABWEAPONS], box.x + box.w / 2, box.y + box.h / 2 - 7, g_cfg.esp.weapon_color, HFONT_CENTERED_X, weapon->get_icon());

					if (g_cfg.esp.weapon[WEAPON_TEXT])
						render::get().text(fonts[ESP], box.x + box.w / 2, box.y + box.h / 2 + 6, g_cfg.esp.weapon_color, HFONT_CENTERED_X, weapon->get_name().c_str());

					if (g_cfg.esp.weapon[WEAPON_DISTANCE])
					{
						auto distance = g_ctx.local()->GetAbsOrigin().DistTo(weapon->GetAbsOrigin()) / 12.0f;

						if (g_cfg.esp.weapon[WEAPON_ICON] && g_cfg.esp.weapon[WEAPON_TEXT])
							offset = 21;
						else if (g_cfg.esp.weapon[WEAPON_ICON])
							offset = 21;
						else if (g_cfg.esp.weapon[WEAPON_TEXT])
							offset = 8;

						render::get().text(fonts[ESP], box.x + box.w / 2, box.y + box.h / 2 - offset, g_cfg.esp.weapon_color, HFONT_CENTERED_X, "%i FT", (int)distance);
					}
				}
			}
		}
	}
}

void worldesp::bomb_timer(entity_t* entity)
{
	if (!g_cfg.esp.bomb_timer)
		return;

	if (!g_ctx.globals.bomb_timer_enable)
		return;

	static auto mp_c4timer = m_cvar()->FindVar(crypt_str("mp_c4timer"));
	auto bomb = (CCSBomb*)entity;

	auto c4timer = mp_c4timer->GetFloat();
	auto bomb_timer = bomb->m_flC4Blow() - m_globals()->m_curtime;

	if (bomb_timer < 0.0f)
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto factor = bomb_timer / c4timer * height;

	auto red_factor = (int)(255.0f - bomb_timer / c4timer * 255.0f);
	auto green_factor = (int)(bomb_timer / c4timer * 255.0f);

	render::get().rect_filled(0, height - factor, 26, factor, Color(red_factor, green_factor, 0, 100));

	auto text_position = height - factor + 11;

	if (text_position > height - 9)
		text_position = height - 9;

	render::get().text(fonts[ESP], 13, text_position, Color::White, HFONT_CENTERED_X | HFONT_CENTERED_Y, "%0.1f", bomb_timer);

	Vector screen;

	if (math::world_to_screen(entity->GetAbsOrigin(), screen))
		render::get().text(fonts[ESP], screen.x, screen.y, Color(red_factor, green_factor, 0), HFONT_CENTERED_X | HFONT_CENTERED_Y, "BOMB");
}

void worldesp::dropped_weapons(entity_t* entity)
{
	auto weapon = (weapon_t*)entity; //-V1027
	auto owner = (player_t*)m_entitylist()->GetClientEntityFromHandle(weapon->m_hOwnerEntity());

	if (owner->is_player())
		return;

	Box box;

	if (util::get_bbox(weapon, box, false))
	{
		auto offset = 0;

		if (g_cfg.esp.weapon[WEAPON_BOX])
		{
			render::get().rect(box.x, box.y, box.w, box.h, g_cfg.esp.box_color);

			if (g_cfg.esp.weapon[WEAPON_ICON])
			{
				render::get().text(fonts[SUBTABWEAPONS], box.x + box.w / 2, box.y - 14, g_cfg.esp.weapon_color, HFONT_CENTERED_X, weapon->get_icon());
				offset = 14;
			}

			if (g_cfg.esp.weapon[WEAPON_TEXT])
				render::get().text(fonts[ESP], box.x + box.w / 2, box.y + box.h + 2, g_cfg.esp.weapon_color, HFONT_CENTERED_X, weapon->get_name().c_str());

			if (g_cfg.esp.weapon[WEAPON_AMMO] && entity->GetClientClass()->m_ClassID != CBaseCSGrenadeProjectile && entity->GetClientClass()->m_ClassID != CSmokeGrenadeProjectile && entity->GetClientClass()->m_ClassID != CSensorGrenadeProjectile && entity->GetClientClass()->m_ClassID != CMolotovProjectile && entity->GetClientClass()->m_ClassID != CDecoyProjectile)
			{
				auto inner_back_color = Color::Black;
				inner_back_color.SetAlpha(153);

				render::get().rect_filled(box.x - 1, box.y + box.h + 14, box.w + 2, 4, inner_back_color);
				render::get().rect_filled(box.x, box.y + box.h + 15, weapon->m_iClip1() * box.w / weapon->get_csweapon_info()->iMaxClip1, 2, g_cfg.esp.weapon_ammo_color);
			}

			if (g_cfg.esp.weapon[WEAPON_DISTANCE])
			{
				auto distance = g_ctx.local()->GetAbsOrigin().DistTo(weapon->GetAbsOrigin()) / 12.0f;
				render::get().text(fonts[ESP], box.x + box.w / 2, box.y - 13 - offset, g_cfg.esp.weapon_color, HFONT_CENTERED_X, "%i FT", (int)distance);
			}
		}
		else
		{
			if (g_cfg.esp.weapon[WEAPON_ICON])
				render::get().text(fonts[SUBTABWEAPONS], box.x + box.w / 2, box.y + box.h / 2 - 7, g_cfg.esp.weapon_color, HFONT_CENTERED_X, weapon->get_icon());

			if (g_cfg.esp.weapon[WEAPON_TEXT])
				render::get().text(fonts[ESP], box.x + box.w / 2, box.y + box.h / 2 + 6, g_cfg.esp.weapon_color, HFONT_CENTERED_X, weapon->get_name().c_str());

			if (g_cfg.esp.weapon[WEAPON_AMMO] && entity->GetClientClass()->m_ClassID != CBaseCSGrenadeProjectile && entity->GetClientClass()->m_ClassID != CSmokeGrenadeProjectile && entity->GetClientClass()->m_ClassID != CSensorGrenadeProjectile && entity->GetClientClass()->m_ClassID != CMolotovProjectile && entity->GetClientClass()->m_ClassID != CDecoyProjectile)
			{
				static auto pos = 0;

				if (g_cfg.esp.weapon[WEAPON_ICON] && g_cfg.esp.weapon[WEAPON_TEXT])
					pos = 19;
				else if (g_cfg.esp.weapon[WEAPON_ICON])
					pos = 8;
				else if (g_cfg.esp.weapon[WEAPON_TEXT])
					pos = 19;

				auto inner_back_color = Color::Black;
				inner_back_color.SetAlpha(153);

				render::get().rect_filled(box.x - 1, box.y + box.h / 2 + pos - 1, box.w + 2, 4, inner_back_color);
				render::get().rect_filled(box.x, box.y + box.h / 2 + pos, weapon->m_iClip1() * box.w / weapon->get_csweapon_info()->iMaxClip1, 2, g_cfg.esp.weapon_ammo_color);
			}

			if (g_cfg.esp.weapon[WEAPON_DISTANCE])
			{
				auto distance = g_ctx.local()->GetAbsOrigin().DistTo(weapon->GetAbsOrigin()) / 12.0f;

				if (g_cfg.esp.weapon[WEAPON_ICON] && g_cfg.esp.weapon[WEAPON_TEXT])
					offset = 21;
				else if (g_cfg.esp.weapon[WEAPON_ICON])
					offset = 21;
				else if (g_cfg.esp.weapon[WEAPON_TEXT])
					offset = 8;

				render::get().text(fonts[ESP], box.x + box.w / 2, box.y + box.h / 2 - offset, g_cfg.esp.weapon_color, HFONT_CENTERED_X, "%i FT", (int)distance);
			}
		}
	}
}