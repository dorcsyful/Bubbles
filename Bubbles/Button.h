#pragma once
#include <memory>
#include <string>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class Button : public sf::Drawable
{
public:
    Button(const sf::Vector2f& a_Position, const sf::Font& a_Font, const std::shared_ptr<sf::Texture>& a_BaseTexture, const std::shared_ptr<sf::Texture>& a_ClickedTexture);
    void SetText(const std::string& a_Text);

    bool DetectClick(const sf::Vector2f& a_MousePosition);

private:
	void draw(sf::RenderTarget& a_Target, const sf::RenderStates a_States) const override
	{
        // You can draw other high-level objects
        a_Target.draw(*m_Shape, a_States);
        a_Target.draw(*m_Text, a_States);
    }
    std::shared_ptr<sf::Texture> m_BaseBackGround;
    std::shared_ptr<sf::Texture> m_ClickedBackground;
    std::unique_ptr<sf::Text> m_Text;
    std::unique_ptr<sf::Sprite> m_Shape;
};

