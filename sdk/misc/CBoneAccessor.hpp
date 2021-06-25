//========= Copyright   1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#pragma once
#include "..\math\VMatrix.hpp"

class CBoneAccessor 
{
public:
	matrix3x4_t* GetBoneArrayForWrite()
	{
		return m_pBones;
	}

	void SetBoneArrayForWrite(matrix3x4_t* bone_array)
	{
		m_pBones = bone_array;
	}

	int GetReadableBones()
	{
		return m_ReadableBones;
	}

	void SetReadableBones(int flags)
	{
		m_ReadableBones = flags;
	}

	int GetWritableBones()
	{
		return m_WritableBones;
	}

	void SetWritableBones(int flags)
	{
		m_WritableBones = flags;
	}

	alignas(16) matrix3x4_t* m_pBones;
	int m_ReadableBones;
	int m_WritableBones;
};