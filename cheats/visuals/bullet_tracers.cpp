// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "bullet_tracers.h"
#include "..\..\sdk\misc\BeamInfo_t.hpp"
#include "..\ragebot\aim.h"
#include "..\..\utils\ctx.hpp"
#include "..\misc\logs.h"

void bullettracers::draw_beam(bool local_tracer, const Vector& src, const Vector& end, Color color)
{
	if (src == ZERO)
		return;

	BeamInfo_t beam_info;
	beam_info.m_vecStart = src;

	if (local_tracer)
		beam_info.m_vecStart.z -= 2.0f;
	
	beam_info.m_vecEnd = end;
	beam_info.m_nType = TE_BEAMPOINTS;
	beam_info.m_pszModelName = crypt_str("sprites/purplelaser1.vmt");
	beam_info.m_nModelIndex = -1;
	beam_info.m_flHaloScale = 0.0f;
	beam_info.m_flLife = 4.0f;
	beam_info.m_flWidth = 2.0f;
	beam_info.m_flEndWidth = 2.0f;
	beam_info.m_flFadeLength = 0.0f;
	beam_info.m_flAmplitude = 2.0f;
	beam_info.m_flBrightness = (float)color.a();
	beam_info.m_flSpeed = 0.2f;
	beam_info.m_nStartFrame = 0;
	beam_info.m_flFrameRate = 0.0f;
	beam_info.m_flRed = (float)color.r();
	beam_info.m_flGreen = (float)color.g();
	beam_info.m_flBlue = (float)color.b();
	beam_info.m_nSegments = 2;
	beam_info.m_bRenderable = true; 
	beam_info.m_nFlags = FBEAM_SHADEIN | FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;
	
	auto beam = m_viewrenderbeams()->CreateBeamPoints(beam_info);
	
	if (beam)
		m_viewrenderbeams()->DrawBeam(beam);
}

void bullettracers::events(IGameEvent* event)
{
	auto event_name = event->GetName();

	if (!strcmp(event_name, crypt_str("bullet_impact")))
	{
		auto user_id = event->GetInt(crypt_str("userid"));
		auto user = m_engine()->GetPlayerForUserID(user_id);

		auto e = static_cast<player_t *>(m_entitylist()->GetClientEntity(user));

		if (e->valid(false))
		{
			if (e == g_ctx.local())
			{
				auto new_record = true;
				Vector position(event->GetFloat(crypt_str("x")), event->GetFloat(crypt_str("y")), event->GetFloat(crypt_str("z")));

				for (auto& current : impacts)
				{
					if (e == current.e)
					{
						new_record = false;

						current.impact_position = position;
						current.time = m_globals()->m_curtime;
					}
				}

				if (new_record)
					impacts.push_back(
						impact_data
						{
							e,
							position,
							m_globals()->m_curtime
						});
			}
			else if (e->m_iTeamNum() != g_ctx.local()->m_iTeamNum())
			{
				auto new_record = true;
				Vector position(event->GetFloat(crypt_str("x")), event->GetFloat(crypt_str("y")), event->GetFloat(crypt_str("z")));

				for (auto& current : impacts)
				{
					if (e == current.e)
					{
						new_record = false;

						current.impact_position = position;
						current.time = m_globals()->m_curtime;
					}
				}

				if (new_record)
					impacts.push_back(
						impact_data
						{
							e,
							position,
							m_globals()->m_curtime
						});
			}
		}
	}
}

void bullettracers::draw_beams()
{
	if (impacts.empty())
		return;

	while (!impacts.empty())
	{
		if (impacts.begin()->impact_position.IsZero()) //-V807
		{
			impacts.erase(impacts.begin());
			continue;
		}

		if (fabs(m_globals()->m_curtime - impacts.begin()->time) > 4.0f) //-V807
		{
			impacts.erase(impacts.begin());
			continue;
		}

		if (!impacts.begin()->e->valid(false))
		{
			impacts.erase(impacts.begin());
			continue;
		}

		if (TIME_TO_TICKS(m_globals()->m_curtime) > TIME_TO_TICKS(impacts.begin()->time))
		{
			auto color = g_cfg.esp.enemy_bullet_tracer_color;

			if (impacts.begin()->e == g_ctx.local())
			{
				if (!g_cfg.esp.bullet_tracer)
				{
					impacts.erase(impacts.begin());
					continue;
				}

				color = g_cfg.esp.bullet_tracer_color;
			}
			else if (!g_cfg.esp.enemy_bullet_tracer)
			{
				impacts.erase(impacts.begin());
				continue;
			}

			draw_beam(impacts.begin()->e == g_ctx.local(), impacts.begin()->e == g_ctx.local() ? aim::get().last_shoot_position : impacts.begin()->e->get_shoot_position(), impacts.begin()->impact_position, color);
			impacts.erase(impacts.begin());
			continue;
		}

		break;
	}
}