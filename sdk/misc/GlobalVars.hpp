#pragma once

class CGlobalVarsBase
{
public:
	float     m_realtime;                     // 0x0000
	int       m_framecount;                   // 0x0004
	float     m_absoluteframetime;            // 0x0008
	float     m_absoluteframestarttimestddev; // 0x000C
	float     m_curtime;                      // 0x0010
	float     m_frametime;                    // 0x0014
	int       m_maxclients;                   // 0x0018
	int       m_tickcount;                    // 0x001C
	float     m_intervalpertick;              // 0x0020
	float     m_interpolation_amount;         // 0x0024
	int       m_simticksthisframe;            // 0x0028
	int       m_networkprotocol;              // 0x002C
	void*     m_savedata;                     // 0x0030
	bool      m_client;						  // 0x0031
	bool      m_remoteclient;                 // 0x0032
	int		  m_timestampnetworkingbase;	  // 0x0036
	int		  m_timestamprandomizewindow;	  // 0x003A
};