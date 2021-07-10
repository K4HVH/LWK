// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "prediction_system.h"
#include <cheats/lagcompensation/animation_system.h>

void engineprediction::store_netvars()
{
    auto data = &netvars_data[m_clientstate()->iCommandAck % MULTIPLAYER_BACKUP];

    data->tickbase = g_ctx.local()->m_nTickBase();
    data->m_velocity_modifier = g_ctx.local()->m_flVelocityModifier();
    data->m_velocity = g_ctx.local()->m_vecVelocity();
    data->m_aimPunchAngle = g_ctx.local()->m_aimPunchAngle();
    data->m_aimPunchAngleVel = g_ctx.local()->m_aimPunchAngleVel();
    data->m_viewPunchAngle = g_ctx.local()->m_viewPunchAngle();
    data->m_vecViewOffset = g_ctx.local()->m_vecViewOffset();
    data->m_duckAmount = g_ctx.local()->m_flDuckAmount();
    data->index = g_ctx.local()->EntIndex();
    data->m_duckSpeed = g_ctx.local()->m_flDuckSpeed();

}

void engineprediction::restore_netvars()
{
    auto data = &netvars_data[(m_clientstate()->iCommandAck - 0) % MULTIPLAYER_BACKUP];

    if (data->tickbase != g_ctx.local()->m_nTickBase())
        return;
    int repredict = 0;
    auto aim_punch_angle_delta = g_ctx.local()->m_aimPunchAngle() - data->m_aimPunchAngle;
    auto aim_punch_angle_vel_delta = g_ctx.local()->m_aimPunchAngleVel() - data->m_aimPunchAngleVel;
    auto view_punch_angle_delta = g_ctx.local()->m_viewPunchAngle() - data->m_viewPunchAngle;
    auto view_offset_delta = g_ctx.local()->m_vecViewOffset() - data->m_vecViewOffset;
    auto duck_amount = g_ctx.local()->m_flDuckAmount() - data->m_duckAmount;
    auto m_velocity_modifier = g_ctx.local()->m_flVelocityModifier() - data->m_velocity_modifier;
    const auto velocity_diff = data->m_velocity - g_ctx.local()->m_vecVelocity();

    if (fabs(aim_punch_angle_delta.x) > 0.03425f && fabs(aim_punch_angle_delta.y) > 0.03425f && fabs(aim_punch_angle_delta.z) > 0.03425f)
        g_ctx.local()->m_aimPunchAngle() = data->m_aimPunchAngle;

    if (fabs(aim_punch_angle_vel_delta.x) > 0.03425f && fabs(aim_punch_angle_vel_delta.y) > 0.03425f && fabs(aim_punch_angle_vel_delta.z) > 0.03425f)
        g_ctx.local()->m_aimPunchAngleVel() = data->m_aimPunchAngleVel;

    if (fabs(view_punch_angle_delta.x) > 0.03425f && fabs(view_punch_angle_delta.y) > 0.03425f && fabs(view_punch_angle_delta.z) > 0.03425f)
        g_ctx.local()->m_viewPunchAngle() = data->m_viewPunchAngle;

    if (fabs(view_offset_delta.x) > 0.03425f && fabs(view_offset_delta.y) > 0.03425f && fabs(view_offset_delta.z) > 0.03425f)
        g_ctx.local()->m_vecViewOffset() = data->m_vecViewOffset;

    if (std::abs(velocity_diff.x) <= 0.03125f && std::abs(velocity_diff.y) <= 0.03125f && std::abs(velocity_diff.z) <= 0.03125f)
        g_ctx.local()->m_vecVelocity() = data->m_velocity;

    if (std::abs(g_ctx.local()->m_flVelocityModifier() - data->m_velocity_modifier) <= 0.00625f)
        g_ctx.local()->m_flVelocityModifier() = data->m_velocity_modifier;

    if (std::abs(g_ctx.local()->m_nTickBase() - data->tickbase) <= 0.00625f)
        g_ctx.local()->m_nTickBase() = data->tickbase;

    if (std::abs(g_ctx.local()->EntIndex() - data->index) <= 0.00625f)
        g_ctx.local()->EntIndex() != data->index;

    if (fabs(duck_amount) > 0.03425f)
    {
        g_ctx.local()->m_flDuckAmount() = data->m_duckAmount;
        g_ctx.local()->m_flDuckSpeed() = data->m_duckSpeed;
    }
}

void engineprediction::setup()
{
    if (prediction_data.prediction_stage != SETUP)
        return;

    backup_data.flags = g_ctx.local()->m_fFlags();
    backup_data.velocity = g_ctx.local()->m_vecVelocity();

    prediction_data.old_curtime = m_globals()->m_curtime;
    prediction_data.old_frametime = m_globals()->m_frametime;

    m_globals()->m_curtime++;
    m_globals()->m_frametime++;

    m_globals()->m_curtime = TICKS_TO_TIME(g_ctx.globals.fixed_tickbase);
    m_globals()->m_frametime = m_prediction()->EnginePaused ? 0.f : m_globals()->m_intervalpertick;

    prediction_data.prediction_stage = PREDICT;
}

void engineprediction::predict(CUserCmd* m_pcmd)
{
    if (prediction_data.prediction_stage != PREDICT)
        return;

    const auto backup_footsteps = m_cvar()->FindVar(crypt_str("sv_footsteps"))->GetInt();

    if (m_clientstate()->iDeltaTick > 0)
        m_prediction()->Update(m_clientstate()->iDeltaTick, true, m_clientstate()->nLastCommandAck, m_clientstate()->nLastOutgoingCommand + m_clientstate()->iChokedCommands);



    if (!prediction_data.prediction_random_seed)
        prediction_data.prediction_random_seed = *reinterpret_cast <int**> (util::FindSignature(crypt_str("client.dll"), crypt_str("A3 ? ? ? ? 66 0F 6E 86")) + 0x1);

    *prediction_data.prediction_random_seed = MD5_PseudoRandom(m_pcmd->m_command_number) & INT_MAX;

    if (!prediction_data.prediction_player)
        prediction_data.prediction_player = *reinterpret_cast <int**> (util::FindSignature(crypt_str("client.dll"), crypt_str("89 35 ? ? ? ? F3 0F 10 48")) + 0x2);
    if (!prediction_data.prediction_player)

        *prediction_data.prediction_player = reinterpret_cast <int> (g_ctx.local());

    m_movehelper()->set_host(g_ctx.local());
    m_gamemovement()->StartTrackPredictionErrors(g_ctx.local()); //-V807

    static auto m_nImpulse = util::find_in_datamap(g_ctx.local()->GetPredDescMap(), crypt_str("m_nImpulse"));
    static auto m_nButtons = util::find_in_datamap(g_ctx.local()->GetPredDescMap(), crypt_str("m_nButtons"));
    static auto m_afButtonLast = util::find_in_datamap(g_ctx.local()->GetPredDescMap(), crypt_str("m_afButtonLast"));
    static auto m_afButtonPressed = util::find_in_datamap(g_ctx.local()->GetPredDescMap(), crypt_str("m_afButtonPressed"));
    static auto m_afButtonReleased = util::find_in_datamap(g_ctx.local()->GetPredDescMap(), crypt_str("m_afButtonReleased"));

    if (m_pcmd->m_impulse)
        *reinterpret_cast<uint32_t*>(uint32_t(g_ctx.local()) + m_nImpulse) = m_pcmd->m_impulse;

    int* buttons = reinterpret_cast<int*>(uint32_t(g_ctx.local()) + m_nButtons);
    const int buttonsChanged = m_pcmd->m_buttons ^ *buttons;

    *reinterpret_cast<int*>(uint32_t(g_ctx.local()) + m_afButtonLast) = *buttons;
    *reinterpret_cast<int*>(uint32_t(g_ctx.local()) + m_nButtons) = m_pcmd->m_buttons;
    *reinterpret_cast<int*>(uint32_t(g_ctx.local()) + m_afButtonPressed) = buttonsChanged & m_pcmd->m_buttons;
    *reinterpret_cast<int*>(uint32_t(g_ctx.local()) + m_afButtonReleased) = buttonsChanged & ~m_pcmd->m_buttons;

    CMoveData move_data;
    move_data.m_flForwardMove = m_pcmd->m_forwardmove;
    move_data.m_flSideMove = m_pcmd->m_sidemove;
    move_data.m_flUpMove = m_pcmd->m_upmove;
    move_data.m_nButtons = m_pcmd->m_buttons;
    move_data.m_vecViewAngles = m_pcmd->m_viewangles;
    move_data.m_vecAngles = m_pcmd->m_viewangles;
    move_data.m_nImpulseCommand = m_pcmd->m_impulse;
    memset(&move_data, 0, sizeof(CMoveData));

    m_prediction()->SetupMove(g_ctx.local(), m_pcmd, m_movehelper(), &move_data);
    m_gamemovement()->ProcessMovement(g_ctx.local(), &move_data);
    m_prediction()->FinishMove(g_ctx.local(), m_pcmd, &move_data);

    m_gamemovement()->FinishTrackPredictionErrors(g_ctx.local());
    m_movehelper()->set_host(nullptr);


    const auto weapon = g_ctx.local()->m_hActiveWeapon().Get();
    if (!weapon) {
        weapon->get_spread();
        weapon->get_inaccuracy();
    }
    else {
        weapon->get_spread();
        weapon->get_inaccuracy();
    }

    weapon->update_accuracy_penality();

    auto viewmodel = g_ctx.local()->m_hViewModel().Get();

    if (viewmodel)
    {
        viewmodel_data.weapon = viewmodel->m_hWeapon().Get();

        viewmodel_data.viewmodel_index = viewmodel->m_nViewModelIndex();
        viewmodel_data.sequence = viewmodel->m_nSequence();
        viewmodel_data.animation_parity = viewmodel->m_nAnimationParity();

        viewmodel_data.cycle = viewmodel->m_flCycle();
        viewmodel_data.animation_time = viewmodel->m_flAnimTime();
    }
    prediction_data.prediction_stage = FINISH;
}

void engineprediction::finish()
{
    if (prediction_data.prediction_stage != FINISH)
        return;

    *prediction_data.prediction_random_seed = -1;
    *prediction_data.prediction_player = 0;

    m_globals()->m_curtime = prediction_data.old_curtime;
    m_globals()->m_frametime = prediction_data.old_frametime;
}