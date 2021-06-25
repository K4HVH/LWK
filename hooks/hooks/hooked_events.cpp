// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\ragebot\aim.h"
#include "..\..\cheats\misc\logs.h"
#include "..\..\cheats\visuals\other_esp.h"
#include "..\..\cheats\visuals\bullet_tracers.h"
#include "..\..\cheats\visuals\player_esp.h"
#include "..\..\cheats\ragebot\antiaim.h"
#include "..\..\cheats\visuals\nightmode.h"
#include "..\..\cheats\misc\misc.h"
#include "..\..\cheats\visuals\dormant_esp.h"
#include "..\..\cheats\lagcompensation\animation_system.h"

bool weapon_is_aim(const std::string& weapon)
{
	return weapon.find(crypt_str("decoy")) == std::string::npos && weapon.find(crypt_str("flashbang")) == std::string::npos &&
		weapon.find(crypt_str("hegrenade")) == std::string::npos && weapon.find(crypt_str("inferno")) == std::string::npos &&
		weapon.find(crypt_str("molotov")) == std::string::npos && weapon.find(crypt_str("smokegrenade")) == std::string::npos;
}

void C_HookedEvents::FireGameEvent(IGameEvent* event)
{
	auto event_name = event->GetName();

	if (g_ctx.globals.loaded_script)
	{
		for (auto& script : c_lua::get().scripts)
		{
			auto script_id = c_lua::get().get_script_id(script);

			if (c_lua::get().events.find(script_id) == c_lua::get().events.end())
				continue;

			if (c_lua::get().events[script_id].find(event_name) == c_lua::get().events[script_id].end())
				continue;

			c_lua::get().events[script_id][event_name](event);
		}
	}

	if (!strcmp(event->GetName(), crypt_str("player_footstep")))
	{
		auto userid = m_engine()->GetPlayerForUserID(event->GetInt(crypt_str("userid")));
		auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(userid));

		if (e->valid(false, false))
		{
			auto type = ENEMY;

			if (e == g_ctx.local())
				type = LOCAL;
			else if (e->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
				type = TEAM;

			if (g_cfg.player.type[type].footsteps)
			{
				static auto model_index = m_modelinfo()->GetModelIndex(crypt_str("sprites/physbeam.vmt"));

				if (g_ctx.globals.should_update_beam_index)
					model_index = m_modelinfo()->GetModelIndex(crypt_str("sprites/physbeam.vmt"));

				BeamInfo_t info;

				info.m_nType = TE_BEAMRINGPOINT;
				info.m_pszModelName = crypt_str("sprites/physbeam.vmt");
				info.m_nModelIndex = model_index;
				info.m_nHaloIndex = -1;
				info.m_flHaloScale = 3.0f;
				info.m_flLife = 2.0f;
				info.m_flWidth = (float)g_cfg.player.type[type].thickness;
				info.m_flFadeLength = 1.0f;
				info.m_flAmplitude = 0.0f;
				info.m_flRed = (float)g_cfg.player.type[type].footsteps_color.r();
				info.m_flGreen = (float)g_cfg.player.type[type].footsteps_color.g();
				info.m_flBlue = (float)g_cfg.player.type[type].footsteps_color.b();
				info.m_flBrightness = (float)g_cfg.player.type[type].footsteps_color.a();
				info.m_flSpeed = 0.0f;
				info.m_nStartFrame = 0.0f;
				info.m_flFrameRate = 60.0f;
				info.m_nSegments = -1;
				info.m_nFlags = FBEAM_FADEOUT;
				info.m_vecCenter = e->GetAbsOrigin() + Vector(0.0f, 0.0f, 5.0f);
				info.m_flStartRadius = 5.0f;
				info.m_flEndRadius = (float)g_cfg.player.type[type].radius;
				info.m_bRenderable = true;

				auto beam_draw = m_viewrenderbeams()->CreateBeamRingPoint(info);

				if (beam_draw)
					m_viewrenderbeams()->DrawBeam(beam_draw);
			}
		}

	}
	else if (!strcmp(event_name, crypt_str("weapon_fire")))
	{
		auto user_id = event->GetInt(crypt_str("userid"));
		auto user = m_engine()->GetPlayerForUserID(user_id);

		if (user == m_engine()->GetLocalPlayer())
		{
			aim_shot* current_shot = nullptr;

			for (auto& shot : g_ctx.shots)
			{
				if (shot.start)
				{
					shot.end = true;
					continue;
				}

				if (shot.end)
					continue;

				current_shot = &shot;
				break;
			}

			if (current_shot)
			{
				current_shot->start = true;
				current_shot->event_fire_tick = m_globals()->m_tickcount;

				++g_ctx.globals.fired_shots[aim::get().last_target[current_shot->last_target].record.i];
			}
		}
	}
	else if (!strcmp(event_name, crypt_str("bullet_impact")))
	{
		auto user_id = event->GetInt(crypt_str("userid"));
		auto user = m_engine()->GetPlayerForUserID(user_id); //-V807

		if (user == m_engine()->GetLocalPlayer())
		{
			Vector position(event->GetFloat(crypt_str("x")), event->GetFloat(crypt_str("y")), event->GetFloat(crypt_str("z")));

			if (g_cfg.esp.server_bullet_impacts)
				m_debugoverlay()->BoxOverlay(position, Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f), QAngle(0.0f, 0.0f, 0.0f), g_cfg.esp.server_bullet_impacts_color.r(), g_cfg.esp.server_bullet_impacts_color.g(), g_cfg.esp.server_bullet_impacts_color.b(), g_cfg.esp.server_bullet_impacts_color.a(), 4.0f);

			aim_shot* current_shot = nullptr;

			for (auto& shot : g_ctx.shots)
			{
				if (!shot.start)
					continue;

				if (shot.end)
					continue;

				current_shot = &shot;
				break;
			}

			if (current_shot && aim::get().last_target[current_shot->last_target].record.player->valid(true, false)) //-V807
			{
				auto backup_data = adjust_data(aim::get().last_target[current_shot->last_target].record.player);
				aim::get().last_target[current_shot->last_target].record.adjust_player();

				trace_t trace, trace_zero, trace_first, trace_second;
				Ray_t ray;

				ray.Init(aim::get().last_shoot_position, position);
				m_trace()->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, aim::get().last_target[current_shot->last_target].record.player, &trace);

				if (aim::get().last_target[current_shot->last_target].data.point.safe)
				{
					memcpy(aim::get().last_target[current_shot->last_target].record.player->m_CachedBoneData().Base(), aim::get().last_target[current_shot->last_target].record.matrixes_data.zero, aim::get().last_target[current_shot->last_target].record.player->m_CachedBoneData().Count() * sizeof(matrix3x4_t)); //-V807
					m_trace()->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, aim::get().last_target[current_shot->last_target].record.player, &trace_zero);

					memcpy(aim::get().last_target[current_shot->last_target].record.player->m_CachedBoneData().Base(), aim::get().last_target[current_shot->last_target].record.matrixes_data.first, aim::get().last_target[current_shot->last_target].record.player->m_CachedBoneData().Count() * sizeof(matrix3x4_t)); //-V807
					m_trace()->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, aim::get().last_target[current_shot->last_target].record.player, &trace_first);

					memcpy(aim::get().last_target[current_shot->last_target].record.player->m_CachedBoneData().Base(), aim::get().last_target[current_shot->last_target].record.matrixes_data.second, aim::get().last_target[current_shot->last_target].record.player->m_CachedBoneData().Count() * sizeof(matrix3x4_t)); //-V807
					m_trace()->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, aim::get().last_target[current_shot->last_target].record.player, &trace_second);
				}

				auto hit = trace.hit_entity == aim::get().last_target[current_shot->last_target].record.player;

				if (aim::get().last_target[current_shot->last_target].data.point.safe)
					hit = hit && trace_zero.hit_entity == aim::get().last_target[current_shot->last_target].record.player && trace_first.hit_entity == aim::get().last_target[current_shot->last_target].record.player && trace_second.hit_entity == aim::get().last_target[current_shot->last_target].record.player;

				if (hit)
					current_shot->impact_hit_player = true;
				else if (aim::get().last_shoot_position.DistTo(position) < aim::get().last_target[current_shot->last_target].distance)
					current_shot->occlusion = true;
				else
					current_shot->occlusion = false;

				current_shot->impacts = true;
				backup_data.adjust_player();
			}
		}
	}
	else if (!strcmp(event_name, crypt_str("player_death")))
	{
		auto attacker = event->GetInt(crypt_str("attacker"));
		auto user = event->GetInt(crypt_str("userid"));

		auto attacker_id = m_engine()->GetPlayerForUserID(attacker); //-V807
		auto user_id = m_engine()->GetPlayerForUserID(user);

		if (g_ctx.local()->is_alive() && attacker_id == m_engine()->GetLocalPlayer() && user_id != m_engine()->GetLocalPlayer())
		{
			auto entity = static_cast<player_t*>(m_entitylist()->GetClientEntity(user_id));

			if (g_cfg.player.enable && g_cfg.esp.kill_effect)
				g_ctx.local()->m_flHealthShotBoostExpirationTime() = m_globals()->m_curtime + g_cfg.esp.kill_effect_duration;

			if (g_cfg.esp.killsound)
			{
				auto headshot = event->GetBool(crypt_str("headshot"));

				switch (g_ctx.globals.kills)
				{
				case 0:
					m_surface()->PlaySound_(headshot ? crypt_str("kill2.wav") : crypt_str("kill1.wav"));
					break;
				case 1:
					m_surface()->PlaySound_(crypt_str("kill3.wav"));
					break;
				case 2:
					m_surface()->PlaySound_(crypt_str("kill4.wav"));
					break;					
				case 3:						
					m_surface()->PlaySound_(crypt_str("kill5.wav"));
					break;					
				case 4:						
					m_surface()->PlaySound_(crypt_str("kill6.wav"));
					break;				
				case 5:					
					m_surface()->PlaySound_(crypt_str("kill7.wav"));
					break;				
				case 6:					
					m_surface()->PlaySound_(crypt_str("kill8.wav"));
					break;				
				case 7:					
					m_surface()->PlaySound_(crypt_str("kill9.wav"));
					break;					
				case 8:						
					m_surface()->PlaySound_(crypt_str("kill10.wav"));
					break;				
				default:				
					m_surface()->PlaySound_(crypt_str("kill11.wav"));
					break;
				}
			}

			g_ctx.globals.kills++;

			std::string weapon_name = event->GetString(crypt_str("weapon"));
			auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

			if (weapon && weapon_name.find(crypt_str("knife")) != std::string::npos)
				SkinChanger::overrideHudIcon(event);
		}
	}
	else if (!strcmp(event_name, crypt_str("player_hurt")))
	{
		auto attacker = event->GetInt(crypt_str("attacker"));
		auto user = event->GetInt(crypt_str("userid"));

		auto attacker_id = m_engine()->GetPlayerForUserID(attacker);
		auto user_id = m_engine()->GetPlayerForUserID(user);

		if (attacker_id == m_engine()->GetLocalPlayer() && user_id != m_engine()->GetLocalPlayer())
		{
			if (g_cfg.esp.hitsound && g_cfg.player.enable)
			{
				switch (g_cfg.esp.hitsound)
				{
				case 1:
					m_surface()->PlaySound_(crypt_str("metallic.wav"));
					break;
				case 2:
					m_surface()->PlaySound_(crypt_str("cod.wav"));
					break;
				case 3:
					m_surface()->PlaySound_(crypt_str("bubble.wav"));
					break;
				case 4:
					m_surface()->PlaySound_(crypt_str("stapler.wav"));
					break;
				case 5:
					m_surface()->PlaySound_(crypt_str("bell.wav"));
					break;
				case 6:
					m_surface()->PlaySound_(crypt_str("flick.wav"));
					break;
				}
			}

			auto entity = static_cast<player_t*>(m_entitylist()->GetClientEntity(user_id));

			std::string weapon = event->GetString(crypt_str("weapon"));
			auto damage = event->GetInt(crypt_str("dmg_health"));
			auto hitgroup = event->GetInt(crypt_str("hitgroup"));

			static auto get_hitbox_by_hitgroup = [](int hitgroup) -> int
			{
				switch (hitgroup)
				{
				case HITGROUP_HEAD:
					return HITBOX_HEAD;
				case HITGROUP_CHEST:
					return HITBOX_CHEST;
				case HITGROUP_STOMACH:
					return HITBOX_STOMACH;
				case HITGROUP_LEFTARM:
					return HITBOX_LEFT_HAND;
				case HITGROUP_RIGHTARM:
					return HITBOX_RIGHT_HAND;
				case HITGROUP_LEFTLEG:
					return HITBOX_RIGHT_CALF;
				case HITGROUP_RIGHTLEG:
					return HITBOX_LEFT_CALF;
				default:
					return HITBOX_PELVIS;
				}
			};

			static auto get_hitbox_name = [](int hitbox) -> std::string
			{
				switch (hitbox)
				{
				case HITBOX_HEAD:
					return crypt_str("Head");
				case HITBOX_CHEST:
					return crypt_str("Chest");
				case HITBOX_STOMACH:
					return crypt_str("Stomach");
				case HITBOX_PELVIS:
					return crypt_str("Pelvis");
				case HITBOX_RIGHT_UPPER_ARM:
				case HITBOX_RIGHT_FOREARM:
				case HITBOX_RIGHT_HAND:
					return crypt_str("Left arm");
				case HITBOX_LEFT_UPPER_ARM:
				case HITBOX_LEFT_FOREARM:
				case HITBOX_LEFT_HAND:
					return crypt_str("Right arm");
				case HITBOX_RIGHT_THIGH:
				case HITBOX_RIGHT_CALF:
					return crypt_str("Left leg");
				case HITBOX_LEFT_THIGH:
				case HITBOX_LEFT_CALF:
					return crypt_str("Right leg");
				case HITBOX_RIGHT_FOOT:
					return crypt_str("Left foot");
				case HITBOX_LEFT_FOOT:
					return crypt_str("Right foot");
				}
			};

			aim_shot* current_shot = nullptr;

			for (auto& shot : g_ctx.shots)
			{
				if (!shot.start)
					continue;

				if (shot.end)
					continue;

				current_shot = &shot;
				break;
			}

			if (weapon_is_aim(weapon))
			{
				if (current_shot && entity == aim::get().last_target[current_shot->last_target].record.player)
					misc::get().aimbot_hitboxes();

				otheresp::get().hitmarker.hurt_time = m_globals()->m_curtime;
				otheresp::get().hitmarker.point = entity->hitbox_position_matrix(get_hitbox_by_hitgroup(hitgroup), current_shot && entity == aim::get().last_target[current_shot->last_target].record.player ? aim::get().last_target[current_shot->last_target].record.matrixes_data.main : entity->m_CachedBoneData().Base());
				otheresp::get().damage_marker[user_id] = otheresp::Damage_marker
				{
					entity->hitbox_position_matrix(get_hitbox_by_hitgroup(hitgroup), current_shot && entity == aim::get().last_target[current_shot->last_target].record.player ? aim::get().last_target[current_shot->last_target].record.matrixes_data.main : entity->m_CachedBoneData().Base()),
					m_globals()->m_curtime,
					Color::White,
					damage,
					hitgroup
				};
			}
			
			auto headshot = hitgroup == HITGROUP_HEAD;
			auto health = entity->m_iHealth() - damage;

			if (health <= 0 && headshot)
				otheresp::get().hitmarker.hurt_color = Color::Red;
			else if (health <= 0)
				otheresp::get().hitmarker.hurt_color = Color::Yellow;
			else
				otheresp::get().hitmarker.hurt_color = Color::White;

			otheresp::get().damage_marker[user_id].hurt_color = headshot ? Color::Red : Color::White;

			if (current_shot)
			{
				current_shot->shot_info.result = crypt_str("Hit");
				current_shot->shot_info.server_hitbox = get_hitbox_name(get_hitbox_by_hitgroup(hitgroup));
				current_shot->shot_info.server_damage = damage;

				current_shot->end = true;
				current_shot->hurt_player = true;
			}
		}
	}
	else if (!strcmp(event_name, crypt_str("round_start")))
	{
		if (!g_cfg.misc.show_default_log && eventlogs::get().last_log && g_cfg.misc.log_output[EVENTLOG_OUTPUT_CONSOLE])
		{
			eventlogs::get().last_log = false;
			m_cvar()->ConsolePrintf(crypt_str("-------------------------\n-------------------------\n-------------------------\n"));
		}

		for (auto i = 1; i < m_globals()->m_maxclients; i++)
		{
			g_ctx.globals.fired_shots[i] = 0;
			g_ctx.globals.missed_shots[i] = 0;
			lagcompensation::get().is_dormant[i] = false;
			lagcompensation::get().player_resolver[i].reset();
			playeresp::get().esp_alpha_fade[i] = 0.0f;
			playeresp::get().health[i] = 100;
			c_dormant_esp::get().m_cSoundPlayers[i].reset();
		}
		
		antiaim::get().freeze_check = true;
		g_ctx.globals.bomb_timer_enable = true;
		g_ctx.globals.should_buy = 2;
		g_ctx.globals.should_clear_death_notices = true;
		g_ctx.globals.should_update_playerresource = true;
		g_ctx.globals.should_update_gamerules = true;
		g_ctx.globals.kills = 0;
		g_ctx.shots.clear();
	}
	else if (!strcmp(event_name, crypt_str("round_freeze_end")))
		antiaim::get().freeze_check = false;
	else if (!strcmp(event_name, crypt_str("bomb_defused")))
		g_ctx.globals.bomb_timer_enable = false;

	if ((g_cfg.esp.bullet_tracer || g_cfg.esp.enemy_bullet_tracer) && g_cfg.player.enable)
		bullettracers::get().events(event);

	eventlogs::get().events(event);
}

int C_HookedEvents::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void C_HookedEvents::RegisterSelf()
{
	m_iDebugId = EVENT_DEBUG_ID_INIT;
	auto eventmanager = m_eventmanager();

	eventmanager->AddListener(this, crypt_str("player_footstep"), false);
	eventmanager->AddListener(this, crypt_str("player_hurt"), false);
	eventmanager->AddListener(this, crypt_str("player_death"), false);
	eventmanager->AddListener(this, crypt_str("weapon_fire"), false);
	eventmanager->AddListener(this, crypt_str("item_purchase"), false);
	eventmanager->AddListener(this, crypt_str("bullet_impact"), false);
	eventmanager->AddListener(this, crypt_str("round_start"), false);
	eventmanager->AddListener(this, crypt_str("round_freeze_end"), false);
	eventmanager->AddListener(this, crypt_str("bomb_defused"), false);
	eventmanager->AddListener(this, crypt_str("bomb_begindefuse"), false);
	eventmanager->AddListener(this, crypt_str("bomb_beginplant"), false);

	g_ctx.globals.events.emplace_back(crypt_str("player_footstep"));
	g_ctx.globals.events.emplace_back(crypt_str("player_hurt"));
	g_ctx.globals.events.emplace_back(crypt_str("player_death"));
	g_ctx.globals.events.emplace_back(crypt_str("weapon_fire"));
	g_ctx.globals.events.emplace_back(crypt_str("item_purchase"));
	g_ctx.globals.events.emplace_back(crypt_str("bullet_impact"));
	g_ctx.globals.events.emplace_back(crypt_str("round_start"));
	g_ctx.globals.events.emplace_back(crypt_str("round_freeze_end"));
	g_ctx.globals.events.emplace_back(crypt_str("bomb_defused"));
	g_ctx.globals.events.emplace_back(crypt_str("bomb_begindefuse"));
	g_ctx.globals.events.emplace_back(crypt_str("bomb_beginplant"));
}

void C_HookedEvents::RemoveSelf()
{
	m_eventmanager()->RemoveListener(this);
}