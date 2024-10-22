#include "Button.h"

#include <SFML/Window/Event.hpp>

#include "Helpers.h"

Button::Button(const sf::Vector2f& a_Position, const sf::Font& a_Font, sf::Texture* a_BaseTexture, sf::Texture* a_ClickedTexture)
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
	sf::FloatRect fr = m_Text->getGlobalBounds();
	float width = m_Shape->getGlobalBounds().width;
	if(fr.width > width)
	{
		unsigned int size = m_Text->getString().getSize();
		unsigned int size1 = static_cast<unsigned int>(width) / size;
		m_Text->setCharacterSize(size1 * 2 - 5);
	}
	m_Text->setOrigin(m_Text->getLocalBounds().getSize() / 2.f);

	sf::Vector2f position = m_Shape->getPosition();
	position.y -= 10;
	m_Text->setPosition(position);

}

bool Button::DetectClick(const sf::Vector2f& a_MousePosition)
{
	if(!m_IsClicked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && m_Shape->getGlobalBounds().contains(a_MousePosition))
	{
		m_Shape->setTexture(*m_ClickedBackground);
		CallAfterDelay::getInstance().AddFunction([this](){DisableClicked();}, 0.2f, false);
		return true;
	}
	m_Shape->setTexture(*m_BaseBackGround);
	return false;
}
