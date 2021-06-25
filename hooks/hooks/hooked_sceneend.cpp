// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\misc\fakelag.h"

using SceneEnd_t = void(__thiscall*)(void*);

void __fastcall hooks::hooked_sceneend(void* ecx, void* edx)
{
	static auto original_fn = renderview_hook->get_func_address <SceneEnd_t> (9);
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true); //-V807

	if (!g_ctx.local())
		return original_fn(ecx);

	if (!g_cfg.player.type[ENEMY].ragdoll_chams && !g_cfg.player.type[TEAM].ragdoll_chams)
		return original_fn(ecx);

	for (auto i = 1; i <= m_entitylist()->GetHighestEntityIndex(); i++)
	{
		auto e = static_cast<entity_t*>(m_entitylist()->GetClientEntity(i));

		if (!e) 
			continue;

		if (((player_t*)e)->m_lifeState() == LIFE_ALIVE)
			continue;

		auto client_class = e->GetClientClass();

		if (!client_class)
			continue;

		if (client_class->m_ClassID != CCSRagdoll)
			continue;

		auto type = ENEMY;

		if (e->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			type = TEAM;
		
		if (!g_cfg.player.type[type].ragdoll_chams)
			continue;

		static IMaterial* chams_materials[] =
		{
			m_materialsystem()->FindMaterial(crypt_str("lwk_chams"), nullptr), //-V807
			m_materialsystem()->FindMaterial(crypt_str("lwk_metallic"), nullptr),
			m_materialsystem()->FindMaterial(crypt_str("lwk_flat"), nullptr),
			m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/dogtags/dogtags_outline"), nullptr),
			m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/trophy_majors/crystal_clear"), nullptr),
			m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/cologne_prediction/cologne_prediction_glass"), nullptr),
			m_materialsystem()->FindMaterial(crypt_str("dev/glow_armsrace.vmt"), nullptr),
			m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/wildfire_gold/wildfire_gold_detail"), nullptr),
			m_materialsystem()->FindMaterial(crypt_str("lwk_glow"), nullptr)
		};

		auto material = chams_materials[g_cfg.player.type[type].ragdoll_chams_material];

		if (material && !material->IsErrorMaterial())
		{
			auto alpha = (float)g_cfg.player.type[type].ragdoll_chams_color.a() / 255.0f;

			float ragdoll_color[3] =
			{
				g_cfg.player.type[type].ragdoll_chams_color[0] / 255.0f,
				g_cfg.player.type[type].ragdoll_chams_color[1] / 255.0f,
				g_cfg.player.type[type].ragdoll_chams_color[2] / 255.0f
			};

			m_renderview()->SetBlend(alpha);
			util::color_modulate(ragdoll_color, material);

			material->IncrementReferenceCount();
			material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

			m_modelrender()->ForcedMaterialOverride(material);
			e->DrawModel(0x1, 255);
			m_modelrender()->ForcedMaterialOverride(nullptr);
		}
	}

	return original_fn(ecx);
}