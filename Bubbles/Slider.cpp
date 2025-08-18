#include "Slider.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Mouse.hpp>

#include "Audio.h"
#include "BubbleMath.h"
#include "FilePaths.h"
#include "Helpers.h"

Slider::Slider(const sf::Vector2f& a_Position, float a_Size)
{
	m_IsClicked = false;

	m_SliderTexture = std::make_unique<sf::Texture>();
	if (!m_SliderTexture->loadFromFile(SETTINGS_SLIDER)) return;

	m_PointerTexture = std::make_unique<sf::Texture>();
	if (!m_PointerTexture->loadFromFile(SETTINGS_SLIDER_POINTER)) return;


	m_Pointer = std::make_unique<sf::Sprite>(*m_PointerTexture); 
	m_Pointer->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(m_PointerTexture->getSize().x / 3, m_PointerTexture->getSize().y)));
	float size = a_Size / m_PointerTexture->getSize().x;
	m_Pointer->setScale(sf::Vector2f(size, size));
	m_Pointer->setOrigin(sf::Vector2f(m_Pointer->getGlobalBounds().size.x / 2, m_Pointer->getGlobalBounds().size.y));




	m_Slider = std::make_unique<sf::Sprite>(*m_SliderTexture);
	m_Slider->setScale(sf::Vector2f(size, size));
	m_Slider->setOrigin(sf::Vector2f(m_Pointer->getGlobalBounds().size.x / 2,m_Slider->getGlobalBounds().size.y));
	m_Slider->setPosition(sf::Vector2f(a_Position.x,a_Position.y + m_Slider->getGlobalBounds().size.y));
	




	m_Pointer->setPosition(m_Slider->getPosition());

}

void Slider::DetectHover(const sf::Vector2f& a_MousePosition) const
{
	sf::Vector2u size = m_PointerTexture->getSize();

	if (m_IsClicked) return;
	if (m_Pointer->getGlobalBounds().contains(a_MousePosition))
	{
		m_Pointer->setTextureRect(sf::IntRect(sf::Vector2i(size.x / 3, 0), sf::Vector2i(size.x / 3, size.y)));
		return;
	}
	m_Pointer->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(size.x / 3, size.y)));
}

bool Slider::DetectClick(const sf::Vector2f& a_MousePosition, bool a_IsPressed)
{
	sf::Vector2u size = m_PointerTexture->getSize();

	if (!m_IsClicked && a_IsPressed && m_Pointer->getGlobalBounds().contains(a_MousePosition))
	{
		m_Pointer->setTextureRect(sf::IntRect(sf::Vector2i(size.x / 3 * 2, 0), sf::Vector2i(size.x / 3, size.y)));
		m_IsClicked = true;
		return true;
	}
	if (m_IsClicked && a_IsPressed)
	{
		float x1 = m_Pointer->getGlobalBounds().size.x / 2;
		float x = std::clamp(a_MousePosition.x, m_Slider->getPosition().x, m_Slider->getPosition().x + m_Slider->getGlobalBounds().size.x - m_Pointer->getGlobalBounds().size.x);
		m_Pointer->setPosition(sf::Vector2f(x, m_Slider->getPosition().y));
		return true;
	}
	m_Pointer->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(size.x / 3, size.y)));
	m_IsClicked = false;
	return false;
}

float Slider::GetSliderValue()
{
	float x1 = (m_Pointer->getPosition().x - (m_Slider->getPosition().x));
	float width = m_Slider->getGlobalBounds().size.x - m_Pointer->getGlobalBounds().size.x;
	float x = std::floorf((x1 / width) * 100);
	return x;
}

void Slider::SetSliderValue(float a_NewVolume)
{
	float x = a_NewVolume / 100;
	auto start = m_Slider->getPosition();
	sf::Vector2f end(start.x + m_Slider->getGlobalBounds().size.x - m_Pointer->getGlobalBounds().size.x / 2.f, start.y);
	float width =  BubbleMath::Lerp(start, end, x).x;
	m_Pointer->setPosition(sf::Vector2f(width, start.y));
}

void Slider::draw(sf::RenderTarget& a_Target, const sf::RenderStates a_States) const
{
	a_Target.draw(*m_Slider, a_States);
	a_Target.draw(*m_Pointer, a_States);

}
