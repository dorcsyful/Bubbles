#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

class Rendering
{
public:

	Rendering(int a_x, int a_y);
	void draw() const;

	sf::RenderWindow* GetWindow() const { return m_window.get(); }

	std::shared_ptr<sf::RectangleShape>& AddSprite(int a_size, const sf::Vector2f& a_position, float a_rotation);
	void RemoveSprite(const std::shared_ptr<sf::RectangleShape>& a_spriteToRemove);

private:

	void LoadBackground();
	void LoadBubbleTextures();
	void CreateContainerLines();

	std::shared_ptr<sf::RenderWindow> m_window;

	std::shared_ptr<sf::RectangleShape> m_backgroundSprite;
	std::shared_ptr<sf::Texture> m_backgroundTexture;

	std::vector<std::shared_ptr<sf::RectangleShape>> m_container;

	std::vector<std::shared_ptr<sf::Texture>> m_bubbleTextures;
	std::vector<std::shared_ptr<sf::RectangleShape>> m_bubbleSprites;

};

