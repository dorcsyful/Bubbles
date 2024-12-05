#include "Button.h"

#include <SFML/Window/Event.hpp>

#include "Audio.h"
#include "Helpers.h"
#include "Settings.h"

Button::Button(const sf::Vector2f& a_Position, const sf::Font& a_Font, sf::Texture* a_BaseTexture)
{
	m_Text = std::make_unique<sf::Text>();
	m_Shape = std::make_unique<sf::Sprite>();

	m_BaseBackGround = a_BaseTexture;

	m_Shape->setTexture(*m_BaseBackGround);
	m_Shape->setTextureRect(sf::IntRect(0, 0, m_BaseBackGround->getSize().x / 3, m_BaseBackGround->getSize().y));
	m_Shape->setOrigin(static_cast<float>(m_BaseBackGround->getSize().x) / 6 , static_cast<float>(m_BaseBackGround->getSize().y) / 2);
	m_Shape->setPosition(a_Position);


	m_Text->setFont(a_Font);
	m_Text->setCharacterSize(40);
	sf::Color temp = sf::Color(100, 55, 0, 255);
	temp = sf::Color::White;
	m_Text->setFillColor(temp);
	m_Text->setStyle(sf::Text::Bold);
	m_Text->setPosition(a_Position);
	m_Text->setOutlineColor(sf::Color(0, 0, 0, 25));
	m_Text->setOutlineThickness(2);

	m_Shader.loadFromFile("Assets/Other/misty_text.glsl", sf::Shader::Fragment);
	m_RenderTexture.create(Settings::get().GetButtonWidth() * 2, Settings::get().GetButtonHeight() * 2);

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
		m_Shape->setTextureRect(sf::IntRect(m_BaseBackGround->getSize().x / 3 * 2, 0, m_BaseBackGround->getSize().x / 3, m_BaseBackGround->getSize().y));
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
	if(m_Shape->getGlobalBounds().contains(a_MousePosition))
	{
		m_Shape->setTextureRect(sf::IntRect(m_BaseBackGround->getSize().x / 3, 0, m_BaseBackGround->getSize().x / 3, m_BaseBackGround->getSize().y));
		return;
	}
	m_Shape->setTextureRect(sf::IntRect(0, 0, m_BaseBackGround->getSize().x / 3, m_BaseBackGround->getSize().y));
}

void Button::ResizeCharacters(unsigned int a_Size) const
{
	m_Text->setCharacterSize(a_Size);
	m_Text->setOrigin(m_Text->getGlobalBounds().getSize() / 2.f + m_Text->getLocalBounds().getPosition());


	m_Text->setPosition(m_Shape->getPosition());
}

void Button::DisableClicked()
{
	m_IsClicked = false;
	m_Shape->setTextureRect(sf::IntRect(0, 0, m_BaseBackGround->getSize().x / 3, m_BaseBackGround->getSize().y));
}
