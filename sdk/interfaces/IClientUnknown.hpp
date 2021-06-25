#pragma once

#include "../misc/IHandleEntity.hpp"
#include "ICollideable.hpp"

//class ICollideable;
class IClientNetworkable;
class IClientRenderable;
class IClientEntity;
class entity_t;
class IClientThinkable;
class IClientAlphaProperty;

class IClientUnknown : public IHandleEntity
{
public:
    virtual ICollideable*              GetCollideable() = 0;
    virtual IClientNetworkable*        GetClientNetworkable() = 0;
    virtual IClientRenderable*         GetClientRenderable() = 0;
    virtual IClientEntity*             GetIClientEntity() = 0;
    virtual entity_t*              GetBaseEntity() = 0;
    virtual IClientThinkable*          GetClientThinkable() = 0;
    //virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
    virtual IClientAlphaProperty*      GetClientAlphaProperty() = 0;
};