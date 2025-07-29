#include "SpriteWithText.h"
#include <SFML/Graphics/Texture.hpp>
#include <iostream>


SpriteWithText::SpriteWithText(const sf::Font& a_Font)
{
	m_Shape = std::make_unique<sf::RectangleShape>();
	m_Text = std::make_unique<sf::Text>(a_Font);
}

SpriteWithText::SpriteWithText(const std::string& a_Text, const sf::Font& a_Font, const sf::Vector2f& a_Size, const sf::Vector2f& a_Position, const sf::Color& a_TextColor, const sf::Color& a_ShapeBackground)
{
	m_Shape = std::make_unique<sf::RectangleShape>();
	m_Text = std::make_unique<sf::Text>(a_Font);
	InitializeWithData(a_Text, a_Font, a_Size, a_Position);

	m_Shape->setFillColor(a_ShapeBackground);
	m_Text->setFillColor(a_TextColor);
}

SpriteWithText::SpriteWithText(const std::string& a_Text, const sf::Font& a_Font, const sf::Vector2f& a_Size, const sf::Vector2f& a_Position, const sf::Color& a_TextColor, const sf::Texture* a_BackgroundTexture)
{
	InitializeWithData(a_Text, a_Font, a_Size, a_Position);

	m_Shape = std::make_unique<sf::RectangleShape>();
	m_Shape->setTexture(a_BackgroundTexture);
	m_Shape->setTextureRect(sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(static_cast<int>(a_BackgroundTexture->getSize().x), static_cast<int>(a_BackgroundTexture->getSize().y))));
	m_Shape->setSize(a_Size);
	m_Shape->setOrigin(sf::Vector2f(m_Shape->getSize().x / 2, m_Shape->getSize().y / 2));
	m_Shape->setPosition(a_Position);

	m_Text = std::make_unique<sf::Text>(a_Font);
	m_Text->setString(a_Text);
	auto vector2 = m_Text->getGlobalBounds().size;
	m_Text->setOrigin(vector2 / 2.f);
	m_Text->setPosition(a_Position);

	m_Text->setFillColor(a_TextColor);
}

void SpriteWithText::SetText(const std::string& a_Text)
{
	m_Text->setString(a_Text);

	float height = m_Shape->getGlobalBounds().size.y;
	unsigned int size1 = static_cast<unsigned int>(height);
	//m_Text->setCharacterSize(size1/2 - 10);

	auto vector2 = m_Text->getGlobalBounds().size;
	m_Text->setOrigin(vector2 / 2.f);

}

void SpriteWithText::InitializeWithData(const std::string& a_Text, const sf::Font& a_Font, const sf::Vector2f& a_Size, const sf::Vector2f& a_Position)
{
	m_Shape = std::make_unique<sf::RectangleShape>();
	m_Text = std::make_unique<sf::Text>(a_Font);

	m_Shape->setSize(a_Size);
	m_Shape->setOrigin(sf::Vector2f(m_Shape->getSize().x / 2.f, m_Shape->getSize().y / 2.f));
	m_Shape->setPosition(a_Position);


	SetText(a_Text);

	sf::Vector2f pos = a_Position;
	pos.y -= 7.5f;
	m_Text->setPosition(pos);
}
