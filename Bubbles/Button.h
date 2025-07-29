#pragma once
#include <memory>
#include <string>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

class Button
{
public:
    Button(const sf::Vector2f& a_Position, const sf::Font& a_Font, sf::Texture* a_BaseTexture);
    void SetText(const std::string& a_Text) const;
    void SetScale(const sf::Vector2f& a_Scale) const
    {
        m_Shape->setScale(a_Scale);
    }
    void ApplyBaseTexture() const { m_Shape->setTexture(*m_BaseBackGround); }
    bool DetectClick(const sf::Vector2f& a_MousePosition);
    void DetectHover(const sf::Vector2f& a_MousePosition) const;
    void ResizeCharacters(unsigned int a_Size) const;
    sf::Vector2f GetPosition() const { return m_Shape->getPosition(); }
    float GetTextWidth() const { return m_Text->getGlobalBounds().size.x; }
    float GetWidth() const { return m_Shape->getGlobalBounds().size.x; }
	void Draw(sf::RenderTarget& a_Target) 
	{
        a_Target.draw(*m_Shape);
        a_Target.draw(*m_Text);

    }
private:

    void DisableClicked();
	sf::Texture* m_BaseBackGround;
    std::unique_ptr<sf::Text> m_Text;
    std::unique_ptr<sf::Sprite> m_Shape;

    bool m_IsClicked = false;
};

