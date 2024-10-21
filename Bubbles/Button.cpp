#include "Button.h"

#include <SFML/Window/Event.hpp>

Button::Button(const sf::Vector2f& a_Position, const sf::Font& a_Font, const std::shared_ptr<sf::Texture>& a_BaseTexture, const std::shared_ptr<sf::Texture>& a_ClickedTexture)
{
	m_Text = std::make_unique<sf::Text>();
	m_Shape = std::make_unique<sf::Sprite>();

	m_BaseBackGround = a_BaseTexture;
	m_ClickedBackground = a_ClickedTexture;

	m_Shape->setTexture(*m_BaseBackGround);
	m_Shape->setOrigin(static_cast<float>(m_BaseBackGround->getSize().x) / 2.f, static_cast<float>(m_BaseBackGround->getSize().y) / 2.f);
	m_Shape->setPosition(a_Position);


	m_Text->setFont(a_Font);
	m_Text->setCharacterSize(75);
	m_Text->setFillColor(sf::Color::Black);
	m_Text->setStyle(sf::Text::Bold);
}

void Button::SetText(const std::string& a_Text)
{
	m_Text->setString(a_Text);
	m_Text->setOrigin(m_Text->getLocalBounds().getSize() / 2.f);

	sf::Vector2f position = m_Shape->getPosition();
	position.y -= 10;
	m_Text->setPosition(position);

}

bool Button::DetectClick(const sf::Vector2f& a_MousePosition)
{
	if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && m_Shape->getGlobalBounds().contains(a_MousePosition))
	{
		m_Shape->setTexture(*m_ClickedBackground);
		return true;
	}
	m_Shape->setTexture(*m_BaseBackGround);
	return false;
}
