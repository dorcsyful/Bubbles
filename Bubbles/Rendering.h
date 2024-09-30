#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "BubbleObject.h"

class Rendering
{
public:

	Rendering(int a_X, int a_Y);
	void Draw() const;
	void CreateSprite(EBUBBLE_TYPE a_Size, const sf::Vector2f& a_Position, float a_Rotation,
	                  std::shared_ptr<sf::RectangleShape>& newSprite);

	sf::RenderWindow* GetWindow() const { return m_Window.get(); }

	std::shared_ptr<sf::RectangleShape>& AddSprite(EBUBBLE_TYPE a_Size, const sf::Vector2f& a_Position, float a_Rotation);
	void RemoveSprite(const std::shared_ptr<sf::RectangleShape>& a_SpriteToRemove);

	void MovePointerLine(float a_X);
	void MovePreviewBubble(EBUBBLE_TYPE a_NewPreview);

private:

	void LoadBackground();
	void LoadBubbleTextures();
	void CreateContainerLines();
	void CreatePointer();

	std::shared_ptr<sf::RenderWindow> m_Window;

	std::shared_ptr<sf::RectangleShape> m_BackgroundSprite;
	std::shared_ptr<sf::Texture> m_BackgroundTexture;

	std::vector<std::shared_ptr<sf::RectangleShape>> m_Container;

	std::map<EBUBBLE_TYPE,std::shared_ptr<sf::Texture>> m_BubbleTextures;
	std::vector<std::shared_ptr<sf::RectangleShape>> m_BubbleSprites;

	EBUBBLE_TYPE m_ActiveBubble;
	std::map<EBUBBLE_TYPE, std::shared_ptr<sf::RectangleShape>> m_PreviewBubbles;
	std::shared_ptr<sf::RectangleShape> m_Line;

};

