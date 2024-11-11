#include "Button.h"

#include <SFML/Window/Event.hpp>

#include "Audio.h"
#include "Helpers.h"

Button::Button(const sf::Vector2f& a_Position, const sf::Font& a_Font, sf::Texture* a_BaseTexture, sf::Texture* a_ClickedTexture)
{
	m_Text = std::make_unique<sf::Text>();
	m_Shape = std::make_unique<sf::Sprite>();

	m_BaseBackGround = a_BaseTexture;
	m_ClickedBackground = a_ClickedTexture;

	m_Shape->setTexture(*m_BaseBackGround);
	m_Shape->setOrigin(static_cast<float>(m_BaseBackGround->getSize().x) / 2, static_cast<float>(m_BaseBackGround->getSize().y) / 2);
	m_Shape->setPosition(a_Position);


	m_Text->setFont(a_Font);
	m_Text->setCharacterSize(50);
	m_Text->setFillColor(sf::Color::Black);
	m_Text->setStyle(sf::Text::Bold);
	m_Text->setPosition(a_Position);

}

void Button::SetText(const std::string& a_Text) const
{
	m_Text->setString(a_Text);
}

bool Button::DetectClick(const sf::Vector2f& a_MousePosition)
{
	if(!m_IsClicked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && m_Shape->getGlobalBounds().contains(a_MousePosition))
	{
		m_Shape->setTexture(*m_ClickedBackground);
		Audio::getInstance().PlayClick();
		CallAfterDelay::getInstance().AddFunction([this](){DisableClicked();},"DisableClicked", 0.5f, false);
		return true;
	}
	m_Shape->setTexture(*m_BaseBackGround);
	return false;
}

void Button::ResizeCharacters(unsigned int a_Size) const
{
	m_Text->setCharacterSize(a_Size);
	m_Text->setOrigin(m_Text->getGlobalBounds().getSize() / 2.f + m_Text->getLocalBounds().getPosition());


	m_Text->setPosition(m_Shape->getPosition());


}
