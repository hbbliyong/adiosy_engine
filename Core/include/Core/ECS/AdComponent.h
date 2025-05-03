#pragma once

#include "Core/ECS/AdEntity.h"
namespace ade
{
    class AdComponent
    {
    public:
        void SetOwner(AdEntity* owner) { mOwner = owner; }
        AdEntity* GetOwner() const { return mOwner; }
    private:
        AdEntity* mOwner=nullptr;
    };
}