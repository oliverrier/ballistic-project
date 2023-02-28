
struct HudArrow : public sf::RectangleShape {

public:
	HudArrow(const FVector2& position, const FVector2& size, float* angle, const sf::Color& color = sf::Color::Transparent) : m_angle(angle)
	{
		m_initialPosition = position;

		setPosition(position);
		setSize(sf::Vector2f(size.x, size.y));
		setFillColor(color);

		if (!m_texture.loadFromFile("Assets/WindArrow.png")) {
			std::cout << "can't load texture" << std::endl;
		}

		m_sprite.setTexture(m_texture);
		m_sprite.setPosition(position.x, position.y);
		m_sprite.setScale(size.x / m_texture.getSize().x, size.y / m_texture.getSize().y);
		m_sprite.setOrigin(size.x / 2.0f, size.y / 2.0f);

	}

	~HudArrow() = default;

	void draw(sf::RenderTarget& renderTarget, sf::RenderStates state) const override {
		renderTarget.draw(static_cast<sf::RectangleShape>(*this), state);
		if (m_sprite.getTexture() != nullptr) {
			renderTarget.draw(m_sprite, state);
		}
	}


	void setPosition(const FVector2 position) {
		sf::RectangleShape::setPosition(position.x, position.y);
		setSpriteRotation(*m_angle);
		if (m_sprite.getTexture() != nullptr) {
			m_sprite.setPosition(position.x, position.y);
		}


	}

	void setSpriteRotation(float angle) {
		if (m_sprite.getTexture() != nullptr) {
			m_sprite.setRotation(angle);
		}
	}

	void getSpriteRotation() {
		std::cout << m_sprite.getRotation() << std::endl;
	}

	FVector2 getInitialPosition() {
		return m_initialPosition;
	}

private:

	sf::Texture m_texture;
	sf::Sprite m_sprite;
	float* m_angle;
	FVector2 m_initialPosition;
};