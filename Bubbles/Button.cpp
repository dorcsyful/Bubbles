#include "Button.h"

#include <SFML/Window/Event.hpp>

#include "Audio.h"
#include "BubbleMath.h"
#include "Helpers.h"

Button::Button(const sf::Vector2f& a_Position, const sf::Font& a_Font, sf::Texture* a_BaseTexture)
{
	m_Text = std::make_unique<sf::Text>();
	m_Shape = std::make_unique<sf::Sprite>();

	m_BaseBackGround = a_BaseTexture;

	m_Shape->setTexture(*m_BaseBackGround);
	sf::Vector2i size = sf::Vector2i(m_BaseBackGround->getSize());
	m_Shape->setTextureRect(sf::IntRect(0, 0, size.x / 3, size.y));
	m_Shape->setOrigin(static_cast<float>(size.x) / 6 , static_cast<float>(size.y) / 2);
	m_Shape->setPosition(a_Position);


	m_Text->setFont(a_Font);
	m_Text->setCharacterSize(40);
	sf::Color temp = sf::Color::White;
	m_Text->setFillColor(temp);
	m_Text->setStyle(sf::Text::Bold);
	m_Text->setPosition(a_Position);
	m_Text->setOutlineColor(sf::Color(0, 0, 0, 25));
	m_Text->setOutlineThickness(2);

}

void Button::SetText(const std::string& a_Text) const
{
	m_Text->setString(a_Text);
}

bool Button::DetectClick(const sf::Vector2f& a_MousePosition)
{
	if(!m_IsClicked && m_Shape->getGlobalBounds().contains(a_MousePosition))
	{
		Audio::getInstance().PlayClick();
		sf::Vector2i size = sf::Vector2i(m_BaseBackGround->getSize());
		m_Shape->setTextureRect(sf::IntRect(size.x / 3 * 2, 0, size.x / 3, size.y));
		CallAfterDelay::getInstance().AddFunction([this]() { DisableClicked(); }, "DisableClicked", 0.1f, false);

		m_IsClicked = true;
		return true;
	}
	m_Shape->setTexture(*m_BaseBackGround);
	return false;
}

void Button::DetectHover(const sf::Vector2f& a_MousePosition) const
{
	if (m_IsClicked) return;

	sf::Vector2i size = sf::Vector2i(m_BaseBackGround->getSize());
	if(m_Shape->getGlobalBounds().contains(a_MousePosition))
	{
		m_Shape->setTextureRect(sf::IntRect(size.x / 3, 0, size.x / 3, size.y));
		return;
	}
	m_Shape->setTextureRect(sf::IntRect(0, 0, size.x / 3,size.y));
}

void Button::ResizeCharacters(unsigned int a_Size) const
{
	m_Text->setCharacterSize(a_Size);
	m_Text->setOrigin(m_Text->getGlobalBounds().getSize() / 2.f + m_Text->getLocalBounds().getPosition());


	m_Text->setPosition(m_Shape->getPosition());
}

void Button::DisableClicked()
{
	sf::Vector2i size = sf::Vector2i(m_BaseBackGround->getSize());
	m_IsClicked = false;
	m_Shape->setTextureRect(sf::IntRect(0, 0, size.x / 3, size.y));
}
