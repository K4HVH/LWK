/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#pragma once
#include <map>
#include <vector>

#include "../sdk/misc/Enums.hpp"

static auto is_knife(const int i) -> bool
{
    return (i >= WEAPON_KNIFE_BAYONET && i < GLOVE_STUDDED_BLOODHOUND) || i == WEAPON_KNIFE_T || i == WEAPON_KNIFE;
}

//extern const std::map<size_t, weapon_info> k_weapon_info;
namespace game_data
{
    // Stupid MSVC requires separate constexpr constructors for any initialization
    struct weapon_info
    {
        constexpr weapon_info(const char* model, const char* icon = nullptr) :
            model(model),
            icon(icon)
        {}

        const char* model;
        const char* icon;
    };

    struct weapon_name
    {
        constexpr weapon_name(const int definition_index, const char* name) :
            definition_index(definition_index),
            name(name)
        {}

        int definition_index = 0;
        const char* name = nullptr;
    };

    struct quality_name
    {
        constexpr quality_name(const int index, const char* name) :
            index(index),
            name(name)
        {}

        int index = 0;
        const char* name = nullptr;
    };

    const weapon_info* get_weapon_info(int defindex);

    constexpr weapon_name knife_names[]{
        {0, "Default"},
        {WEAPON_KNIFE_BAYONET, "Bayonet"},
        {WEAPON_KNIFE_CSS, "Classic Knife"},
        {WEAPON_KNIFE_SKELETON, "Skeleton Knife"},
        {WEAPON_KNIFE_OUTDOOR, "Nomad Knife"},
        {WEAPON_KNIFE_CORD, "Paracord Knife"},
        {WEAPON_KNIFE_CANIS, "Survival Knife"},
        {WEAPON_KNIFE_FLIP, "Flip Knife"},
        {WEAPON_KNIFE_GUT, "Gut Knife"},
        {WEAPON_KNIFE_KARAMBIT, "Karambit"},
        {WEAPON_KNIFE_M9_BAYONET, "M9 Bayonet"},
        {WEAPON_KNIFE_TACTICAL, "Huntsman Knife"},
        {WEAPON_KNIFE_FALCHION, "Falchion Knife"},
        {WEAPON_KNIFE_SURVIVAL_BOWIE, "Bowie Knife"},
        {WEAPON_KNIFE_BUTTERFLY, "Butterfly Knife"},
        {WEAPON_KNIFE_PUSH, "Shadow Daggers"},
        {WEAPON_KNIFE_URSUS, "Ursus Knife"},
        {WEAPON_KNIFE_GYPSY_JACKKNIFE, "Navaja Knife"},
        {WEAPON_KNIFE_STILETTO, "Stiletto Knife"},
        {WEAPON_KNIFE_WIDOWMAKER, "Talon Knife"}
    };


    constexpr weapon_name glove_names[]{
        {0, "Default"},
        {GLOVE_STUDDED_BLOODHOUND, "Bloodhound"},
        {GLOVE_T_SIDE, "Default (Terrorists)"},
        {GLOVE_CT_SIDE, "Default (Counter-Terrorists)"},
        {GLOVE_SPORTY, "Sporty"},
        {GLOVE_SLICK, "Slick"},
        {GLOVE_LEATHER_WRAP, "Handwrap"},
        {GLOVE_MOTORCYCLE, "Motorcycle"},
        {GLOVE_SPECIALIST, "Specialist"},
        {GLOVE_HYDRA, "Hydra"}
    };

    constexpr weapon_name weapon_names[]{
        {WEAPON_KNIFE, "Knife"},
        {GLOVE_T_SIDE, "Glove"},
        {WEAPON_AK47, "AK-47"},
        {WEAPON_AUG, "AUG"},
        {WEAPON_AWP, "AWP"},
        {WEAPON_CZ75A, "CZ75 Auto"},
        {WEAPON_DEAGLE, "Desert Eagle"},
        {WEAPON_ELITE, "Dual Berettas"},
        {WEAPON_FAMAS, "FAMAS"},
        {WEAPON_FIVESEVEN, "Five-SeveN"},
        {WEAPON_G3SG1, "G3SG1"},
        {WEAPON_GALILAR, "Galil AR"},
        {WEAPON_GLOCK, "Glock-18"},
        {WEAPON_M249, "M249"},
        {WEAPON_M4A1_SILENCER, "M4A1-S"},
        {WEAPON_M4A1, "M4A4"},
        {WEAPON_MAC10, "MAC-10"},
        {WEAPON_MAG7, "MAG-7"},
        {WEAPON_MP5SD, "MP5-SD"},
        {WEAPON_MP7, "MP7"},
        {WEAPON_MP9, "MP9"},
        {WEAPON_NEGEV, "Negev"},
        {WEAPON_NOVA, "Nova"},
        {WEAPON_HKP2000, "P2000"},
        {WEAPON_P250, "P250"},
        {WEAPON_P90, "P90"},
        {WEAPON_BIZON, "PP-Bizon"},
        {WEAPON_REVOLVER, "R8 Revolver"},
        {WEAPON_SAWEDOFF, "Sawed-Off"},
        {WEAPON_SCAR20, "SCAR-20"},
        {WEAPON_SSG08, "SSG 08"},
        {WEAPON_SG553, "SG 553"},
        {WEAPON_TEC9, "Tec-9"},
        {WEAPON_UMP45, "UMP-45"},
        {WEAPON_USP_SILENCER, "USP-S"},
        {WEAPON_XM1014, "XM1014"},
    };

    constexpr quality_name quality_names[]{
        {0, "Default"},
        {1, "Genuine"},
        {2, "Vintage"},
        {3, "Unusual"},
        {5, "Community"},
        {6, "Developer"},
        {7, "Self-Made"},
        {8, "Customized"},
        {9, "Strange"},
        {10, "Completed"},
        {12, "Tournament"}
    };
}
