#pragma once
#include <memory>

#include "Entity.h"
#include "engine/utils/Factory/Factory.h"

#pragma once

// Add Available Entities for the factory
using EntityFactory = Factory<AvailableTypes, std::shared_ptr>;

