#pragma once

#include <memory>

#include "engine/Ui/Buttons/RectangleButton.h"
#include "engine/Ui/HUD/HudElement.h"
#include "engine/Ui/HUD/HudArrow.h"
#include "engine/utils/Factory/Factory.h"
#include "engine/utils/Typelist/Typelist.h"

// Add Available Entities for the factory
using UiTypes = typelist < RectangleButton, HudElement<std::string> ,HudElement<float>, HudElement<int>, HudArrow>;

using UiFactory = Factory<UiTypes, std::shared_ptr>;

