#pragma once

#include <cstdint>

#include "..\math\Vector.hpp"
#include "..\misc\bf_write.h"

#define GenDefineVFunc(...) ( this, __VA_ARGS__ ); }
#define VFUNC( index, func, sig ) auto func { return call_virtual< sig >( this, index ) GenDefineVFunc

class CClockDriftMgr {
public:
	float m_ClockOffsets[ 17 ];   //0x0000
	uint32_t m_iCurClockOffset; //0x0044
	uint32_t m_nServerTick;     //0x0048
	uint32_t m_nClientTick;     //0x004C
}; 

class INetChannel
{
public:
	char pad_0x0000[ 0x18 ]; //0x0000
	__int32 m_nOutSequenceNr; //0x0018 
	__int32 m_nInSequenceNr; //0x001C 
	__int32 m_nOutSequenceNrAck; //0x0020 
	__int32 m_nOutReliableState; //0x0024 
	__int32 m_nInReliableState; //0x0028 
	__int32 m_nChokedPackets; //0x002C

	void Transmit(bool onlyreliable)
	{
		using Fn = bool(__thiscall*)(void*, bool);
		call_virtual<Fn>(this, 49)(this, onlyreliable);
	}

	void send_datagram()
	{
		using Fn = int(__thiscall*)(void*, void*);
		call_virtual<Fn>(this, 46)(this, 0);
	}

	void SetTimeOut(float seconds)
	{
		using Fn = void(__thiscall*)(void*, float);
		return call_virtual<Fn>(this, 4)(this, seconds);
	}

	int RequestFile(const char* filename)
	{
		using Fn = int(__thiscall*)(void*, const char*);
		return call_virtual<Fn>(this, 62)(this, filename);
	}
};

class INetMessage
{
public:
	virtual	~INetMessage() {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void	SetNetChannel(INetChannel * netchan) = 0; // netchannel this message is from/for
	virtual void	SetReliable(bool state) = 0;	// set to true if it's a reliable message

	virtual bool	Process(void) = 0; // calles the recently set handler to process this message

	virtual	bool	ReadFromBuffer(bf_read &buffer) = 0; // returns true if parsing was OK
	virtual	bool	WriteToBuffer(bf_write &buffer) = 0;	// returns true if writing was OK

	virtual bool	IsReliable(void) const = 0;  // true, if message needs reliable handling

	virtual int				GetType(void) const = 0; // returns module specific header tag eg svc_serverinfo
	virtual int				GetGroup(void) const = 0;	// returns net message group of this message
	virtual const char		*GetName(void) const = 0;	// returns network message name, eg "svc_serverinfo"
	virtual INetChannel		*GetNetChannel(void) const = 0;
	virtual const char		*ToString(void) const = 0; // returns a human readable string about message content
};

class CClientState
{
public:
	std::byte        pad0[0x9C];                //0x0000
	INetChannel* pNetChannel;            //0x009C
	int                iChallengeNr;            //0x00A0
	std::byte        pad1[0x64];                //0x00A4
	int                iSignonState;            //0x0108
	std::byte        pad2[0x8];                //0x010C
	float            flNextCmdTime;            //0x0114
	int                iServerCount;            //0x0118
	int                iCurrentSequence;        //0x011C
	char _0x0120[4];
	__int32 m_iClockDriftMgr; //0x0124 
	char _0x0128[68];
	__int32 m_iServerTick; //0x016C 
	__int32 m_iClientTick; //0x0170 
	int                iDeltaTick;                //0x0174
	bool            bPaused;                //0x0178
	std::byte        pad4[0x7];                //0x0179
	int                iViewEntity;            //0x0180
	int                iPlayerSlot;            //0x0184
	char            szLevelName[260];        //0x0188
	char            szLevelNameShort[80];    //0x028C
	char            szGroupName[80];        //0x02DC
	std::byte        pad5[0x5C];                //0x032C
	int                iMaxClients;            //0x0388
	std::byte        pad6[0x4984];            //0x038C
	float            flLastServerTickTime;    //0x4D10
	bool            bInSimulation;            //0x4D14
	std::byte        pad7[0xB];                //0x4D15
	int                iOldTickcount;            //0x4D18
	float            flTickRemainder;        //0x4D1C
	float            flFrameTime;            //0x4D20
	int                nLastOutgoingCommand;    //0x4D38
	int                iChokedCommands;        //0x4D30
	int                nLastCommandAck;        //0x4D2C
	int                iCommandAck;            //0x4D30
	int                iSoundSequence;            //0x4D34
	std::byte        pad8[0x50];                //0x4D38
	Vector            angViewPoint;            //0x4D88
	std::byte        pad9[0xD0];                //0x4D9A
}; 