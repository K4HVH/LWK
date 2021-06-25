// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"

using PacketStart_t = void(__thiscall*)(void*, int, int);

void __fastcall hooks::hooked_packetstart(void* ecx, void* edx, int incoming, int outgoing)
{
	static auto original_fn = clientstate_hook->get_func_address <PacketStart_t> (5);
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

    if (!g_ctx.available())
        return original_fn(ecx, incoming, outgoing);

	if (!g_ctx.local()->is_alive())
		return original_fn(ecx, incoming, outgoing);
   
    if (g_ctx.globals.commands.empty())
        return original_fn(ecx, incoming, outgoing);

    if (m_gamerules()->m_bIsValveDS())
        return original_fn(ecx, incoming, outgoing);

    for (auto it = g_ctx.globals.commands.rbegin(); it != g_ctx.globals.commands.rend(); ++it)
    {
        if (!it->is_outgoing)
            continue;

        if (it->command_number == outgoing || outgoing > it->command_number && (!it->is_used || it->previous_command_number == outgoing))
        {
            it->previous_command_number = outgoing;
            it->is_used = true;
            original_fn(ecx, incoming, outgoing);
            break;
        }
    }

    auto result = false;

    for (auto it = g_ctx.globals.commands.begin(); it != g_ctx.globals.commands.end();) 
    {
        if (outgoing == it->command_number || outgoing == it->previous_command_number)
            result = true;

        if (outgoing > it->command_number && outgoing > it->previous_command_number) 
            it = g_ctx.globals.commands.erase(it);
        else 
            ++it;
    }

    if (!result)
        original_fn(ecx, incoming, outgoing);
}

using PacketEnd_t = void(__thiscall*)(void*);

void __fastcall hooks::hooked_packetend(void* ecx, void* edx)
{
	static auto original_fn = clientstate_hook->get_func_address <PacketEnd_t>(6);
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

	if (!g_ctx.local()->is_alive())  //-V807
	{
		g_ctx.globals.data.clear();
		return original_fn(ecx);
	}

	if (*(int*)((uintptr_t)ecx + 0x164) == *(int*)((uintptr_t)ecx + 0x16C))
	{
		auto ack_cmd = *(int*)((uintptr_t)ecx + 0x4D2C);
		auto correct = std::find_if(g_ctx.globals.data.begin(), g_ctx.globals.data.end(),
			[&ack_cmd](const correction_data& other_data)
			{
				return other_data.command_number == ack_cmd;
			}
		);

		auto netchannel = m_engine()->GetNetChannelInfo();

		if (netchannel && correct != g_ctx.globals.data.end())
		{
			if (g_ctx.globals.last_velocity_modifier > g_ctx.local()->m_flVelocityModifier() + 0.1f)
			{
				auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

				if (!weapon || weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER && !weapon->is_grenade()) //-V648
				{
					for (auto& number : g_ctx.globals.choked_number)
					{
						auto cmd = &m_input()->m_pCommands[number % MULTIPLAYER_BACKUP];
						auto verified = &m_input()->m_pVerifiedCommands[number % MULTIPLAYER_BACKUP];

						if (cmd->m_buttons & (IN_ATTACK | IN_ATTACK2))
						{
							cmd->m_buttons &= ~IN_ATTACK;

							verified->m_cmd = *cmd;
							verified->m_crc = cmd->GetChecksum();
						}
					}
				}
			}

			g_ctx.globals.last_velocity_modifier = g_ctx.local()->m_flVelocityModifier();
		}
	}

	return original_fn(ecx);
}

void __fastcall hooks::hooked_checkfilecrcswithserver(void* ecx, void* edx)
{
    
}

bool __fastcall hooks::hooked_loosefileallowed(void* ecx, void* edx)
{
    return true;
}