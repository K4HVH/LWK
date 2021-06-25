#pragma once

#include "..\..\includes.hpp"
#include "..\..\cheats\visuals\player_esp.h"
#include "..\..\cheats\visuals\dormant_esp.h"

class Radar : public singleton <Radar>
{
private:
	bool m_bLoaded = false;
	IDirect3DTexture9* m_pMapTexture = nullptr;

	Vector m_vMapOrigin;
	double m_flMapScale;
	Vector bomb_siteA;
	Vector bomb_siteB;

	int m_iTexture;
	bool draw_plantA;
	bool draw_plantB;

	std::string parseString(const std::string& szBefore, const std::string& szSource)
	{
		if (!szBefore.empty() && !szSource.empty() && (szSource.find(szBefore) != std::string::npos))
		{
			std::string t = strstr(szSource.c_str(), szBefore.c_str()); //-V522
			t.erase(0, szBefore.length());
			size_t firstLoc = t.find('\"', 0);
			size_t secondLoc = t.find('\"', firstLoc + 1);
			t = t.substr(firstLoc + 1, secondLoc - 3);
			return t;
		}
		else
			return crypt_str("");
	}

	Vector WorldToMap(const Vector& vWorldPosition)
	{
		const int iMapCenter = 512;

		// World2Map
		Vector vMapPosition;

		vMapPosition.x = (vWorldPosition.x - m_vMapOrigin.x) / m_flMapScale;
		vMapPosition.y = (vWorldPosition.y - m_vMapOrigin.y) / -m_flMapScale;
		vMapPosition.z = 0;

		// Map2Panel
		Vector vOffset;
		vOffset.x = vMapPosition.x - iMapCenter;
		vOffset.y = vMapPosition.y - iMapCenter;
		vOffset.z = 0;

		// (m_fZoom * m_fFullZoom) / OVERVIEW_MAP_SIZE
		float fScale = 1.f / 1024;
		vOffset.x *= fScale;
		vOffset.y *= fScale;

		Vector vFinalPos;
		vFinalPos.x = (ImGui::GetWindowSize().x * 0.5f) + (ImGui::GetWindowSize().x * vOffset.x);
		vFinalPos.y = (ImGui::GetWindowSize().x * 0.5f) + (ImGui::GetWindowSize().x * vOffset.y);
		vFinalPos.z = 0;

		vMapPosition.x = vMapPosition.x / (1024.f / ImGui::GetWindowSize().x);
		vMapPosition.y = vMapPosition.y / (1024.f / ImGui::GetWindowSize().y);

		vMapPosition.x += ImGui::GetWindowPos().x;
		vMapPosition.y += ImGui::GetWindowPos().y;

		return vMapPosition;
	}

	bool RenderMap()
	{
		// check if texture is loaded and ingame bools
		bool bTrueRadar = Radar::m_bLoaded && m_engine()->IsInGame() && m_engine()->IsConnected();
		if (!bTrueRadar)
			return false;

		static auto open = true;
		static auto set_once = false;
		if (!set_once)
		{
			ImGui::SetNextWindowSize({ 256.f, 256.f }, ImGuiCond_Once);
			set_once = true;
		}

		ImGui::Begin("##radar", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

		if (ImGui::GetWindowSize().x != ImGui::GetWindowSize().y) //-V550
			ImGui::SetWindowSize(ImGui::GetWindowSize().x > ImGui::GetWindowSize().y ? ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().x) : ImVec2(ImGui::GetWindowSize().y, ImGui::GetWindowSize().y));


		// Vars
		int x = ImGui::GetWindowPos().x;
		int y = ImGui::GetWindowPos().y;
		int size = ImGui::GetWindowSize().x;
		int center_size = (size / 2);

		// Limit radar bounds
		ImGui::GetWindowDrawList()->PushClipRect(ImVec2(x, y), ImVec2(x + size, y + size), false);

		// Scale map
		float new_size = size;
		float size_diff = new_size - size;
		size = new_size;
		x -= size_diff / 2;
		y -= size_diff / 2;

		// Locate map by our local player
		auto local_player = g_ctx.local();


		// Prepare imgui shit
	
		ImVec2 uv_a(0, 0), uv_c(1, 1), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
		ImVec2 a(x, y), c(x + size, y + size), b(c.x, a.y), d(a.x, c.y);


		// Render background first
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), ImColor(0, 0, 0));

		// Render
		ImGui::GetWindowDrawList()->PushTextureID(m_pMapTexture);
		ImGui::GetWindowDrawList()->PrimReserve(6, 4);
		ImGui::GetWindowDrawList()->PrimQuadUV(a, b, c, d, uv_a, uv_b, uv_c, uv_d, 0xFFFFFFFF);
		ImGui::GetWindowDrawList()->PopTextureID();

		// Remove limit
		ImGui::GetWindowDrawList()->PopClipRect();

		if (local_player)
		{
			for (auto i = 1; i <= m_entitylist()->GetHighestEntityIndex(); i++)
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

				if (client_class->m_ClassID == CC4 && g_cfg.radar.render_dropped_c4)
				{
					auto owner = (player_t*)m_entitylist()->GetClientEntityFromHandle(e->m_hOwnerEntity());

					if (!owner->is_player())
					{
						Vector bomb_pos;

						bomb_pos = e->GetAbsOrigin();
						bomb_pos = Radar::WorldToMap(bomb_pos);

						ImGui::GetWindowDrawList()->AddText(ImVec2(bomb_pos.x - 1, bomb_pos.y), ImColor(225, 165, 25), "C4");
					}
				}
				else if (client_class->m_ClassID == CPlantedC4 && g_cfg.radar.render_planted_c4)
				{
					Vector bomb_pos;

					bomb_pos = e->GetAbsOrigin();
					bomb_pos = Radar::WorldToMap(bomb_pos);

					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(bomb_pos.x - 2, bomb_pos.y - 2), ImVec2(bomb_pos.x + 1, bomb_pos.y + 2), ImGui::GetColorU32(ImVec4(225 / 255.f, 165 / 255.f, 25 / 255.f, 1.f)));
				}
			}

			if (local_player->is_alive() && g_cfg.radar.render_local)
			{
				Vector world_pos = local_player->GetAbsOrigin();
				Vector map_pos = WorldToMap(world_pos);

				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(map_pos.x, map_pos.y), 5, ImColor(30, 255, 30), 25);
			}

			for (auto i = 1; i < m_globals()->m_maxclients; i++)
			{
				auto e = (player_t*)m_entitylist()->GetClientEntity(i);

				if (!e->valid(false, false))
					continue;

				if (e == local_player)
					continue;

				if (e->m_iTeamNum() == local_player->m_iTeamNum() && !g_cfg.radar.render_team)
					continue;

				if (e->m_iTeamNum() != local_player->m_iTeamNum() && !g_cfg.radar.render_enemy)
					continue;

				auto team_num = e->m_iTeamNum();
				bool bIsEnemy = local_player->m_iTeamNum() != team_num;

				Vector EntityPos;

				EntityPos = e->IsDormant() ? g_ctx.globals.dormant_origin[i] : e->GetAbsOrigin();
				EntityPos = Radar::WorldToMap(EntityPos);

				ImColor clr = bIsEnemy ? ImColor(255, 50, 50) : (team_num == 3 ? ImColor(95, 185, 245) : ImColor(245, 205, 65));
				ImColor clr_dormant = bIsEnemy ? ImColor(255, 50, 50, (int)(playeresp::get().esp_alpha_fade[i] * 255.0f)) : (team_num == 2 ? ImColor(95, 185, 245) : ImColor(245, 205, 65));

				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(EntityPos.x, EntityPos.y), 5, e->IsDormant() ? clr_dormant : clr, 30);

				if (g_cfg.radar.render_health)
				{
					auto hp = math::clamp((float)e->m_iHealth(), 0.0f, 100.0f) / 100.0f;

					if (hp <= 0.0f)
						continue;

					auto height = math::clamp((float)e->m_iHealth() / 20.0f, 2.0f, 5.0f);

					ImColor clr = ImColor(int((1 - hp) * 255), int(hp * 255), 0, 255);
					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(EntityPos.x - 4, EntityPos.y - height), ImVec2(EntityPos.x - 2, EntityPos.y + height), clr);
				}
			}

			if (draw_plantA && !bomb_siteA.IsZero())
			{
				Vector plant_pos_A;
				plant_pos_A.x = bomb_siteA.x * ImGui::GetWindowSize().x;
				plant_pos_A.x += ImGui::GetWindowPos().x;
				plant_pos_A.y = bomb_siteA.y * ImGui::GetWindowSize().y;
				plant_pos_A.y += ImGui::GetWindowPos().y;

				ImGui::GetWindowDrawList()->AddText(ImVec2(plant_pos_A.x, plant_pos_A.y), ImColor(225, 225, 25), "A");
			}

			if (draw_plantB && !bomb_siteB.IsZero())
			{
				Vector plant_pos_B;
				plant_pos_B.x = bomb_siteB.x * ImGui::GetWindowSize().x;
				plant_pos_B.x += ImGui::GetWindowPos().x;
				plant_pos_B.y = bomb_siteB.y * ImGui::GetWindowSize().y;
				plant_pos_B.y += ImGui::GetWindowPos().y;

				ImGui::GetWindowDrawList()->AddText(ImVec2(plant_pos_B.x, plant_pos_B.y), ImColor(225, 225, 25), "B");
			}
		}

		ImGui::End();
		return true;
	}
public:
	bool OnMapLoad(const char* pszMapName, IDirect3DDevice9* pDevice)
	{
		draw_plantA = false;
		draw_plantB = false;
		m_bLoaded = false;

		char szPath[MAX_PATH];
		sprintf(szPath, crypt_str("csgo\\resource\\overviews\\%s_radar.dds"), pszMapName);

		std::ifstream file;
		file.open(szPath);

		if (!file.is_open())
			return false;

		file.close();

		if (FAILED(D3DXCreateTextureFromFileA(pDevice, szPath, &m_pMapTexture)))
			return false;

		sprintf(szPath, crypt_str(".\\csgo\\resource\\overviews\\%s.txt"), pszMapName);
		std::ifstream ifs(szPath);

		if (ifs.bad())
			return false;

		std::string szInfo((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

		if (szInfo.empty())
			return false;

		m_vMapOrigin.x = std::stoi(parseString(crypt_str("\"pos_x\""), szInfo));
		m_vMapOrigin.y = std::stoi(parseString(crypt_str("\"pos_y\""), szInfo));
		m_flMapScale = std::stod(parseString(crypt_str("\"scale\""), szInfo));

		auto bombA_x = parseString(crypt_str("\"bombA_x\""), szInfo);

		if (!bombA_x.empty())
		{
			bomb_siteA.x = std::stod(parseString(crypt_str("\"bombA_x\""), szInfo));
			bomb_siteA.y = std::stod(parseString(crypt_str("\"bombA_y\""), szInfo));
			bomb_siteA.z = 0.0f;

			draw_plantA = true;
		}

		auto bombB_x = parseString(crypt_str("\"bombB_x\""), szInfo);

		if (!bombB_x.empty())
		{
			bomb_siteB.x = std::stod(parseString(crypt_str("\"bombB_x\""), szInfo));
			bomb_siteB.y = std::stod(parseString(crypt_str("\"bombB_y\""), szInfo));
			bomb_siteB.z = 0.0f;

			draw_plantB = true;
		}

		m_bLoaded = true;
		g_ctx.globals.should_update_radar = false;

		return true;
	}

	void Render()
	{
		if (!m_pMapTexture)
			return;

		if (!m_bLoaded)
			return;

		if (!g_cfg.misc.ingame_radar)
			return;

		RenderMap();
	}
};
