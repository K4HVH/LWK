#pragma once
#include "..\..\includes.hpp"

class c_dormant_esp : public singleton <c_dormant_esp>
{
public:
	void start();

	bool adjust_sound(player_t* player);
	void setup_adjust(player_t* player, SndInfo_t& sound);
	bool valid_sound(SndInfo_t& sound);

	struct SoundPlayer
	{
		void reset(bool store_data = false, const Vector& origin = ZERO, int flags = 0)
		{
			if (store_data)
			{
				m_iReceiveTime = m_globals()->m_realtime;
				m_vecOrigin = origin;
				m_nFlags = flags;
			}
			else
			{
				m_iReceiveTime = 0.0f;
				m_vecOrigin.Zero();
				m_nFlags = 0;
			}
		}

		void Override(SndInfo_t& sound) 
		{
			m_iReceiveTime = m_globals()->m_realtime;
			m_vecOrigin = *sound.m_pOrigin;
		}

		float m_iReceiveTime = 0.0f;
		Vector m_vecOrigin = ZERO;
		int m_nFlags = 0;
	} m_cSoundPlayers[65];

	CUtlVector <SndInfo_t> m_utlvecSoundBuffer;
	CUtlVector <SndInfo_t> m_utlCurSoundList;
};