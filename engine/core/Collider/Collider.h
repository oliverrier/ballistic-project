#pragma once

#include "../../utils/RTTI/RTTI.h"

class Collider: public RTTI {
    DECLARE_RTTI(Collider, RTTI)
public:
    virtual ~Collider() = default;
};