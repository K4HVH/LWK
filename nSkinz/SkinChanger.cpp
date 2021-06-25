// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <algorithm>
#include <fstream>
#include "..\configs\configs.h"

static const char* player_model_index_t[] =
{
	"models/player/custom_player/legacy/tm_phoenix_variantf.mdl",
	"models/player/custom_player/legacy/tm_phoenix_varianth.mdl",
	"models/player/custom_player/legacy/tm_leet_variantg.mdl",
	"models/player/custom_player/legacy/tm_balkan_varianti.mdl",
	"models/player/custom_player/legacy/tm_leet_varianth.mdl",
	"models/player/custom_player/legacy/tm_phoenix_variantg.mdl",
	"models/player/custom_player/legacy/tm_balkan_variantf.mdl",
	"models/player/custom_player/legacy/tm_balkan_variantj.mdl",
	"models/player/custom_player/legacy/tm_leet_varianti.mdl",
	"models/player/custom_player/legacy/tm_balkan_variantg.mdl",
	"models/player/custom_player/legacy/tm_balkan_varianth.mdl",
	"models/player/custom_player/legacy/tm_leet_variantf.mdl"
};

static const char* player_model_index_ct[] =
{
	"models/player/custom_player/legacy/ctm_st6_variante.mdl",
	"models/player/custom_player/legacy/ctm_st6_variantk.mdl",
	"models/player/custom_player/legacy/ctm_fbi_variantf.mdl",
	"models/player/custom_player/legacy/ctm_sas_variantf.mdl",
	"models/player/custom_player/legacy/ctm_fbi_variantg.mdl",
	"models/player/custom_player/legacy/ctm_st6_variantg.mdl",
	"models/player/custom_player/legacy/ctm_st6_variantm.mdl",
	"models/player/custom_player/legacy/ctm_st6_varianti.mdl",
	"models/player/custom_player/legacy/ctm_fbi_variantb.mdl"
};

Memory memory;

std::unordered_map <std::string, int> SkinChanger::model_indexes;
std::unordered_map <std::string, int> SkinChanger::player_model_indexes;

std::vector <SkinChanger::PaintKit> SkinChanger::skinKits;
std::vector <SkinChanger::PaintKit> SkinChanger::gloveKits;
std::vector <SkinChanger::PaintKit> SkinChanger::displayKits;

static std::unordered_map <std::string_view, const char*> iconOverrides;

static void erase_override_if_exists_by_index(const int definition_index) noexcept
{
    if (auto original_item = game_data::get_weapon_info(definition_index)) 
	{
        if (!original_item->icon)
            return;

        if (const auto override_entry = iconOverrides.find(original_item->icon); override_entry != end(iconOverrides))
            iconOverrides.erase(override_entry);
    }
}

static void apply_config_on_attributable_item(attributableitem_t* item, const item_setting* config, const unsigned xuid_low) noexcept
{
    item->m_iItemIDHigh() = -1; //-V522
    item->m_iAccountID() = xuid_low;
	item->m_flFallbackWear() = config->wear;

    if (config->quality)
        item->m_iEntityQuality() = config->quality;

    if (config->custom_name[0])
        strcpy_s(item->m_szCustomName(), sizeof(config->custom_name), config->custom_name);

    if (config->paintKit)
        item->m_nFallbackPaintKit() = config->paintKit;

    if (config->seed)
        item->m_nFallbackSeed() = config->seed;

    if (config->stat_trak)
        item->m_nFallbackStatTrak() = config->stat_trak;

    auto& definition_index = item->m_iItemDefinitionIndex();

    if (config->definition_override_index && config->definition_override_index != definition_index)
    {
        if (auto replacement_item = game_data::get_weapon_info(config->definition_override_index)) 
		{
            auto old_definition_index = definition_index;
            definition_index = config->definition_override_index;

			if (SkinChanger::model_indexes.find(replacement_item->model) == SkinChanger::model_indexes.end())
				SkinChanger::model_indexes.emplace(replacement_item->model, m_modelinfo()->GetModelIndex(replacement_item->model));

            item->set_model_index(SkinChanger::model_indexes.at(replacement_item->model));
            item->PreDataUpdate(0);

            if (old_definition_index) 
                if (auto original_item = game_data::get_weapon_info(old_definition_index); original_item && original_item->icon && replacement_item->icon)
                    iconOverrides[original_item->icon] = replacement_item->icon;
        }
    } 
	else
        erase_override_if_exists_by_index(definition_index);
}

static auto get_wearable_create_fn() -> CreateClientClassFn
{
	auto classes = m_client()->GetAllClasses();

	while (classes->m_ClassID != CEconWearable)
		classes = classes->m_pNext;

	return classes->m_pCreateFn;
}

static attributableitem_t* make_glove(int entry, int serial) noexcept
{
	get_wearable_create_fn()(entry, serial);
	auto glove = static_cast <attributableitem_t*> (m_entitylist()->GetClientEntity(entry));

	if (!glove)
		return nullptr;

	static auto Fn = util::find_pattern(crypt_str("client.dll"), crypt_str("\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1"), crypt_str("xxxxxxxxxxxx"));
	static auto set_abs_origin = reinterpret_cast <void(__thiscall*)(void*, const Vector&)> (Fn);

	set_abs_origin(glove, Vector(16384.0f, 16384.0f, 16384.0f));
	return glove;
}

static float last_skins_update = 0.0f;

static void post_data_update_start(player_t* local) noexcept
{
    player_info_t player_info;

    if (!m_engine()->GetPlayerInfo(local->EntIndex(), &player_info))
        return;

	static auto glove_handle = CBaseHandle(0);

	auto wearables = local->m_hMyWearables();
	auto glove_config = get_by_definition_index(GLOVE_T_SIDE);
	auto glove = reinterpret_cast <attributableitem_t*> (m_entitylist()->GetClientEntityFromHandle(wearables[0]));

	if (!glove)
	{
		auto our_glove = reinterpret_cast <attributableitem_t*> (m_entitylist()->GetClientEntityFromHandle(glove_handle));

		if (our_glove)
		{
			wearables[0] = glove_handle;
			glove = our_glove;
		}
	}

	if (!local->is_alive())
	{
		if (glove)
		{
			glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
			glove->GetClientNetworkable()->Release();
		}

		return;
	}

	if (glove_config && glove_config->definition_override_index)
	{
		if (!glove)
		{
			auto entry = m_entitylist()->GetHighestEntityIndex() + 1;
			auto serial = rand() % 0x1000;

			glove = make_glove(entry, serial);
			wearables[0] = entry | serial << 16;
			glove_handle = wearables[0];
		}

		*reinterpret_cast <int*> (uintptr_t(glove) + 0x64) = -1;
		apply_config_on_attributable_item(glove, glove_config, player_info.xuid_low);
	}
	
	auto weapons = local->m_hMyWeapons();

	for (auto weapon_handle = 0; weapons[weapon_handle].IsValid(); weapon_handle++) 
	{
		auto weapon = (weapon_t*)m_entitylist()->GetClientEntityFromHandle(weapons[weapon_handle]); //-V807

		if (!weapon)
			continue;

		auto& definition_index = weapon->m_iItemDefinitionIndex();

		if (auto active_conf = get_by_definition_index(is_knife(definition_index) ? WEAPON_KNIFE : definition_index))
			apply_config_on_attributable_item(weapon, active_conf, player_info.xuid_low);
		else
			erase_override_if_exists_by_index(definition_index);
	}

	auto view_model = (weapon_t*)m_entitylist()->GetClientEntityFromHandle(local->m_hViewModel());

    if (!view_model)
        return;

    auto view_model_weapon = (weapon_t*)m_entitylist()->GetClientEntityFromHandle(view_model->weapon());

    if (!view_model_weapon)
        return;

    auto override_info = game_data::get_weapon_info(view_model_weapon->m_iItemDefinitionIndex());

    if (!override_info)
        return;

    auto world_model = (weapon_t*)m_entitylist()->GetClientEntityFromHandle(view_model_weapon->m_hWeaponWorldModel());

    if (!world_model)
        return;

	if (SkinChanger::model_indexes.find(override_info->model) == SkinChanger::model_indexes.end())
		SkinChanger::model_indexes.emplace(override_info->model, m_modelinfo()->GetModelIndex(override_info->model));

	view_model->m_nModelIndex() = SkinChanger::model_indexes.at(override_info->model);
    world_model->m_nModelIndex() = SkinChanger::model_indexes.at(override_info->model) + 1;
}

static bool UpdateRequired = false;
static bool hudUpdateRequired = false;

static constexpr void updateHud() noexcept
{
    if (auto hud_weapons = memory.findHudElement(memory.hud, crypt_str("CCSGO_HudWeaponSelection")) - 0x28)
        for (auto i = 0; i < *(hud_weapons + 0x20); i++)
            i = memory.clearHudWeapon(hud_weapons, i);
    
    hudUpdateRequired = false;
}

void SkinChanger::run(ClientFrameStage_t stage) noexcept
{
	if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		return;

	if (!g_ctx.local())
		return;

	post_data_update_start(g_ctx.local());

	if (!g_ctx.local()->is_alive()) //-V807
	{
		UpdateRequired = false;
		hudUpdateRequired = false;
		return;
	}

	static auto backup_model_index = -1;

	if (g_cfg.player.enable)
	{
		const char** player_model_index = nullptr;
		auto player_model = 0;

		switch (g_ctx.local()->m_iTeamNum())
		{
		case 2:
			player_model_index = player_model_index_t;
			player_model = g_cfg.player.player_model_t;
			break;
		case 3:
			player_model_index = player_model_index_ct;
			player_model = g_cfg.player.player_model_ct;
			break;
		}

		if (player_model)
		{
			if (!g_ctx.globals.backup_model)
			{
				auto model = g_ctx.local()->GetModel();

				if (model)
				{
					auto studio_model = m_modelinfo()->GetStudioModel(model);

					if (studio_model)
					{
						auto name = crypt_str("models/") + (std::string)studio_model->szName;
						backup_model_index = m_modelinfo()->GetModelIndex(name.c_str());
					}
				}
			}

			if (SkinChanger::player_model_indexes.find(player_model_index[player_model - 1]) == SkinChanger::player_model_indexes.end()) //-V522
				SkinChanger::player_model_indexes.emplace(player_model_index[player_model - 1], m_modelinfo()->GetModelIndex(player_model_index[player_model - 1]));

			g_ctx.local()->set_model_index(SkinChanger::player_model_indexes[player_model_index[player_model - 1]]);
			g_ctx.globals.backup_model = true;
		}
		else if (g_ctx.globals.backup_model)
		{
			g_ctx.local()->set_model_index(backup_model_index);
			g_ctx.globals.backup_model = false;
		}
	}

	if (UpdateRequired)
	{
		UpdateRequired = false;
		hudUpdateRequired = true;

		m_clientstate()->iDeltaTick = -1;
		g_ctx.globals.updating_skins = true;
	}
	else if (hudUpdateRequired && !g_ctx.globals.updating_skins)
	{
		hudUpdateRequired = false;
		updateHud();
	}
}

void SkinChanger::scheduleHudUpdate() noexcept
{
	if (!g_ctx.local()->is_alive())
		return;

	if (m_globals()->m_realtime - last_skins_update < 1.0f)
		return;

	UpdateRequired = true;
	last_skins_update = m_globals()->m_realtime;
}

void SkinChanger::overrideHudIcon(IGameEvent* event) noexcept
{
	if (auto iconOverride = iconOverrides[event->GetString(crypt_str("weapon"))])
		event->SetString(crypt_str("weapon"), iconOverride);
}