#pragma once
#include <memory>
#include <SFML/Graphics/RectangleShape.hpp>

class Slider : public sf::Drawable
{
public:
	Slider(const sf::Vector2f& a_Position, const sf::Vector2f& a_Size, const sf::Color& a_BaseColor,
			const sf::Color& a_HoverColor, const sf::Color& a_ClickedColor, const sf::Color& a_SliderColor);

	void DetectHover(const sf::Vector2f& a_MousePosition) const;
	bool DetectClick(const sf::Vector2f& a_MousePosition, bool a_IsPressed);
	sf::Vector2f GetSliderPosition() const { return m_Slider->getPosition(); }
	float GetSliderValue();
	void SetSliderValue(float a_NewVolume);

private:
	void draw(sf::RenderTarget& a_Target, const sf::RenderStates a_States) const override;

	bool m_IsClicked;
	sf::Color m_Color[3];
	std::unique_ptr<sf::RectangleShape> m_Slider;
	std::unique_ptr<sf::RectangleShape> m_Pointer;

};

