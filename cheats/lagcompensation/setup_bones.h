#pragma once
#include "..\..\includes.hpp"

class CSetupBones
{
public:
	void setup();
	void get_skeleton();
	void studio_build_matrices(CStudioHdr* hdr, const matrix3x4_t& worldTransform, Vector* pos, Quaternion* q, int boneMask, matrix3x4_t* out, uint32_t* boneComputed);
	void attachment_helper();
	void fix_bones_rotations();

	matrix3x4_t* m_boneMatrix = nullptr;
	Vector m_vecOrigin = ZERO;
	Vector m_angAngles = ZERO;
	CStudioHdr* m_pHdr = nullptr;
	Vector* m_vecBones = nullptr;
	Quaternion* m_quatBones = nullptr;
	bool m_bShouldDoIK = false;
	bool m_bShouldAttachment = true;
	bool m_bShouldDispatch = true;
	int m_boneMask = 0;
	float m_flPoseParameters[24];
	float m_flWorldPoses[24];
	int m_nAnimOverlayCount = 0;
	AnimationLayer* m_animLayers = nullptr;
	float m_flCurtime = 0.0f;
	player_t* m_animating = nullptr;
};