#pragma once
#include <memory>
#include "../utils/Factory/Factory.h"

class CircleEntity;
class RectangleEntity;
class PolygonEntity;

using AvailableTypes = typelist<CircleEntity, RectangleEntity, PolygonEntity>;
using EntityFactory = Factory<AvailableTypes, std::shared_ptr>;
