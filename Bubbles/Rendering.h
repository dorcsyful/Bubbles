#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "AnimatedSprite.h"
#include "BubbleObject.h"
#include "Button.h"

class LineObject;

class Rendering
{
public:

	Rendering(int a_X, int a_Y);
	void Draw(EGAME_STATE a_State) const;
	void CreateSprite(EBUBBLE_TYPE a_Size, const sf::Vector2f& a_Position, float a_Rotation,
	                  std::shared_ptr<AnimatedSprite>& a_NewSprite);

	sf::RenderWindow* GetWindow() const { return m_Window.get(); }
	sf::Vector2f GetPreviewPosition() const { return m_PreviewBubbles.at(m_ActiveBubble)->GetPosition(); }
	std::shared_ptr<AnimatedSprite>& AddSprite(EBUBBLE_TYPE a_Size, const sf::Vector2f& a_Position, float a_Rotation);
	void RemoveSprite(const std::shared_ptr<AnimatedSprite>& a_SpriteToRemove);
	
	void MovePointerLine(float a_X);
	void MovePreviewBubble(EBUBBLE_TYPE a_NewPreview);

	std::vector<std::shared_ptr<Button>> GetMenuButtons() { return m_MenuButtons; }

	std::vector<std::shared_ptr<LineObject>> ConvertToLine();
	std::shared_ptr<LineObject> ConvertTopLine();

private:
	void PlayDraw() const;
	void MenuDraw() const;
	void GameOverAnimationDraw() const;
	void LoadBackground();
	void LoadBubbleTextures();
	void CreateContainerLines();
	void CreatePointer();
	void CreateTitleSprite();
	void CreateGameOverSprite();

	void CreateMenuSprites();
	void CreateMenuButtonSprites();

	std::shared_ptr<sf::RenderWindow> m_Window;

	std::shared_ptr<sf::RectangleShape> m_BackgroundSprite;
	std::shared_ptr<sf::Texture> m_BackgroundTexture;

	//Play mode
	std::vector<std::shared_ptr<sf::RectangleShape>> m_Container;
	std::map<EBUBBLE_TYPE,std::shared_ptr<sf::Texture>> m_BubbleTextures;
	std::vector<std::shared_ptr<AnimatedSprite>> m_BubbleSprites;
	EBUBBLE_TYPE m_ActiveBubble;
	std::map<EBUBBLE_TYPE, std::shared_ptr<AnimatedSprite>> m_PreviewBubbles;
	std::shared_ptr<sf::RectangleShape> m_Line;

	//Play mode UI

	//Game over mode
	std::shared_ptr<sf::RectangleShape> m_GameOver;
	std::shared_ptr<sf::Texture> m_GameOverTexture;

	//Menu mode
	std::shared_ptr<sf::RectangleShape> m_Title;
	std::shared_ptr<sf::Texture> m_TitleTexture;
	std::vector<std::shared_ptr<Button>> m_MenuButtons;
	std::shared_ptr<sf::Font> m_Font;
	std::shared_ptr<AnimatedSprite> m_Loading;
};

