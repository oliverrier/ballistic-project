#pragma once
#include <vector>

#include "Collider.h"
#include "Vector2.h"

class PolygonCollider : public Collider {
public:
    std::vector<Vector2> vertices;

    PolygonCollider() = default;

    bool detectCollision(const Collider* other) const override {
        const auto* polygon = dynamic_cast<const PolygonCollider*>(other);
        if (polygon) {
            // Use Separating Axis Theorem to check for collision
            std::vector<Vector2> axes;
            getAxes(axes);
            getAxes(polygon->vertices, axes);

            for (Vector2 axis : axes) {
                float minA, maxA, minB, maxB;
                project(axis, vertices, minA, maxA);
                project(axis, polygon->vertices, minB, maxB);

                if (maxA < minB || maxB < minA) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    Vector2 getSupportPoint(const Vector2& direction) const override {
        float maxDot = -std::numeric_limits<float>::infinity();
        Vector2 maxVertex;
        for (const Vector2 vertex : vertices) {
            const float dot = vertex.x * direction.x + vertex.y * direction.y;
            if (dot > maxDot) {
                maxDot = dot;
                maxVertex = vertex;
            }
        }
        return maxVertex;
    }

private:
    void getAxes(std::vector<Vector2>& axes) const {
        const int numVertices = vertices.size();
        axes.reserve(numVertices);
        for (int i = 0; i < numVertices; i++) {
            Vector2 p1 = vertices[i];
            Vector2 p2 = vertices[(i + 1) % numVertices];
            const Vector2 edge = p2 - p1;
            Vector2 normal = Vector2(-edge.y, edge.x).normalized();
            axes.push_back(normal);
        }
    }

    void getAxes(const std::vector<Vector2>& otherVertices, std::vector<Vector2>& axes) const {
        const int numVertices = otherVertices.size();
        axes.reserve(axes.size() + numVertices);
        for (int i = 0; i < numVertices; i++) {
            Vector2 p1 = otherVertices[i];
            Vector2 p2 = otherVertices[(i + 1) % numVertices];
            const Vector2 edge = p2 - p1;
            Vector2 normal = Vector2(-edge.y, edge.x).normalized();
            axes.push_back(normal);
        }
    }

    void project(const Vector2& axis, const std::vector<Vector2>& vertices, float& min, float& max) const {
        min = std::numeric_limits<float>::infinity();
        max = -std::numeric_limits<float>::infinity();
        for (const Vector2 vertex : vertices) {
            const float dot = vertex.x * axis.x + vertex.y * axis.y;
            if (dot < min) {
                min = dot;
            }
            if (dot > max) {
                max = dot;
            }
        }
    }
};