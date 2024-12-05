#include "Slider.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Mouse.hpp>

#include "Audio.h"
#include "BubbleMath.h"
#include "Helpers.h"

Slider::Slider(const sf::Vector2f& a_Position, const sf::Vector2f& a_Size, const sf::Color& a_BaseColor,
	const sf::Color& a_HoverColor, const sf::Color& a_ClickedColor, const sf::Color& a_SliderColor)
{
	m_IsClicked = false;
	m_Color[0] = a_BaseColor;
	m_Color[1] = a_HoverColor;
	m_Color[2] = a_ClickedColor;
	m_Pointer = std::make_unique<sf::RectangleShape>(sf::Vector2f(a_Size.x / 33, a_Size.y * 2.f)); 
	m_Pointer->setFillColor(m_Color[0]);
	m_Pointer->setOrigin(m_Pointer->getSize().x / 2, m_Pointer->getSize().y / 2);

	m_Slider = std::make_unique<sf::RectangleShape>(a_Size);
	m_Slider->setFillColor(a_SliderColor);
	m_Slider->setPosition(a_Position);
	m_Slider->setOrigin(m_Pointer->getSize().x / 2, m_Slider->getSize().y / 2);

	m_Pointer->setPosition(m_Slider->getPosition());

}

void Slider::DetectHover(const sf::Vector2f& a_MousePosition) const
{
	if (m_IsClicked) return;
	if (m_Pointer->getGlobalBounds().contains(a_MousePosition))
	{
		m_Pointer->setFillColor(m_Color[1]);
		return;
	}
	m_Pointer->setFillColor(m_Color[0]);
}

bool Slider::DetectClick(const sf::Vector2f& a_MousePosition, bool a_IsPressed)
{

	if (!m_IsClicked && a_IsPressed && m_Pointer->getGlobalBounds().contains(a_MousePosition))
	{
		m_Pointer->setFillColor(m_Color[2]);
		m_IsClicked = true;
		return true;
	}
	if (m_IsClicked && a_IsPressed)
	{
		float x1 = m_Pointer->getSize().x / 2;
		float x = std::clamp(a_MousePosition.x, m_Slider->getPosition().x, m_Slider->getPosition().x + m_Slider->getSize().x - m_Pointer->getSize().x);
		m_Pointer->setPosition(x, m_Slider->getPosition().y);
		return true;
	}
	m_Pointer->setFillColor(m_Color[0]);
	m_IsClicked = false;
	return false;
}

float Slider::GetSliderValue()
{
	float x1 = (m_Pointer->getPosition().x - (m_Slider->getPosition().x));
	float width = m_Slider->getGlobalBounds().width - m_Pointer->getGlobalBounds().width;
	float x = std::floorf((x1 / width) * 100);
	return x;
}

void Slider::SetSliderValue(float a_NewVolume)
{
	float x = a_NewVolume / 100;
	auto start = m_Slider->getPosition();
	sf::Vector2f end(start.x + m_Slider->getGlobalBounds().width - m_Pointer->getSize().x / 2.f, start.y);
	float width =  BubbleMath::Lerp(start, end, x).x;
	m_Pointer->setPosition(width, start.y);
}

void Slider::draw(sf::RenderTarget& a_Target, const sf::RenderStates a_States) const
{
	a_Target.draw(*m_Slider, a_States);
	a_Target.draw(*m_Pointer, a_States);

}
