#pragma once

#include <vector>
#include "Collider.h"
#include "CircleCollider.h"
#include "RectangleCollider.h"
#include "PolygonCollider.h"
#include "../RigidBody.h"
#include <cmath>

class PolygonCollider : public Collider {
public:
    PolygonCollider(const std::shared_ptr<RigidBody>& rb, const std::vector<FVector2>& vertices): rb(rb), vertices(vertices) {}

    std::shared_ptr<RigidBody> rb;
    std::vector<FVector2> vertices;

    /*
    virtual bool IsColliding(const Collider* other) const
    {
        if (const auto* circle = dynamic_cast<const CircleCollider*>(other)) {
            // Check if any vertex of the polygon is inside the circle
            for (const FVector2& vertex : vertices) {
                const FVector2 delta = vertex - circle->rb->position;
                if (delta.Size() < circle->radius) {
                    return true;
                }
            }

            // Check if the center of the circle is inside the polygon
            if (IsPointInPolygon(circle->rb->position)) {
                return true;
            }

            // Check if any edge of the polygon intersects with the circle
            for (int i = 0; i < vertices.size(); ++i) {
                FVector2 edge = vertices[(i + 1) % vertices.size()] - vertices[i];
                FVector2 delta = vertices[i] - circle->rb->position;
                const float edgeLength = edge.Size();
                const float projection = delta.Dot(edge) / edgeLength;

                if (projection >= 0.0f && projection <= edgeLength) {
                    float distance = (delta - edge * (projection / edgeLength)).Size();
                    if (distance < circle->radius) {
                        return true;
                    }
                }
            }
        }
        else if (const auto* rectangle = dynamic_cast<const RectangleCollider*>(other)) {
            // Check if any vertex of the polygon is inside the rectangle
            for (const FVector2& vertex : vertices) {
                if (rectangle->IsPointInRectangle(vertex)) {
                    return true;
                }
            }

            // Check if any edge of the polygon intersects with any edge of the rectangle
            for (int i = 0; i < vertices.size(); ++i) {
                FVector2 edge = vertices[(i + 1) % vertices.size()] - vertices[i];
                if (rectangle->IsSegmentIntersectingRectangle(vertices[i], edge)) {
                    return true;
                }
            }
        }
        else if (const auto* polygon = dynamic_cast<const PolygonCollider*>(other)) {
            // Check if any vertex of either polygon is inside the other polygon
            for (const FVector2& vertex : vertices) {
                if (polygon->IsPointInPolygon(vertex)) {
                    return true;
                }
            }
            for (const FVector2& vertex : polygon->vertices) {
                if (IsPointInPolygon(vertex)) {
                    return true;
                }
            }

            // Check if any edge of either polygon intersects with any edge of the other polygon
            for (int i = 0; i < vertices.size(); ++i) {
                FVector2 edge = vertices[(i + 1) % vertices.size()] - vertices[i];
                for (int j = 0; j < polygon->vertices.size(); ++j) {
                    FVector2 otherEdge = polygon->vertices[(j + 1) % polygon->vertices.size()] - polygon->vertices[j];
                    FVector2 intersection;
                    if (AreSegmentsIntersecting(vertices[i], edge, polygon->vertices[j], otherEdge, intersection)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    */

    void ProjectOntoAxis(const FVector2& axis, float& min, float& max) const
    {
        float dotProduct = vertices[0].Dot(axis);
        min = max = dotProduct;

        for (unsigned int i = 1; i < vertices.size(); i++) {
            dotProduct = vertices[i].Dot(axis);
            if (dotProduct < min) {
                min = dotProduct;
            }
            else if (dotProduct > max) {
                max = dotProduct;
            }
        }
    }

private:
    bool IsPointInPolygon(const FVector2& point) const
    {
        float angle = 0.0f;
        for (int i = 0; i < vertices.size(); ++i) {
            const FVector2 delta1 = vertices[i] - point;
            const FVector2 delta2 = vertices[(i + 1) % vertices.size()] - point;
            const float crossProduct = FVector2::CrossProduct(delta1, delta2);
            const float dotProduct = delta1.Dot(delta2);
            const float lengthProduct = delta1.Size() * delta2.Size();

            if (crossProduct == 0.0f && dotProduct > 0.0f && dotProduct < lengthProduct) {
                return false;
            }

            angle += std::atan2(crossProduct, dotProduct);
        }
        return std::abs(angle) > 1e-5;
    }

    bool AreSegmentsIntersecting(const FVector2& a1, const FVector2& a2, const FVector2& b1, const FVector2& b2, FVector2& intersection) const
    {
        FVector2 b = a2 - a1;
        FVector2 d = b2 - b1;
        float bDotDPerp = b.x * d.y - b.y * d.x;

        if (bDotDPerp == 0) {
            return false;
        }

        FVector2 c = b1 - a1;
        float t = (c.x * d.y - c.y * d.x) / bDotDPerp;
        if (t < 0 || t > 1) {
            return false;
        }

        float u = (c.x * b.y - c.y * b.x) / bDotDPerp;
        if (u < 0 || u > 1) {
            return false;
        }

        intersection = a1 + b * t;
        return true;
    }


};