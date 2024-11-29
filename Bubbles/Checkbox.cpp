#include "Checkbox.h"

#include <SFML/Window/Mouse.hpp>

#include "Audio.h"
#include "BubbleMath.h"
#include "Settings.h"

Checkbox::Checkbox(const std::unique_ptr<sf::Texture>& a_Texture, const sf::Vector2f& a_Position, float a_Width)
{
	m_Sprite = std::make_unique<sf::Sprite>();
	m_Sprite->setTexture(*a_Texture);
	sf::Vector2i spriteSize = sf::Vector2i(m_Sprite->getTexture()->getSize());
	m_Sprite->setTextureRect(sf::IntRect(0, 0, spriteSize.x / 4, spriteSize.y));
	auto vector2 = BubbleMath::ToVector2f(a_Texture->getSize());
	m_Sprite->setScale(a_Width / (vector2.x / 4), a_Width / vector2.y);
	m_Sprite->setOrigin(m_Sprite->getLocalBounds().getSize().x / 2, m_Sprite->getLocalBounds().getSize().y / 2);
	m_Sprite->setPosition(a_Position);
	m_IsChecked = false;
	m_IsHovered = false;
}

void Checkbox::SetEnableCheckbox(bool a_Enable)
{
	m_IsChecked = a_Enable;
	auto vector2U = sf::Vector2i(m_Sprite->getTexture()->getSize());
	int rect = m_IsChecked ? 1 : 0;

	m_Sprite->setTextureRect(sf::IntRect(vector2U.x / 4 * rect, 0, vector2U.x / 4, vector2U.y));
}

bool Checkbox::DetectClick(const sf::Vector2f& a_MousePosition)
{
	if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && m_Sprite->getGlobalBounds().contains(a_MousePosition))
	{
		Audio::getInstance().PlayClick();
		int rect = m_IsChecked ? 0 : 1;
		int x = m_Sprite->getTexture()->getSize().x;
		m_Sprite->setTextureRect(sf::IntRect(x / 4 * rect, 0, 
		                                     x / 4, static_cast<int>(m_Sprite->getTexture()->getSize().y)));

		m_IsChecked = !m_IsChecked;
		std::cout << std::to_string(m_IsChecked) << std::endl;
		return true;
	}
	return false;
}

void Checkbox::DetectHover(const sf::Vector2f& a_MousePosition) const
{
	int rect = m_IsChecked ? 3 : 2;
	sf::Vector2i vector2U = sf::Vector2i(m_Sprite->getTexture()->getSize());

	if (m_Sprite->getGlobalBounds().contains(a_MousePosition))
	{
		m_Sprite->setTextureRect(sf::IntRect(vector2U.x / 4 * rect, 0, vector2U.x / 4, vector2U.y));
		return;
	}
	rect = m_IsChecked ? 1 : 0;
	m_Sprite->setTextureRect(sf::IntRect(vector2U.x / 4 * (rect), 0, vector2U.x / 4, vector2U.y));
}
