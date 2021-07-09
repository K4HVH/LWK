// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\misc\prediction_system.h"
#include "..\..\cheats\lagcompensation\local_animations.h"
#include "..\..\cheats\misc\misc.h"
#include "..\..\cheats\misc\logs.h"

using RunCommand_t = void(__thiscall*)(void*, player_t*, CUserCmd*, IMoveHelper*);

void __fastcall hooks::hooked_runcommand(void* ecx, void* edx, player_t* player, CUserCmd* m_pcmd, IMoveHelper* move_helper)
{
	static auto original_fn = prediction_hook->get_func_address <RunCommand_t> (19);
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

	if (!player || player != g_ctx.local())
		return original_fn(ecx, player, m_pcmd, move_helper);

	if (!m_pcmd)
		return original_fn(ecx, player, m_pcmd, move_helper);

	if (m_pcmd->m_tickcount > m_globals()->m_tickcount + 72) //-V807
	{
		m_pcmd->m_predicted = true;
		player->set_abs_origin(player->m_vecOrigin());

		if (m_globals()->m_frametime > 0.0f && !m_prediction()->EnginePaused)
			++player->m_nTickBase();

		return;
	}

	if (g_cfg.ragebot.enable && player->is_alive())
	{
		auto weapon = player->m_hActiveWeapon().Get();

		if (weapon)
		{
			static float tickbase_records[MULTIPLAYER_BACKUP];
			static bool in_attack[MULTIPLAYER_BACKUP];
			static bool can_shoot[MULTIPLAYER_BACKUP];

			tickbase_records[m_pcmd->m_command_number % MULTIPLAYER_BACKUP] = player->m_nTickBase();
			in_attack[m_pcmd->m_command_number % MULTIPLAYER_BACKUP] = m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2;
			can_shoot[m_pcmd->m_command_number % MULTIPLAYER_BACKUP] = weapon->can_fire(false);

			if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER) 
			{
				auto postpone_fire_ready_time = FLT_MAX;
				auto tickrate = (int)(1.0f / m_globals()->m_intervalpertick);

				if (tickrate >> 1 > 1)
				{
					auto command_number = m_pcmd->m_command_number - 1;
					auto shoot_number = 0;
			
					for (auto i = 1; i < tickrate >> 1; ++i)
					{
						shoot_number = command_number;

						if (!in_attack[command_number % MULTIPLAYER_BACKUP] || !can_shoot[command_number % MULTIPLAYER_BACKUP])
							break;

						--command_number;
					}

					if (shoot_number)
					{
						auto tick = 1 - (int)(-0.03348f / m_globals()->m_intervalpertick);

						if (m_pcmd->m_command_number - shoot_number >= tick)
							postpone_fire_ready_time = TICKS_TO_TIME(tickbase_records[(tick + shoot_number) % MULTIPLAYER_BACKUP]) + 0.2f;
					}
				}

				weapon->m_flPostponeFireReadyTime() = postpone_fire_ready_time;
			}
		}

		auto backup_velocity_modifier = player->m_flVelocityModifier();

		player->m_flVelocityModifier() = g_ctx.globals.last_velocity_modifier;
		original_fn(ecx, player, m_pcmd, move_helper);

		if (!g_ctx.globals.in_createmove)
			player->m_flVelocityModifier() = backup_velocity_modifier;
	}
	else
		return original_fn(ecx, player, m_pcmd, move_helper);
}

using InPrediction_t = bool(__thiscall*)(void*);

bool __stdcall hooks::hooked_inprediction()
{
	static auto original_fn = prediction_hook->get_func_address <InPrediction_t> (14);
	static auto maintain_sequence_transitions = util::FindSignature(crypt_str("client.dll"), crypt_str("84 C0 74 17 8B 87"));

	if ((g_cfg.ragebot.enable || g_cfg.legitbot.enabled) && g_ctx.globals.setuping_bones && (uintptr_t)_ReturnAddress() == maintain_sequence_transitions)
		return true;

	return original_fn(m_prediction());
}

using WriteUsercmdDeltaToBuffer_t = bool(__thiscall*)(void*, int, void*, int, int, bool);
void WriteUser—md(void* buf, CUserCmd* incmd, CUserCmd* outcmd);

bool __fastcall hooks::hooked_writeusercmddeltatobuffer(void* ecx, void* edx, int slot, bf_write* buf, int from, int to, bool is_new_command)
{
	static auto original_fn = client_hook->get_func_address <WriteUsercmdDeltaToBuffer_t> (24);

	if (!g_ctx.globals.tickbase_shift)
		return original_fn(ecx, slot, buf, from, to, is_new_command);

	if (from != -1)
		return true;

	auto final_from = -1;

	uintptr_t frame_ptr;
	__asm mov frame_ptr, ebp;

	auto backup_commands = reinterpret_cast <int*> (frame_ptr + 0xFD8);
	auto new_commands = reinterpret_cast <int*> (frame_ptr + 0xFDC);

	auto newcmds = *new_commands;
	auto shift = g_ctx.globals.tickbase_shift;

	g_ctx.globals.tickbase_shift = 0;
	*backup_commands = 0;

	auto choked_modifier = newcmds + shift;

	if (choked_modifier > 62)
		choked_modifier = 62;

	*new_commands = choked_modifier;

	auto next_cmdnr = m_clientstate()->iChokedCommands + m_clientstate()->nLastOutgoingCommand + 1;
	auto final_to = next_cmdnr - newcmds + 1;

	if (final_to <= next_cmdnr)
	{
		while (original_fn(ecx, slot, buf, final_from, final_to, true))
		{
			final_from = final_to++;

			if (final_to > next_cmdnr)
				goto next_cmd;
		}

		return false;
	}
next_cmd:

	auto user_cmd = m_input()->GetUserCmd(final_from);

	if (!user_cmd)
		return true;

	CUserCmd to_cmd;
	CUserCmd from_cmd;

	from_cmd = *user_cmd;
	to_cmd = from_cmd;

	to_cmd.m_command_number++;
	to_cmd.m_tickcount += 200;

	if (newcmds > choked_modifier)
		return true;

	for (auto i = choked_modifier - newcmds + 1; i > 0; --i)
	{
		WriteUser—md(buf, &to_cmd, &from_cmd);

		from_cmd = to_cmd;
		to_cmd.m_command_number++;
		to_cmd.m_tickcount++;
	}

	return true;
}

void WriteUser—md(void* buf, CUserCmd* incmd, CUserCmd* outcmd)
{
	using WriteUserCmd_t = void(__fastcall*)(void*, CUserCmd*, CUserCmd*);
	static auto Fn = (WriteUserCmd_t)util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 51 53 56 8B D9"));

	__asm
	{
		mov     ecx, buf
		mov     edx, incmd
		push    outcmd
		call    Fn
		add     esp, 4
	}
}