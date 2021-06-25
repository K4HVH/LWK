// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\menu.h"
#include "..\..\cheats\lagcompensation\animation_system.h"
#include "..\..\cheats\visuals\player_esp.h"
#include "..\..\cheats\visuals\other_esp.h"
#include "..\..\cheats\misc\logs.h"
#include "..\..\cheats\visuals\world_esp.h"
#include "..\..\cheats\misc\misc.h"
#include "..\..\cheats\visuals\GrenadePrediction.h"
#include "..\..\cheats\visuals\bullet_tracers.h"
#include "..\..\cheats\visuals\dormant_esp.h"
#include "..\..\cheats\lagcompensation\local_animations.h"

using PaintTraverse_t = void(__thiscall*)(void*, vgui::VPANEL, bool, bool);

bool reload_fonts()
{
	static int old_width, old_height;
	static int width, height;

	m_engine()->GetScreenSize(width, height);

	if (width != old_width || height != old_height)
	{
		old_width = width;
		old_height = height;

		return true;
	}

	return false;
}

void __fastcall hooks::hooked_painttraverse(void* ecx, void* edx, vgui::VPANEL panel, bool force_repaint, bool allow_force)
{
	static auto original_fn = panel_hook->get_func_address <PaintTraverse_t> (41);
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true); //-V807

	static auto set_console = true;

	if (set_console)
	{
		set_console = false;

		m_cvar()->FindVar(crypt_str("developer"))->SetValue(FALSE); //-V807
		m_cvar()->FindVar(crypt_str("con_filter_enable"))->SetValue(TRUE);
		m_cvar()->FindVar(crypt_str("con_filter_text"))->SetValue(crypt_str(""));
		m_engine()->ExecuteClientCmd(crypt_str("clear"));
	}

	static auto log_value = true;

	if (log_value != g_cfg.misc.show_default_log)
	{
		log_value = g_cfg.misc.show_default_log;

		if (log_value)
			m_cvar()->FindVar(crypt_str("con_filter_text"))->SetValue(crypt_str(""));
		else
			m_cvar()->FindVar(crypt_str("con_filter_text"))->SetValue(crypt_str("IrWL5106TZZKNFPz4P4Gl3pSN?J370f5hi373ZjPg%VOVh6lN"));
	}

	static vgui::VPANEL panel_id = 0;
	static auto in_game = false;

	if (!in_game && m_engine()->IsInGame()) //-V807
	{
		in_game = true;

		for (auto i = 1; i < 65; i++)
		{
			g_ctx.globals.fired_shots[i] = 0;
			g_ctx.globals.missed_shots[i] = 0;
			player_records[i].clear();
			lagcompensation::get().is_dormant[i] = false;
			playeresp::get().esp_alpha_fade[i] = 0.0f;
			playeresp::get().health[i] = 100;
			c_dormant_esp::get().m_cSoundPlayers[i].reset();
			otheresp::get().damage_marker[i].reset();
		}

		antiaim::get().freeze_check = false;
		g_ctx.globals.next_lby_update = FLT_MIN;
		g_ctx.globals.last_lby_move = FLT_MIN;
		g_ctx.globals.last_aimbot_shot = 0;
		g_ctx.globals.bomb_timer_enable = true;
		g_ctx.globals.backup_model = false;
		g_ctx.globals.should_remove_smoke = false;
		g_ctx.globals.should_update_beam_index = true;
		g_ctx.globals.should_update_playerresource = true;
		g_ctx.globals.should_update_gamerules = true;
		g_ctx.globals.should_update_radar = true;
		g_ctx.globals.kills = 0;
		g_ctx.shots.clear();
		otheresp::get().hitmarker.hurt_time = FLT_MIN;
		otheresp::get().hitmarker.point = ZERO;
		g_ctx.globals.commands.clear();
		SkinChanger::model_indexes.clear();
		SkinChanger::player_model_indexes.clear();
	}
	else if (in_game && !m_engine()->IsInGame())
	{
		in_game = false;

		g_ctx.globals.should_update_weather = true;
		g_ctx.globals.m_networkable = nullptr;

		g_cfg.player_list.players.clear();

		misc::get().double_tap_enabled = false;
		misc::get().double_tap_key = false;

		misc::get().hide_shots_enabled = false;
		misc::get().hide_shots_key = false;
	}

	if (m_engine()->IsTakingScreenshot() && g_cfg.misc.anti_screenshot)
		return;

	static uint32_t HudZoomPanel = 0;
	
	if (!HudZoomPanel)
		if (!strcmp(crypt_str("HudZoom"), m_panel()->GetName(panel)))
			HudZoomPanel = panel;

	if (HudZoomPanel == panel && g_cfg.player.enable && g_cfg.esp.removals[REMOVALS_SCOPE])
		return;

	original_fn(ecx, panel, force_repaint, allow_force);

	if (!panel_id)
	{
		auto panelName = m_panel()->GetName(panel);

		if (!strcmp(panelName, crypt_str("MatSystemTopPanel")))
			panel_id = panel;
	}

	if (reload_fonts())
	{
		static auto create_font = [](const char* name, int size, int weight, DWORD flags) -> vgui::HFont
		{
			g_ctx.last_font_name = name;

			auto font = m_surface()->FontCreate();
			m_surface()->SetFontGlyphSet(font, name, size, weight, NULL, NULL, flags);

			return font;
		};

		fonts[LOGS] = create_font(crypt_str("Lucida Console"), 10, FW_MEDIUM, FONTFLAG_DROPSHADOW);
		fonts[ESP] = create_font(crypt_str("Smallest Pixel-7"), 11, FW_MEDIUM, FONTFLAG_OUTLINE);
		fonts[NAME] = create_font(crypt_str("Verdana"), 12, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
		fonts[SUBTABWEAPONS] = create_font(crypt_str("undefeated"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
		fonts[KNIFES] = create_font(crypt_str("icomoon"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
		fonts[GRENADES] = create_font(crypt_str("undefeated"), 20, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
		fonts[INDICATORFONT] = create_font(crypt_str("Verdana"), 25, FW_HEAVY, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
		fonts[DAMAGE_MARKER] = create_font(crypt_str("Verdana"), 16, FW_HEAVY, FONTFLAG_OUTLINE);

		g_ctx.last_font_name.clear();
	}

	if (panel_id == panel)
	{
		if (g_ctx.available())
		{
			static auto alive = false;

			if (!alive && g_ctx.local()->is_alive())
			{
				alive = true;
				g_ctx.globals.should_clear_death_notices = true;
			}
			else if (alive && !g_ctx.local()->is_alive())
			{
				alive = false;

				for (auto i = 1; i < m_globals()->m_maxclients; i++)
				{
					g_ctx.globals.fired_shots[i] = 0;
					g_ctx.globals.missed_shots[i] = 0;
				}

				local_animations::get().local_data.prediction_animstate = nullptr;
				local_animations::get().local_data.animstate = nullptr;

				g_ctx.globals.weapon = nullptr;
				g_ctx.globals.should_choke_packet = false;
				g_ctx.globals.should_send_packet = false;
				g_ctx.globals.kills = 0;
				g_ctx.globals.should_buy = 3;
			}

			g_ctx.globals.bomb_carrier = -1;

			if (g_cfg.player.enable)
			{
				worldesp::get().paint_traverse();
				playeresp::get().paint_traverse();
			}

			misc::get().zeus_range();
			misc::get().desync_arrows();

			auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

			if (weapon->is_grenade() && g_cfg.esp.grenade_prediction && g_cfg.player.enable)
				GrenadePrediction::get().Paint();

			if (g_cfg.player.enable && g_cfg.esp.removals[REMOVALS_SCOPE] && g_ctx.globals.scoped && weapon->is_sniper())
			{
				static int w, h;
				m_engine()->GetScreenSize(w, h);

				render::get().line(w / 2, 0, w / 2, h, Color::Black);
				render::get().line(0, h / 2, w, h / 2, Color::Black);
			}

			if (g_ctx.local()->is_alive())
			{
				if (c_menu::get().public_alpha > 0.15f && g_cfg.legitbot.enabled)
				{
					int x, y;
					m_engine()->GetScreenSize(x, y);

					if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].fov)
					{
						float radius = tanf(DEG2RAD(g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].fov) / 2) / tanf(DEG2RAD(90 + g_cfg.esp.fov) / 2) * x;
						render::get().circle_filled(x / 2, y / 2, 60, radius, Color(235, 235, 235, c_menu::get().public_alpha * 0.68));
						render::get().circle(x / 2, y / 2, 60, radius, Color(235, 235, 235, c_menu::get().public_alpha * 0.8));
					}

					if (g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].silent_fov)
					{
						float silent_radius = tanf(DEG2RAD(g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].silent_fov) / 2) / tanf(DEG2RAD(90 + g_cfg.esp.fov) / 2) * x;
						render::get().circle_filled(x / 2, y / 2, 60, silent_radius, Color(15, 235, 15, c_menu::get().public_alpha * 0.68));
						render::get().circle(x / 2, y / 2, 60, silent_radius, Color(15, 235, 15, c_menu::get().public_alpha * 0.8));
					}
				}
			}

			if (g_cfg.player.enable)
				otheresp::get().hitmarker_paint();

			if (g_cfg.player.enable && g_cfg.esp.damage_marker)
				otheresp::get().damage_marker_paint();

			
			otheresp::get().penetration_reticle();
			otheresp::get().automatic_peek_indicator();

			misc::get().ChatSpamer();
			misc::get().spectators_list();

			bullettracers::get().draw_beams();
		}

		static auto framerate = 0.0f;
		framerate = 0.9f * framerate + 0.1f * m_globals()->m_absoluteframetime;

		if (framerate <= 0.0f)
			framerate = 1.0f;

		g_ctx.globals.framerate = (int)(1.0f / framerate);
		auto nci = m_engine()->GetNetChannelInfo();

		if (nci)
		{
			auto latency = m_engine()->IsPlayingDemo() ? 0.0f : nci->GetAvgLatency(FLOW_OUTGOING);

			if (latency) //-V550
			{
				static auto cl_updaterate = m_cvar()->FindVar(crypt_str("cl_updaterate"));
				latency -= 0.5f / cl_updaterate->GetFloat();
			}

			g_ctx.globals.ping = (int)(max(0.0f, latency) * 1000.0f);
		}

		time_t lt;
		struct tm* t_m;

		lt = time(nullptr);
		t_m = localtime(&lt);

		auto time_h = t_m->tm_hour;
		auto time_m = t_m->tm_min;
		auto time_s = t_m->tm_sec;

		std::string time;

		if (time_h < 10)
			time += "0";

		time += std::to_string(time_h) + ":";

		if (time_m < 10)
			time += "0";

		time += std::to_string(time_m) + ":";

		if (time_s < 10)
			time += "0";

		time += std::to_string(time_s);
		g_ctx.globals.time = std::move(time);

		static int w, h;
		m_engine()->GetScreenSize(w, h);

		static auto alpha = 0;
		auto speed = 800.0f * m_globals()->m_frametime;

		if (menu_open)
		{
			if (alpha + speed <= 150)
				alpha += speed;

			render::get().rect_filled(0, 0, w, h, Color(0, 0, 0, alpha));
		}
		else
		{
			if (alpha - speed >= 0)
				alpha -= speed;

			render::get().rect_filled(0, 0, w, h, Color(0, 0, 0, alpha));
		}

		misc::get().watermark();

		eventlogs::get().paint_traverse();

		misc::get().NightmodeFix();
		otheresp::get().indicators();
		if (g_ctx.globals.loaded_script)
			for (auto current : c_lua::get().hooks.getHooks(crypt_str("on_paint")))
				current.func();
		otheresp::get().draw_indicators();
	}
}