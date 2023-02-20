#pragma once

#include "engine/utils/Math/Vector2.h"
#include <SFML/Graphics.hpp>


class Camera
{
public:
	explicit Camera(const std::shared_ptr<sf::RenderWindow>& w, const float move_step = 2.f, const float zoom_step = 2.f): m_window(w), m_move_step(move_step), m_zoom_step(zoom_step)
	{
		const auto view = w->getView();
		m_position = { view.getCenter().x, view.getCenter().y };
		m_zoom = 1.f;

	}
	~Camera() = default;

	void Zoom()
	{
		m_zoom += m_zoom_step;
	}

	void UnZoom()
	{
		m_zoom -= m_zoom_step;
	}

	void Move(int x, int y)
	{
		m_position.x += x;
		m_position.y += y;
	}

	void MoveRight()
	{
		m_position.x += m_move_step;
	}

	void MoveLeft()
	{
		m_position.x -= m_move_step;
	}

	void MoveUp()
	{
		m_position.y -= m_move_step;
	}

	void MoveDown()
	{
		m_position.y += m_move_step;
	}

	void Update()
	{
		auto view = m_window->getView();
		view.setCenter({ m_position.x, m_position.y });
		view.zoom(m_zoom);
		m_window->setView(view);
	}

	FVector2& getLastMousePosition()
	{
		return m_last_position;
	}

	void SetLastMousePosition(const FVector2& position)
	{
		m_last_position = position;
	}

private:
	std::shared_ptr<sf::RenderWindow> m_window;

	FVector2 m_last_position;

	FVector2 m_position;
	float m_move_step;

	float m_zoom;
	float m_zoom_step;
};