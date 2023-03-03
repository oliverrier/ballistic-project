#pragma once
#include <functional>
#include "../utils/Math/Vector2.h"
#include <SFML/Graphics.hpp>


class Collision;

class Collider : public sf::Drawable {
public:
	Collider(uint32_t count);
	virtual ~Collider() = default;
		
	// Modifiers
	void SetPosition(const FVector2& position);
	void SetOrigin(const FVector2& origin);
	void SetRotation(const float& angle);
	void Move(const FVector2& offset);
	void Rotate(const float& angle);

	// Accessors
	FVector2 GetPosition() const;
	FVector2 GetOrigin()   const;
	float	 GetRotation() const;

	std::function<void()> Trigger;

protected:
	FVector2  m_position;
	FVector2  m_origin;
	float	  m_rotation;

	std::vector<FVector2> m_vertices;
	uint32_t	  m_verticesCount = 0;

private:
	friend class Collision;
};

class BoxCollider : public Collider {
public:
	BoxCollider();
	~BoxCollider();

	void Create(FVector2 size);

private:
	FVector2 m_size;

	virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;
};

class CircleCollider : public Collider {
public:
	CircleCollider();
	~CircleCollider();

	void Create(float radius);

	float GetRadius() const;

private:
	float m_radius = 0.0f;

	virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;
};

class CustomCollider : public Collider {
public:
	CustomCollider();

	CustomCollider(uint32_t count);

	~CustomCollider();

	std::vector<FVector2>& GetVertices();
	FVector2& GetVertices(uint32_t index);

	void Create(uint32_t count);

	FVector2& operator[](uint32_t index);

private:
	virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;
};
