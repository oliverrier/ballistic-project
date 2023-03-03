#pragma once

#include <memory>
#include "Collider.h"


class CircleCollider : public Collider {
public:
    CircleCollider(std::shared_ptr<RigidBody>& rb, const float& radius) : rb(rb), radius(radius) {}

    std::shared_ptr<RigidBody> rb;
    float radius;
};