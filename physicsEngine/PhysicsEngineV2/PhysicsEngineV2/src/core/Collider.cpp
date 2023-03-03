#include "Collider.h"
#include "../utils/Math/Common.h"

Collider::Collider(const uint32_t count)
	: m_position(0.0f, 0.0f)
	, m_origin(0.0f, 0.0f)
	, m_rotation(0.0f)
	, m_vertices(std::vector<FVector2>{count})
	, m_verticesCount(count)
{}


void Collider::SetPosition(const FVector2& position) {
	const FVector2 diff = position - m_position - m_origin;

	for (uint32_t i = 0; i < m_verticesCount; i++) {
		auto& v = m_vertices[i];
		v += diff;
	}

	m_position = position - m_origin;
}

void Collider::SetOrigin(const FVector2& origin) {
	m_origin = origin;

	SetPosition(m_position);
}

void Collider::SetRotation(const float& angle) {
	const float diff = angle - m_rotation;

	for (uint32_t i = 0; i < m_verticesCount; i++) {
		auto& v = m_vertices[i];

		v = RotateAngle(v, m_position + m_origin, diff);
	}

	m_rotation = angle;
}

void Collider::Move(const FVector2& offset) {
	SetPosition(m_position + offset + m_origin);
}

void Collider::Rotate(const float& angle) {
	SetRotation(m_rotation + angle);
}

FVector2 Collider::GetPosition() const {
	return m_position + m_origin;
}

FVector2 Collider::GetOrigin() const {
	return m_origin;
}

float Collider::GetRotation() const {
	return m_rotation;
}

BoxCollider::BoxCollider()
	: Collider(4)
{

}

BoxCollider::~BoxCollider() {

}

void BoxCollider::Create(FVector2 size) {
	auto& v = m_vertices;

	FVector2 pos = m_position - m_origin;

	v[0] = pos + FVector2(0.0f, 0.0f);
	v[1] = pos + FVector2(size.x, 0.0f);
	v[2] = pos + FVector2(size.x, size.y);
	v[3] = pos + FVector2(0.0f, size.y);
}

/*
void BoxCollider::draw(sf::RenderTarget& target, const sf::RenderStates& states) const {
	static sf::Vertex v[6];

	for (int i = 0; i < 4; i++)
		v[i] = sf::Vertex(m_vertices[i], ColliderColor);

	v[4] = sf::Vertex(m_vertices[0], ColliderColor);
	v[5] = sf::Vertex(m_vertices[2], ColliderColor);

	target.draw(v, 6, sf::LineStrip);
}
*/

CircleCollider::CircleCollider()
	: Collider(1)
{

}

CircleCollider::~CircleCollider() = default;

void CircleCollider::Create(const float radius) {
	m_radius = radius;
}

float CircleCollider::GetRadius() const {
	return m_radius;
}

/*
void CircleCollider::draw(sf::RenderTarget& target, const sf::RenderStates& states) const {
	constexpr uint32_t points = 32;

	if (m_radius <= 0.0f)
		return;

	float angle =  2.f * PI / static_cast<float>(points);

	FVector2 p = m_position;
	p.y += m_radius;

	static sf::Vertex v[points + 1];

	for (uint32_t i = 0; i <= points; i++)
		v[i] = sf::Vertex(::Rotate(p, m_position, angle * (float)i), ColliderColor);

	target.draw(v, points + 1, sf::LineStrip);
	target.draw(&sf::Vertex(m_position, ColliderColor), 1, sf::Points);
}
*/

CustomCollider::CustomCollider()
	: Collider(0)
{

}

CustomCollider::CustomCollider(uint32_t count)
	: Collider(count)
{

}

CustomCollider::~CustomCollider() {

}

std::vector<FVector2>& CustomCollider::GetVertices() {
	return m_vertices;
}

FVector2& CustomCollider::GetVertices(uint32_t index) {
	return m_vertices[index];
}

void CustomCollider::Create(uint32_t count) {
	m_vertices.clear();
	m_vertices.resize(count);
	m_verticesCount = count;
}

FVector2& CustomCollider::operator[](uint32_t index) {
	return m_vertices[index];
}

/*
void CustomCollider::draw(sf::RenderTarget& target, const sf::RenderStates& states) const {
	const uint32_t count = m_verticesCount + 1;

	sf::Vertex* v = new sf::Vertex[count];

	for (uint32_t i = 0; i < count - 1; i++)
		v[i] = sf::Vertex(m_vertices[i], ColliderColor);

	v[count - 1] = sf::Vertex(m_vertices[0], ColliderColor);

	target.draw(v, count, sf::LineStrip);

	delete[] v;
}
*/
