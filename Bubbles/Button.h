#pragma once
#include <memory>
#include <string>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

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
    float GetTextWidth() const { return m_Text->getGlobalBounds().width; }
	void Draw(sf::RenderTarget& a_Target) 
	{
        // You can draw other high-level objects
        a_Target.draw(*m_Shape);
        a_Target.draw(*m_Text);
        return;
        m_RenderTexture.clear(sf::Color::Transparent);
        m_RenderTexture.draw(*m_Text);
        m_RenderTexture.display();

        // Draw the text sprite with the shader
        sf::Sprite sprite = sf::Sprite();
    	sprite.setTexture(m_RenderTexture.getTexture());
        sprite.setPosition(m_Shape->getPosition());
    	m_Shader.setUniform("u_Texture", m_RenderTexture.getTexture());
        a_Target.draw(sprite, &m_Shader);

    }
private:

    void DisableClicked();
    sf::RenderTexture m_RenderTexture;
    sf::Shader m_Shader;
	sf::Texture* m_BaseBackGround;
    std::unique_ptr<sf::Text> m_Text;
    std::unique_ptr<sf::Sprite> m_Shape;

    bool m_IsClicked = false;
};

