#pragma once

#include "..\misc\BeamInfo_t.hpp"
#include "IEngineTrace.hpp"

class Beam_t;
class C_Beam;

class IViewRenderBeams {
public:
	// Construction
public:
	IViewRenderBeams( void );
	virtual				~IViewRenderBeams( void );

	// Implement IViewRenderBeams
public:
	virtual	void		InitBeams( void );
	virtual	void		ShutdownBeams( void );
	virtual	void		ClearBeams( void );

	// Updates the state of the temp ent beams
	virtual void		UpdateTemeBeams( );

	virtual void		DrawBeam( Beam_t *pbeam );
	virtual void		DrawBeam( C_Beam* pbeam, ITraceFilter *pEntityBeamTraceFilter = NULL );

	virtual	void		KillDeadBeams( IClientEntity *pDeadEntity );

	virtual	void		CreateBeamEnts( int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int type = -1 );
	virtual Beam_t		*CreateBeamEnts( BeamInfo_t &beamInfo );

	virtual	void		CreateBeamEntPoint( int	nStartEntity, const Vector *pStart, int nEndEntity, const Vector* pEnd,
		int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b );
	virtual Beam_t		*CreateBeamEntPoint( BeamInfo_t &beamInfo );

	virtual	void		CreateBeamPoints( Vector& start, Vector& end, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b );
	virtual	Beam_t		*CreateBeamPoints( BeamInfo_t &beamInfo );

	virtual	void		CreateBeamRing( int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags );
	virtual Beam_t		*CreateBeamRing( BeamInfo_t &beamInfo );

	virtual void		CreateBeamRingPoint( const Vector& center, float start_radius, float end_radius, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags );
	virtual Beam_t		*CreateBeamRingPoint( BeamInfo_t &beamInfo );

	virtual	void		CreateBeamCirclePoints( int type, Vector& start, Vector& end,
		int modelIndex, int haloIndex, float haloScale, float life, float width,
		float endWidth, float fadeLength, float amplitude, float brightness, float speed,
		int startFrame, float framerate, float r, float g, float b );
	virtual Beam_t		*CreateBeamCirclePoints( BeamInfo_t &beamInfo );

	virtual	void		CreateBeamFollow( int startEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float r, float g, float b,
		float brightness );
	virtual Beam_t		*CreateBeamFollow( BeamInfo_t &beamInfo );
	/*virtual void	InitBeams(void) = 0;
	virtual void	ShutdownBeams(void) = 0;
	virtual void	ClearBeams(void) = 0;

	// Updates the state of the temp ent beams
	virtual void	UpdateTemeBeams() = 0;

	virtual void	DrawBeam(C_Beam* pbeam, ITraceFilter *pEntityBeamTraceFilter = NULL) = 0;
	virtual void	DrawBeam(Beam_t *pbeam) = 0;

	virtual void	KillDeadBeams(IClientEntity* e) = 0;

	// New interfaces!
	virtual Beam_t	*CreateBeamEnts(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t	*CreateBeamEntPoint(BeamInfo_t &beamInfo) = 0;
	virtual	Beam_t	*CreateBeamPoints(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t	*CreateBeamRing(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t	*CreateBeamRingPoint(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t	*CreateBeamCirclePoints(BeamInfo_t &beamInfo) = 0;
	virtual Beam_t	*CreateBeamFollow(BeamInfo_t &beamInfo) = 0;

	virtual void	FreeBeam(Beam_t *pBeam) = 0;
	virtual void	UpdateBeamInfo(Beam_t *pBeam, BeamInfo_t &beamInfo) = 0;*/
};