#pragma once
#include <memory>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

class Slider : public sf::Drawable
{
public:
	Slider(const sf::Vector2f& a_Position, float a_Size);

	void DetectHover(const sf::Vector2f& a_MousePosition) const;
	bool DetectClick(const sf::Vector2f& a_MousePosition, bool a_IsPressed);
	sf::Vector2f GetSliderPosition() const { return m_Slider->getPosition(); }
	float GetSliderValue();
	void SetSliderValue(float a_NewVolume);

private:
	void draw(sf::RenderTarget& a_Target, const sf::RenderStates a_States) const override;

	bool m_IsClicked;
	std::unique_ptr<sf::Texture> m_PointerTexture;
	std::unique_ptr<sf::Texture> m_SliderTexture;
	std::unique_ptr<sf::Sprite> m_Slider;
	std::unique_ptr<sf::Sprite> m_Pointer;

};

