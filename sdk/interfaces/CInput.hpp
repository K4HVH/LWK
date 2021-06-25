#pragma once

#include "../misc/CUserCmd.hpp"

#define MULTIPLAYER_BACKUP 150

class bf_write;
class bf_read;

class CInput
{
public:
	void* pvftable;						//0x0000 
	bool m_fTrackIRAvailable;			//0x0004 
	bool m_fMouseInitialized;			//0x0005 
	bool m_fMouseActive;				//0x0006 
	bool m_fJoystickAdvancedInit;		//0x0007 
	char pad_0x0008[0xA4];				//0x0008
	bool m_fCameraInterceptingMouse;	//0x00AC 
	bool m_fCameraInThirdPerson;		//0x00AD 
	bool m_fCameraMovingWithMouse;
	Vector m_vecCameraOffset;
	bool m_fCameraDistanceMove;
	int m_nCameraOldX;
	int m_nCameraOldY;
	int m_nCameraX;
	int m_nCameraY;
	bool m_CameraIsOrthographic;
	Vector m_angPreviousViewAngles;
	Vector m_angPreviousViewAnglesTilt;
	float m_flLastForwardMove;			// 0xEC
	int m_nClearInputState;
	CUserCmd* m_pCommands;				// 0xF4
	CVerifiedUserCmd* m_pVerifiedCommands;

	CUserCmd* CInput::GetUserCmd(int sequence_number)
	{
		return &m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
	}

	CVerifiedUserCmd* GetVerifiedUserCmd(int sequence_number)
	{
		return &m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
	}
};