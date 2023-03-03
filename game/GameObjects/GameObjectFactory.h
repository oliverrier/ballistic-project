#pragma once
#include "../../engine/utils/Factory/Factory.h"
struct Ground;
struct Bullet;
struct Character;
struct Wall;

using AvailableGOTypes = typelist<Ground, Bullet, Character, Wall>;
using GameObjectFactory = Factory<AvailableGOTypes, std::shared_ptr>;