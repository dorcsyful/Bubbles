#include "Button.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Audio.h"
#include "Helpers.h"
#include "Settings.h"

Button::Button(const sf::Vector2f& a_Position, const sf::Font& a_Font, sf::Texture* a_BaseTexture)
{
	m_Text = std::make_unique<sf::Text>(a_Font);
	m_Shape = std::make_unique<sf::Sprite>(*a_BaseTexture);

	m_BaseBackGround = a_BaseTexture;

	sf::Vector2i size = sf::Vector2i(m_BaseBackGround->getSize());
	m_Shape->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(size.x / 3, size.y)));
	m_Shape->setOrigin(sf::Vector2f(static_cast<float>(size.x) / 6 , static_cast<float>(size.y) / 2));
	m_Shape->setPosition(a_Position);


	m_Text->setFont(a_Font);
	m_Text->setCharacterSize(static_cast<int>(38 * Settings::get().GetScale()));
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
		m_Shape->setTextureRect(sf::IntRect(sf::Vector2i(size.x / 3 * 2, 0), sf::Vector2i(size.x / 3, size.y)));
		CallAfterDelay::getInstance().AddFunction([this]() { DisableClicked(); }, "DisableClicked", 0.5f, false);

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
		m_Shape->setTextureRect(sf::IntRect(sf::Vector2i(size.x / 3, 0), sf::Vector2i(size.x / 3, size.y)));
		return;
	}
	m_Shape->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(size.x / 3,size.y)));
}

void Button::ResizeCharacters(unsigned int a_Size) const
{
	m_Text->setCharacterSize(a_Size);
	m_Text->setOrigin(m_Text->getGlobalBounds().size / 2.f + m_Text->getLocalBounds().position);


	m_Text->setPosition(m_Shape->getPosition());
}

void Button::DisableClicked()
{
	sf::Vector2i size = sf::Vector2i(m_BaseBackGround->getSize());
	m_IsClicked = false;
	m_Shape->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(size.x / 3, size.y)));
}
