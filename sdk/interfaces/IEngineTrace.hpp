#pragma once

#include "..\math\Vector4D.hpp"
#include "IClientEntity.hpp"

#pragma region MASKS

#define   DISPSURF_FLAG_SURFACE           (1<<0)
#define   DISPSURF_FLAG_WALKABLE          (1<<1)
#define   DISPSURF_FLAG_BUILDABLE         (1<<2)
#define   DISPSURF_FLAG_SURFPROP1         (1<<3)
#define   DISPSURF_FLAG_SURFPROP2         (1<<4)

#define   CONTENTS_EMPTY                0

#define   CONTENTS_SOLID                0x1       
#define   CONTENTS_WINDOW               0x2
#define   CONTENTS_AUX                  0x4
#define   CONTENTS_GRATE                0x8
#define   CONTENTS_SLIME                0x10
#define   CONTENTS_WATER                0x20
#define   CONTENTS_BLOCKLOS             0x40 
#define   CONTENTS_OPAQUE               0x80 
#define   LAST_VISIBLE_CONTENTS         CONTENTS_OPAQUE

#define   ALL_VISIBLE_CONTENTS            (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define   CONTENTS_TESTFOGVOLUME        0x100
#define   CONTENTS_UNUSED               0x200     
#define   CONTENTS_BLOCKLIGHT           0x400
#define   CONTENTS_TEAM1                0x800 
#define   CONTENTS_TEAM2                0x1000 
#define   CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000
#define   CONTENTS_MOVEABLE             0x4000
#define   CONTENTS_AREAPORTAL           0x8000
#define   CONTENTS_PLAYERCLIP           0x10000
#define   CONTENTS_MONSTERCLIP          0x20000
#define   CONTENTS_CURRENT_0            0x40000
#define   CONTENTS_CURRENT_90           0x80000
#define   CONTENTS_CURRENT_180          0x100000
#define   CONTENTS_CURRENT_270          0x200000
#define   CONTENTS_CURRENT_UP           0x400000
#define   CONTENTS_CURRENT_DOWN         0x800000

#define   CONTENTS_ORIGIN               0x1000000 

#define   CONTENTS_MONSTER              0x2000000 
#define   CONTENTS_DEBRIS               0x4000000
#define   CONTENTS_DETAIL               0x8000000 
#define   CONTENTS_TRANSLUCENT          0x10000000
#define   CONTENTS_LADDER               0x20000000
#define   CONTENTS_HITBOX               0x40000000

#define   SURF_LIGHT                    0x0001 
#define   SURF_SKY2D                    0x0002 
#define   SURF_SKY                      0x0004 
#define   SURF_WARP                     0x0008 
#define   SURF_TRANS                    0x0010
#define   SURF_NOPORTAL                 0x0020 
#define   SURF_TRIGGER                  0x0040 
#define   SURF_NODRAW                   0x0080 

#define   SURF_HINT                     0x0100 

#define   SURF_SKIP                     0x0200   
#define   SURF_NOLIGHT                  0x0400   
#define   SURF_BUMPLIGHT                0x0800   
#define   SURF_NOSHADOWS                0x1000   
#define   SURF_NODECALS                 0x2000   
#define   SURF_NOPAINT                  SURF_NODECALS
#define   SURF_NOCHOP                   0x4000   
#define   SURF_HITBOX                   0x8000   

// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define   MASK_ALL                      (0xFFFFFFFF)
#define   MASK_SOLID                    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCSOLID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCFLUID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_WATER                    (CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define   MASK_OPAQUE                   (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define   MASK_OPAQUE_AND_NPCS          (MASK_OPAQUE|CONTENTS_MONSTER)
#define   MASK_BLOCKLOS                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define   MASK_BLOCKLOS_AND_NPCS        (MASK_BLOCKLOS|CONTENTS_MONSTER)
#define   MASK_VISIBLE                  (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_VISIBLE_AND_NPCS         (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_SHOT                     (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define   MASK_SHOT_BRUSHONLY           (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define   MASK_SHOT_HULL                (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define   MASK_SHOT_PORTAL              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_SOLID_BRUSHONLY          (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID_BRUSHONLY    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define   MASK_NPCSOLID_BRUSHONLY       (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC           (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC_FLUID     (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define   MASK_SPLITAREAPORTAL          (CONTENTS_WATER|CONTENTS_SLIME)
#define   MASK_CURRENT                  (CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define   MASK_DEADSOLID                (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)
#pragma endregion

class IHandleEntity;
struct Ray_t;
class CGameTrace;
typedef CGameTrace trace_t;
class ICollideable;
class ITraceListData;
class CPhysCollide;
struct cplane_t;
struct virtualmeshlist_t;

#define DAMAGE_NO			0
#define DAMAGE_EVENTS_ONLY	1	
#define DAMAGE_YES			2
#define DAMAGE_AIM			3

enum class TraceType
{
    TRACE_EVERYTHING = 0,
    TRACE_WORLD_ONLY,
    TRACE_ENTITIES_ONLY,
    TRACE_EVERYTHING_FILTER_PROPS,
};

class ITraceFilter
{
public:
    virtual bool ShouldHitEntity(IHandleEntity *pEntity, int contentsMask) = 0;
    virtual TraceType GetTraceType() const = 0;
};


//-----------------------------------------------------------------------------
// Classes are expected to inherit these + implement the ShouldHitEntity method
//-----------------------------------------------------------------------------

// This is the one most normal traces will inherit from
class CTraceFilter : public ITraceFilter //-V730
{
public:
    bool ShouldHitEntity(IHandleEntity * pEntityHandle, int /*contentsMask*/)
    {
		ClientClass* eCC = ((IClientEntity*)pEntityHandle)->GetClientClass();
		if (eCC && strcmp(ccIgnore, "")) //-V526
		{
			if (eCC->m_pNetworkName == ccIgnore)
				return false;
		}

        return !(pEntityHandle == pSkip);
    }

    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }

	inline void SetIgnoreClass(char* Class)
	{
		ccIgnore = Class;
	}

    void* pSkip;
	char* ccIgnore = "";
};

class CTraceFilterOneEntity : public ITraceFilter {
public:
	bool ShouldHitEntity( IHandleEntity * pEntityHandle, int /*contentsMask*/ ) {
		return (pEntityHandle == pEntity );
	}

	TraceType GetTraceType( ) const {
		return TraceType::TRACE_EVERYTHING;
	}

	void* pEntity;
};

class CTraceFilterSkipEntity : public ITraceFilter
{
public:
    CTraceFilterSkipEntity(IHandleEntity* pEntityHandle)
    {
        pSkip = pEntityHandle;
    }

    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return !(pEntityHandle == pSkip);
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }
    void* pSkip;
};

class CTraceFilterEntitiesOnly : public ITraceFilter
{
public:
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return true;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_ENTITIES_ONLY;
    }
};


//-----------------------------------------------------------------------------
// Classes need not inherit from these
//-----------------------------------------------------------------------------
class CTraceFilterWorldOnly : public ITraceFilter
{
public:
    bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
    {
        return false;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_WORLD_ONLY;
    }
};

class CTraceFilterWorldAndPropsOnly : public ITraceFilter
{
public:
    bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
    {
        return false;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }
};

class CTraceFilterPlayersOnlySkipOne : public ITraceFilter
{
public:
    CTraceFilterPlayersOnlySkipOne(IClientEntity* ent)
    {
        e = ent;
    }
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return pEntityHandle != e && ((IClientEntity*)pEntityHandle)->GetClientClass()->m_ClassID == CCSPlayer;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_ENTITIES_ONLY;
    }

private:
    IClientEntity* e;
};

class CTraceFilterSkipTwoEntities : public ITraceFilter
{
public:
    CTraceFilterSkipTwoEntities(IClientEntity* ent1, IClientEntity* ent2)
    {
        e1 = ent1;
        e2 = ent2;
    }
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return !(pEntityHandle == e1 || pEntityHandle == e2);
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }

private:
    IClientEntity* e1;
    IClientEntity* e2;
};

class CTraceFilterHitAll : public CTraceFilter
{
public:
    virtual bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
    {
        return true;
    }
};


enum class DebugTraceCounterBehavior_t
{
    kTRACE_COUNTER_SET = 0,
    kTRACE_COUNTER_INC,
};

//-----------------------------------------------------------------------------
// Enumeration interface for EnumerateLinkEntities
//-----------------------------------------------------------------------------
class IEntityEnumerator
{
public:
    // This gets called with each handle
    virtual bool EnumEntity(IHandleEntity *pHandleEntity) = 0;
};


struct BrushSideInfo_t
{
    Vector4D plane;               // The plane of the brush side
    unsigned short bevel;    // Bevel plane?
    unsigned short thin;     // Thin?
};

class CPhysCollide;

struct vcollide_t
{
    unsigned short solidCount : 15;
    unsigned short isPacked : 1;
    unsigned short descSize;
    // VPhysicsSolids
    CPhysCollide   **solids;
    char           *pKeyValues;
    void           *pUserData;
};

struct cmodel_t
{
    Vector         mins, maxs;
    Vector         origin;        // for sounds or lights
    int            headnode;
    vcollide_t     vcollisionData;
};

struct csurface_t
{
    const char     *name;
    short          surfaceProps;
    unsigned short flags;         // BUGBUG: These are declared per surface, not per material, but this database is per-material now
};

//-----------------------------------------------------------------------------
// A ray...
//-----------------------------------------------------------------------------
struct Ray_t
{
    VectorAligned  m_Start;  // starting point, centered within the extents
    VectorAligned  m_Delta;  // direction + length of the ray
    VectorAligned  m_StartOffset; // Add this to m_Start to Get the actual ray start
    VectorAligned  m_Extents;     // Describes an axis aligned box extruded along a ray
    const matrix3x4_t *m_pWorldAxisTransform;
    bool m_IsRay;  // are the extents zero?
    bool m_IsSwept;     // is delta != 0?

    Ray_t() : m_pWorldAxisTransform(NULL) {} //-V730

	Ray_t( Vector const & start, Vector const & end ) { //-V818
		m_Delta = end - start;

		m_IsSwept = ( m_Delta.LengthSqr( ) != 0 ); //-V550

		m_Extents.Init( );

		m_pWorldAxisTransform = NULL;
		m_IsRay = true;

		// Offset m_Start to be in the center of the box...
		m_StartOffset.Init( );
		m_Start = start;
	}

    __forceinline Ray_t(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs) {
        m_Delta = VectorAligned{ end - start };
        m_pWorldAxisTransform = nullptr;
        m_IsSwept = m_Delta.LengthSqr() != 0.f;
        m_Extents = VectorAligned{ maxs - mins };
        m_Extents *= 0.5f;
        m_IsRay = m_Extents.LengthSqr() < 1e-6;
        m_StartOffset = VectorAligned{ mins + maxs };
        m_StartOffset *= 0.5f;
        m_Start = VectorAligned{ start + m_StartOffset };
        m_StartOffset *= -1.f;
    }
    void Init(Vector const& start, Vector const& end)
    {
        m_Delta = end - start;

        m_IsSwept = (m_Delta.LengthSqr() != 0); //-V550

        m_Extents.Init();

        m_pWorldAxisTransform = NULL;
        m_IsRay = true;

        // Offset m_Start to be in the center of the box...
        m_StartOffset.Init();
        m_Start = start;
    }

    void Init(Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs)
    {
        m_Delta = end - start;

        m_pWorldAxisTransform = NULL;
        m_IsSwept = (m_Delta.LengthSqr() != 0); //-V550

        m_Extents = maxs - mins;
        m_Extents *= 0.5f;
        m_IsRay = (m_Extents.LengthSqr() < 1e-6);

        // Offset m_Start to be in the center of the box...
        m_StartOffset = maxs + mins;
        m_StartOffset *= 0.5f;
        m_Start = start + m_StartOffset;
        m_StartOffset *= -1.0f;
    }
    Vector InvDelta() const
    {
        Vector vecInvDelta;
        for(int iAxis = 0; iAxis < 3; ++iAxis) {
            if(m_Delta[iAxis] != 0.0f) { //-V550
                vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
            } else {
                vecInvDelta[iAxis] = FLT_MAX;
            }
        }
        return vecInvDelta;
    }

private:
};

class CBaseTrace
{
public:
    bool IsDispSurface(void) { return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0); }
    bool IsDispSurfaceWalkable(void) { return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
    bool IsDispSurfaceBuildable(void) { return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
    bool IsDispSurfaceProp1(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
    bool IsDispSurfaceProp2(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

public:

    // these members are aligned!!
    Vector         startpos;            // start position
    Vector         endpos;              // final position
    cplane_t       plane;               // surface normal at impact

    float          fraction;            // time completed, 1.0 = didn't hit anything

    int            contents;            // contents on other side of surface hit
    unsigned short dispFlags;           // displacement flags for marking surfaces with data

    bool           allsolid;            // if true, plane is not valid
    bool           startsolid;          // if true, the initial point was in a solid area

    CBaseTrace() {} //-V730

};

class CGameTrace : public CBaseTrace //-V690
{
public:
	inline bool IsVisible() const
	{
		return fraction > 0.97f;
	}

	inline bool DidHit() const
	{
		return fraction < 1.0 || allsolid || startsolid;
	}
public:

    float               fractionleftsolid;  // time we left a solid, only valid if we started in solid
    csurface_t          surface;            // surface hit (impact surface)
    int                 hitgroup;           // 0 == generic, non-zero is specific body part
    short               physicsbone;        // physics bone hit by trace in studio
    unsigned short      worldSurfaceIndex;  // Index of the msurface2_t, if applicable
    IClientEntity*      hit_entity;
    int                 hitbox;                       // box hit by trace in studio

    CGameTrace() {} //-V730

private:
    // No copy constructors allowed
    CGameTrace(const CGameTrace& other) :
        fractionleftsolid(other.fractionleftsolid),
        surface(other.surface),
        hitgroup(other.hitgroup),
        physicsbone(other.physicsbone),
        worldSurfaceIndex(other.worldSurfaceIndex),
        hit_entity(other.hit_entity),
        hitbox(other.hitbox)
    {
        startpos = other.startpos;
        endpos = other.endpos;
        plane = other.plane;
        fraction = other.fraction;
        contents = other.contents;
        dispFlags = other.dispFlags;
        allsolid = other.allsolid;
        startsolid = other.startsolid;
    }
};

class IEngineTrace
{
public:
    virtual int   GetPointContents(const Vector &vecAbsPosition, int contentsMask = MASK_ALL, IHandleEntity** ppEntity = nullptr) = 0;
    virtual int   GetPointContents_WorldOnly(const Vector &vecAbsPosition, int contentsMask = MASK_ALL) = 0;
    virtual int   GetPointContents_Collideable(ICollideable *pCollide, const Vector &vecAbsPosition) = 0;
    virtual void  ClipRayToEntity(const Ray_t &ray, unsigned int fMask, IHandleEntity *e, CGameTrace *pTrace) = 0;
    virtual void  ClipRayToCollideable(const Ray_t &ray, unsigned int fMask, ICollideable *pCollide, CGameTrace *pTrace) = 0;
    virtual void  TraceRay(const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, CGameTrace *pTrace) = 0;
};