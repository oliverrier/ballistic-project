#pragma once
#include "../../engine/utils/Factory/Factory.h"
struct Ground;
struct Bullet;

using AvailableGOTypes = typelist<Ground, Bullet>;
using GameObjectFactory = Factory<AvailableGOTypes, std::shared_ptr>;