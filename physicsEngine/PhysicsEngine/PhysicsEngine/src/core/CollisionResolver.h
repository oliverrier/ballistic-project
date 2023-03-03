#pragma once

#include "RigidBody.h"
#include "CollisionResolver.h"
#include "Collider/CircleCollider.h"
#include "Collider/RectangleCollider.h"
#include "Collider/PolygonCollider.h"
#include <cmath>
#include <algorithm>

class CollisionResolver {
public:

    static bool IsColliding(RigidBody* a, RigidBody* b)
    {
	    switch (a->colliderType)
	    {
	    case None:
            return false;
	    case Rectangle:
		    switch (b->colliderType)
		    {
				case None:
                    return false;
				case Rectangle:
                    return IsCollidingRectangleRectangle(a->collider->As<RectangleCollider>(), b->collider->As<RectangleCollider>());
				case Circle:
                    return IsCollidingRectangleCircle(a->collider->As<RectangleCollider>(), b->collider->As<CircleCollider>());
                default:
                    return false;
		    }
        case Circle:
            switch (b->colliderType)
            {
            case None:
                return false;
            case Rectangle:
                return IsCollidingRectangleCircle(b->collider->As<RectangleCollider>(), a->collider->As<CircleCollider>());
            case Circle:
                return IsCollidingCircleCircle(a->collider->As<CircleCollider>(), b->collider->As<CircleCollider>());
            default:
                return false;
            }
		default: 
			return false;
	    }
    }

    static bool IsCollidingCircleCircle(CircleCollider* a, CircleCollider* b)
    {
        // Check if the distance between the centers of the circles is less than the sum of their radii
        const float distance = (a->rb->position - b->rb->position).Size();
        return distance < a->radius + b->radius;
    }

    static bool IsCollidingRectangleCircle(RectangleCollider* a, CircleCollider* b)
    {
        // Check if the circle is inside the rectangle
        FVector2 min = a->rb->position - a->size / 2.0f;
        FVector2 max = a->rb->position + a->size / 2.0f;
        FVector2 closest = b->rb->position;
        closest.x = std::min(std::max(min.x, closest.x), max.x);
        closest.y = std::min(std::max(min.y, closest.y), max.y);
        return (closest - b->rb->position).Size() < b->radius;
    }

    static bool IsCollidingRectangleRectangle(RectangleCollider* a, RectangleCollider* b)
    {
        // Check if one rectangle is inside the other
        FVector2 minA = a->rb->position - a->size / 2.0;
        FVector2 maxA = a->rb->position + a->size / 2.0;
        FVector2 minB = b->rb->position - b->size / 2.0;
        FVector2 maxB = b->rb->position + b->size / 2.0;

        return (minA.x <= maxB.x && maxA.x >= minB.x) && (minA.y <= maxB.y && maxA.y >= minB.y);
    }

    


    static void ResolveCollision(RigidBody* a, RigidBody* b)
    {
        if (dynamic_cast<CircleCollider*>(a->collider.get()) && dynamic_cast<CircleCollider*>(b->collider.get())) {
            ResolveCircleCircleCollision(a, b);
        }
        else if (dynamic_cast<CircleCollider*>(a->collider.get()) && dynamic_cast<RectangleCollider*>(b->collider.get())) {
            ResolveCircleRectangleCollision(a, b);
        }
        else if (dynamic_cast<RectangleCollider*>(a->collider.get()) && dynamic_cast<RectangleCollider*>(b->collider.get())) {
            ResolveRectangleRectangleCollision(a, b);
        }
    }

private:
    static void ResolveCircleCircleCollision(RigidBody* a, RigidBody* b)
    {
        CircleCollider* A = dynamic_cast<CircleCollider*>(a->collider.get());
        CircleCollider* B = dynamic_cast<CircleCollider*>(b->collider.get());

        FVector2 normal = a->position - b->position;
        const float distance = normal.Size();
        const float radiusSum = A->radius + B->radius;

        if (distance < radiusSum) {
            normal.Normalize();
            float overlap = radiusSum - distance;
            FVector2 correction = normal * overlap / 2;

            a->position -= correction * !(a->isStatic) * (2.0 * b->isStatic);
            b->position += correction * !(b->isStatic) * (2.0 * a->isStatic);

            if (a->isStatic || b->isStatic)
            {
                a->velocity = { 0.f,0.f };
                b->velocity = { 0.f,0.f };
			} else
            {
                float impulse = std::min(a->mass, b->mass);
                FVector2 relativeVelocity = a->velocity - b->velocity;
                float velocityAlongNormal = relativeVelocity.Dot(normal);

                if (velocityAlongNormal > 0) {
                    return;
                }

                float impulseScalar = -(1 + 0.8) * velocityAlongNormal / (1 / a->mass + 1 / b->mass);
                FVector2 impulseVector = normal * impulseScalar * impulse;
                a->velocity -= impulseVector * 1 / a->mass;
                b->velocity += impulseVector * 1 / b->mass;
            }
        }
    }

    static void ResolveCircleRectangleCollision(RigidBody* a, RigidBody* b)
    {
        CircleCollider* circle = dynamic_cast<CircleCollider*>(a->collider.get());
        RectangleCollider* rectangle = dynamic_cast<RectangleCollider*>(b->collider.get());

        FVector2 closestPoint = a->position;
        closestPoint.x = std::clamp(closestPoint.x, rectangle->rb->position.x - rectangle->GetHalfExtents().x, rectangle->rb->position.x + rectangle->GetHalfExtents().x);
        closestPoint.y = std::clamp(closestPoint.y, rectangle->rb->position.y - rectangle->GetHalfExtents().y, rectangle->rb->position.y + rectangle->GetHalfExtents().y);
        
        // Calculate the vector from the circle's center to the closest point on the rectangle
        FVector2 circleToClosest = closestPoint - a->position;

        // Check if the circle is colliding with the rectangle
        if (circleToClosest.SizeSquared() < (circle->radius * circle->radius)) {
            FVector2 normal = circleToClosest.Normalized();
            float overlap = circle->radius - circleToClosest.Size();

            // Perform overlap correction
            FVector2 correction = normal * overlap;
            a->position -= correction * !(a->isStatic) * (2.0 * b->isStatic);
            b->position += correction * !(b->isStatic) * (2.0 * a->isStatic);

            if (a->isStatic || b->isStatic)
            {
                a->velocity = { 0.f,0.f };
                b->velocity = { 0.f,0.f };
            }
            else {

                float impulse = std::min(a->mass, b->mass);
                FVector2 relativeVelocity = a->velocity - b->velocity;
                float velocityAlongNormal = relativeVelocity.Dot(normal);

                if (velocityAlongNormal > 0) {
                    return;
                }

                float impulseScalar = -(1 + 0.8) * velocityAlongNormal / (1 / a->mass + 1 / b->mass);
                FVector2 impulseVector = normal * impulseScalar * impulse;
                a->velocity -= impulseVector * 1 / a->mass;
                b->velocity += impulseVector * 1 / b->mass;
            }
        }
    }


    static void ResolveRectangleRectangleCollision(RigidBody* a, RigidBody* b)
    {
        RectangleCollider* rectA = a->collider->As<RectangleCollider>();
        RectangleCollider* rectB = b->collider->As<RectangleCollider>();

        FVector2 distance = b->position - a->position;
        FVector2 normal = { 0.f, 0.f };
        float minOverlap = FLT_MAX;

        // Calculate the overlaps in x and y directions
        float xOverlap = rectA->GetHalfSize().x + rectB->GetHalfSize().x - std::abs(distance.x);
        if (xOverlap > 0) {
            float yOverlap = rectA->GetHalfSize().y + rectB->GetHalfSize().y - std::abs(distance.y);
            if (yOverlap > 0) {
                if (xOverlap < yOverlap) {
                    // x axis overlap is smaller, resolve in x axis
                    if (distance.x < 0) {
                        normal.x = -1;
                        minOverlap = xOverlap;
                    }
                    else {
                        normal.x = 1;
                        minOverlap = xOverlap;
                    }
                }
                else {
                    // y axis overlap is smaller, resolve in y axis
                    if (distance.y < 0) {
                        normal.y = -1;
                        minOverlap = yOverlap;
                    }
                    else {
                        normal.y = 1;
                        minOverlap = yOverlap;
                    }
                }
            }
        }

        if (minOverlap != FLT_MAX) {
	        const FVector2 correction = normal * minOverlap * 0.5;
            a->position -= correction * !(a->isStatic) * (2.0 * b->isStatic);
            b->position += correction * !(b->isStatic) * (2.0 * a->isStatic);

            if (a->isStatic || b->isStatic)
            {
                a->velocity = { 0.f,0.f };
                b->velocity = {0.f,0.f};
            }
            else {

                const float impulse = std::min(a->mass, b->mass);
                const FVector2 relativeVelocity = a->velocity - b->velocity;
                const float velocityAlongNormal = relativeVelocity.Dot(normal);

                if (velocityAlongNormal > 0) {
                    return;
                }

                const float impulseScalar = -(1 + 0.8) * velocityAlongNormal / (1 / a->mass + 1 / b->mass);
                const FVector2 impulseVector = normal * impulseScalar * impulse;
                a->velocity -= impulseVector * 1 / a->mass;
                b->velocity += impulseVector * 1 / b->mass;
            }
        }
    }
};