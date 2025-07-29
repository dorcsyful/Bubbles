#pragma once
#include <memory>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
class SpriteWithText : public sf::Drawable
{
public:

	SpriteWithText(const sf::Font& a_Font);
	SpriteWithText(const std::string& a_Text, const sf::Font& a_Font, const sf::Vector2f& a_Size, const sf::Vector2f& a_Position, const sf::Color& a_TextColor = sf::Color::Black, const sf::Color& a_ShapeBackground = sf::Color::White);
	void InitializeWithData(const std::string& a_Text, const sf::Font& a_Font, const sf::Vector2f& a_Size,
	                        const sf::Vector2f& a_Position);
	SpriteWithText(const std::string& a_Text, const sf::Font& a_Font, const sf::Vector2f& a_Size, const sf::Vector2f& a_Position, const sf::Color& a_TextColor = sf::Color::Black, const sf::Texture* a_BackgroundTexture = nullptr);

	void SetText(const std::string& a_Text);
	std::string GetText() const { return m_Text->getString(); }
	sf::Vector2f GetPosition() const { return m_Shape->getPosition(); }
	float GetHeight() const { return m_Shape->getGlobalBounds().size.x; }
	float GetWidth() const { return m_Shape->getGlobalBounds().size.x; }
	void ResizeCharacters(int a_Size) const {
		m_Text->setCharacterSize(a_Size);
		m_Text->setOrigin(m_Text->getGlobalBounds().size / 2.f + m_Text->getLocalBounds().position);


		m_Text->setPosition(m_Shape->getPosition());
	}

private:

	void draw(sf::RenderTarget& a_Target, const sf::RenderStates a_States) const override
	{
		// You can draw other high-level objects
		a_Target.draw(*m_Shape, a_States);
		a_Target.draw(*m_Text, a_States);
	}

	std::unique_ptr<sf::RectangleShape> m_Shape;
	std::unique_ptr<sf::Text> m_Text;
};

