#pragma once
#include <memory>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

class Checkbox : public sf::Drawable
{
public:
	Checkbox(const std::unique_ptr<sf::Texture>& a_Texture, const sf::Vector2f& a_Position, float a_Width);

	void SetEnableCheckbox(bool a_Enable);
	sf::Vector2f GetPosition() { return m_Sprite->getPosition(); }
	bool DetectClick(const sf::Vector2f& a_MousePosition);
	void DetectHover(const sf::Vector2f& a_MousePosition) const;
	bool IsChecked() const { return m_IsChecked; }
private:
	void draw(sf::RenderTarget& a_Target, const sf::RenderStates a_States) const override
	{
		// You can draw other high-level objects
		a_Target.draw(*m_Sprite, a_States);
	}

	std::unique_ptr<sf::Sprite> m_Sprite;
	bool isCooldown;
	bool m_IsChecked;
	bool m_IsHovered;
};

