#pragma once
#include "../../engine/utils/Factory/Factory.h"
struct Ground;
struct Bullet;
struct Character;

using AvailableGOTypes = typelist<Ground, Bullet, Character>;
using GameObjectFactory = Factory<AvailableGOTypes, std::shared_ptr>;