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
	static auto original_fn = prediction_hook->get_func_address <RunCommand_t>(19);
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
	static auto original_fn = prediction_hook->get_func_address <InPrediction_t>(14);
	static auto maintain_sequence_transitions = (void*)util::FindSignature(crypt_str("client.dll"), crypt_str("84 C0 74 17 8B 87"));
	static auto setupbones_timing = (void*)util::FindSignature(crypt_str("client.dll"), crypt_str("84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05"));
	static void* calcplayerview_return = (void*)util::FindSignature(crypt_str("client.dll"), crypt_str("84 C0 75 0B 8B 0D ? ? ? ? 8B 01 FF 50 4C"));

	if (maintain_sequence_transitions && g_ctx.globals.setuping_bones && _ReturnAddress() == maintain_sequence_transitions)
		return true;

	if (setupbones_timing && _ReturnAddress() == setupbones_timing)
		return false;

	if (m_engine()->IsInGame()) {
		if (_ReturnAddress() == calcplayerview_return)
			return true;
	}

	return original_fn(m_prediction());
}

typedef void(__vectorcall* cl_move_t)(float, bool);

void __vectorcall hooks::hooked_clmove(float accumulated_extra_samples, bool bFinalTick)
{
	if (g_ctx.local()) {
		g_ctx.globals.current_tickcount = m_globals()->m_tickcount;
		if (m_clientstate() && m_clientstate()->pNetChannel)
			g_ctx.globals.out_sequence_nr = m_clientstate()->pNetChannel->m_nOutSequenceNr;
		else
			g_ctx.globals.out_sequence_nr = 0;

	}

	(cl_move_t(hooks::original_clmove))(accumulated_extra_samples, bFinalTick);

	if (g_cfg.ragebot.enable)
	{
		if (!g_ctx.local()
			|| !g_ctx.local()->is_alive()
			|| g_ctx.local()->m_fFlags() & 0x40) // fl frozen. Aka engine paused.
			return;
		else if (m_clientstate()) {
			INetChannel* net_channel = m_clientstate()->pNetChannel;
			if (net_channel && !(m_clientstate()->iChokedCommands % 4)) {
				const auto current_choke = net_channel->m_nChokedPackets;
				const auto out_sequence_nr = net_channel->m_nOutSequenceNr;

				net_channel->m_nChokedPackets = 0;
				net_channel->m_nOutSequenceNr = g_ctx.globals.out_sequence_nr;

				net_channel->send_datagram();

				net_channel->m_nOutSequenceNr = out_sequence_nr;
				net_channel->m_nChokedPackets = current_choke;
			}
		}
	}
}

using WriteUsercmdDeltaToBuffer_t = bool(__thiscall*)(void*, int, void*, int, int, bool);
void WriteUser—md(void* buf, CUserCmd* incmd, CUserCmd* outcmd);

bool __fastcall hooks::hooked_writeusercmddeltatobuffer(void* ecx_, void* edx_, int nslot, bf_write* buf, int from, int to, bool isnewcmd)
{
	static auto sendmovecall = (void*)util::FindSignature(crypt_str("engine.dll"), crypt_str("84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84"));
	static auto original_fn = client_hook->get_func_address <WriteUsercmdDeltaToBuffer_t>(24);

	if (!g_ctx.globals.tickbase_shift)
		return original_fn(ecx_, nslot, buf, from, to, isnewcmd);

	uintptr_t stackbase;
	__asm mov stackbase, ebp;

	auto m_pnNewCmds = reinterpret_cast <int*> (stackbase + 0xFCC);

	int m_nNewCmds = *m_pnNewCmds;

	int m_nTickbase = g_ctx.globals.tickbase_shift;

	int m_nTotalNewCmds = min(m_nNewCmds + abs(m_nTickbase), 62);

	if (from != -1)
		return true;

	int m_nNewCommands = m_nTotalNewCmds;
	int m_nBackupCommands = 0;
	int m_nNextCmd = m_clientstate()->nLastOutgoingCommand + m_clientstate()->iChokedCommands + 1;

	if (to > m_nNextCmd)
	{
	Run:
		CUserCmd* m_pCmd = m_input()->GetUserCmd(from);
		if (m_pCmd)
		{
			CUserCmd m_nToCmd = *m_pCmd, m_nFromCmd = *m_pCmd;
			m_nToCmd.m_command_number++;
			m_nToCmd.m_tickcount += m_globals()->m_tickcount + 2 * m_globals()->m_tickcount;
			for (int i = m_nNewCmds; i <= m_nTotalNewCmds; i++)
			{
				int m_shift = m_nTotalNewCmds - m_nNewCmds + 1;

				do
				{
					//       WriteUserCmd(buf, &m_nToCmd, &m_nFromCmd);
					m_nFromCmd.m_buttons = m_nToCmd.m_buttons;
					m_nFromCmd.m_viewangles.x = m_nToCmd.m_viewangles.x;
					m_nFromCmd.m_impulse = m_nToCmd.m_impulse;
					m_nFromCmd.m_weaponselect = m_nToCmd.m_weaponselect;
					m_nFromCmd.m_aimdirection.y = m_nToCmd.m_aimdirection.y;
					m_nFromCmd.m_weaponsubtype = m_nToCmd.m_weaponsubtype;
					m_nFromCmd.m_upmove = m_nToCmd.m_upmove;
					m_nFromCmd.m_random_seed = m_nToCmd.m_random_seed;
					m_nFromCmd.m_mousedx = m_nToCmd.m_mousedx;
					m_nFromCmd.pad_0x4C[3] = m_nToCmd.pad_0x4C[3];
					m_nFromCmd.m_command_number = m_nToCmd.m_command_number;
					m_nFromCmd.m_tickcount = m_nToCmd.m_tickcount;
					m_nFromCmd.m_mousedy = m_nToCmd.m_mousedy;
					m_nFromCmd.pad_0x4C[19] = m_nToCmd.pad_0x4C[19];
					m_nFromCmd.m_predicted = m_nToCmd.m_predicted;
					m_nFromCmd.pad_0x4C[23] = m_nToCmd.pad_0x4C[23];
					m_nToCmd.m_command_number++;
					m_nToCmd.m_tickcount++;
					--m_shift;
				} while (m_shift);
			}
			return true;
		}
	}
	else
	{
		while (original_fn(ecx_, nslot, buf, from, to, true))
		{
			from = to++;

			if (to > m_nNextCmd)
				goto Run;

		}
		return false;
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