#include "SpriteWithText.h"

SpriteWithText::SpriteWithText(const sf::Font& a_Font)
{
	m_Shape = std::make_unique<sf::RectangleShape>();
	m_Text = std::make_unique<sf::Text>();
	m_Text->setFont(a_Font);
}

SpriteWithText::SpriteWithText(const std::string& a_Text, const sf::Font& a_Font, const sf::Vector2f& a_Size, const sf::Vector2f& a_Position, const sf::Color& a_TextColor, const sf::Color& a_ShapeBackground)
{
	m_Shape = std::make_unique<sf::RectangleShape>();
	m_Text = std::make_unique<sf::Text>();
	InitializeWithData(a_Text, a_Font, a_Size, a_Position);

	m_Shape->setFillColor(a_ShapeBackground);
	m_Text->setFillColor(a_TextColor);
}

SpriteWithText::SpriteWithText(const std::string& a_Text, const sf::Font& a_Font, const sf::Vector2f& a_Size, const sf::Vector2f& a_Position, const sf::Color& a_TextColor, const sf::Texture* a_BackgroundTexture)
{
	InitializeWithData(a_Text, a_Font, a_Size, a_Position);
	m_Shape->setTexture(a_BackgroundTexture);
	m_Text->setFillColor(a_TextColor);
}

void SpriteWithText::SetText(const std::string& a_Text)
{
	m_Text->setString(a_Text);

	sf::FloatRect fr = m_Text->getGlobalBounds();
	if (float width = m_Shape->getGlobalBounds().width; fr.width > width)
	{
		unsigned int size = m_Text->getString().getSize();
		unsigned int size1 = static_cast<unsigned int>(width) / size;
		m_Text->setCharacterSize(size1 * 2 - 10);
	}

	m_Text->setOrigin(m_Text->getLocalBounds().getSize() / 2.f);

}

void SpriteWithText::InitializeWithData(const std::string& a_Text, const sf::Font& a_Font, const sf::Vector2f& a_Size, const sf::Vector2f& a_Position)
{
	m_Shape = std::make_unique<sf::RectangleShape>();
	m_Text = std::make_unique<sf::Text>();

	m_Shape->setSize(a_Size);
	m_Shape->setOrigin(m_Shape->getSize().x / 2, m_Shape->getSize().y / 2);
	m_Shape->setPosition(a_Position);

	m_Text->setFont(a_Font);
	m_Text->setString(a_Text);

	sf::FloatRect fr = m_Text->getGlobalBounds();
	if (float width = m_Shape->getGlobalBounds().width; fr.width > width)
	{
		unsigned int size = m_Text->getString().getSize();
		unsigned int size1 = static_cast<unsigned int>(width) / size;
		m_Text->setCharacterSize(size1 * 2 - 10);
	}

	m_Text->setOrigin(m_Text->getLocalBounds().getSize() / 2.f);
	m_Text->setPosition(a_Position);
}
