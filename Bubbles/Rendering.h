#pragma once
#include <iostream>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "AnimatedSprite.h"
#include "BubbleObject.h"
#include "Button.h"
#include "Checkbox.h"
#include "Slider.h"
#include "SpriteWithText.h"

class LineObject;

class Rendering
{
public:

	Rendering(int a_X, int a_Y, std::vector<std::unique_ptr<AnimatedSprite>>& a_Wrapper);
	void Draw(EGAME_STATE a_State) const;
	void CreateSprite(EBUBBLE_TYPE a_Type, const sf::Vector2f& a_Position, float a_Rotation,
	                  std::unique_ptr<AnimatedSprite>& a_NewSprite) const;

	sf::RenderWindow* GetWindow() const { return m_Window.get(); }
	sf::Vector2f GetPreviewPosition() const { return m_PreviewBubbles.at(m_ActiveBubble)->GetPosition(); }
	
	void MovePointerLine(float a_X) const;
	void MovePreviewBubble(EBUBBLE_TYPE a_NewPreview);

	std::map<std::string, std::unique_ptr<Button>>& GetMenuButtons() { return m_MenuButtons; }
	void ResetButtons() const;

	void UpdateScore(const unsigned int a_Score) const
	{
		m_Score->setString(std::to_string(a_Score));
	}
	void UpdateHighScores(const std::vector<unsigned int>& a_Scores) const;
	void UpdateCombo(unsigned int a_Combo) const
	{
		if (a_Combo > 5)
		{
			std::string basicString = std::to_string(a_Combo);
			m_ComboText->setString("Combo:\n +" + basicString);
		}
		else
		{
			m_ComboText->setString("");
		}
	}
	void UpScaleComboText(float a_Amount) const { m_ComboText->setScale(a_Amount, a_Amount); }
	void UpdateNextUp(EBUBBLE_TYPE a_Type) { m_ActiveNextUp = a_Type; }
	void UpdateConfirmText(EGAME_STATE a_NewState) const;
	void UpdateComboPosition(const sf::Vector2f& a_NewPos) const;

	std::unique_ptr<Button>& GetHSBackButton() { return m_HSBackButton; }
	std::unique_ptr<Slider>& GetSettingSlider(int a_Id) { return m_SettingSliders[a_Id]; }
	std::unique_ptr<AnimatedSprite>& GetDuck() { return m_Duck; }
	std::unique_ptr<Checkbox>& GetFullscreenCheckbox() { return m_FullscreenCheckbox; }

	void Reset();

private:
	void PlayDraw() const;
	void MenuDraw() const;
	void GameOverAnimationDraw() const;
	void HighScoreDraw() const;
	void GameOverDraw() const;
	void ConfirmationDraw() const;
	void SettingsDraw() const;

	void LoadBackground();
	void LoadBubbleTextures();
	void LoadNextUpTextures();

	void CreatePointer();
	void CreateTitleSprite();
	void CreateGameOverSprite();
	void CreateMenuSprites();
	void CreateMenuButtonSprites();
	void CreateScoreText();
	void CreateHighScoreSprites();
	void CreateDuck();
	void CreateNextUpSprites();
	void CreatePlayScoreSprites();
	void CreatePlayModeButtons();
	void CreateConfirmationWindow();
	void CreateSettingsButtons();
	void CreateSettings();

	std::unique_ptr<sf::RenderWindow> m_Window;

	std::unique_ptr<sf::RectangleShape> m_BackgroundSprite;
	std::unique_ptr<sf::Texture> m_BackgroundTexture;
	std::unique_ptr<sf::RectangleShape> m_MainBackgroundSprite;
	std::unique_ptr<sf::Texture> m_MainBackgroundTexture;

	//Play mode
	std::vector<std::unique_ptr<AnimatedSprite>>& m_RenderedBubbles;
	std::unique_ptr<sf::Texture> m_ContainerTexture;
	std::unique_ptr<sf::RectangleShape> m_Container;
	std::unique_ptr<sf::Texture> m_FrameTexture;
	std::unique_ptr<sf::RectangleShape> m_Frame;

	std::map<EBUBBLE_TYPE,std::unique_ptr<sf::Texture>> m_BubbleTextures;
	std::map<EBUBBLE_TYPE,std::unique_ptr<sf::Texture>> m_NextUpTextures;
	EBUBBLE_TYPE m_ActiveBubble;
	EBUBBLE_TYPE m_ActiveNextUp;
	std::map<EBUBBLE_TYPE, std::unique_ptr<AnimatedSprite>> m_PreviewBubbles;
	std::map<EBUBBLE_TYPE, std::unique_ptr<sf::Sprite>> m_NextUpBubbles;

	std::unique_ptr<sf::RectangleShape> m_Line;
	std::unique_ptr<sf::Texture> m_DuckTexture;
	std::unique_ptr<AnimatedSprite> m_Duck;

	//Play mode UI
	std::unique_ptr<sf::Texture> m_ScoreBackgroundTexture;
	std::unique_ptr<sf::Text> m_ComboText;
	std::unique_ptr<sf::Text> m_Score;
	std::unique_ptr<sf::RectangleShape> m_ScoreTitle;
	std::unique_ptr<sf::RectangleShape> m_HighScoreTitleInPlay;
	std::unique_ptr<sf::Texture> m_ScoreTitleTexture;
	std::unique_ptr<SpriteWithText> m_HighScoresInPlay;

	//Game over mode
	std::unique_ptr<sf::RectangleShape> m_GameOver;
	std::unique_ptr<sf::Texture> m_GameOverTexture;
	std::unique_ptr<SpriteWithText> m_GOScoreSprite;

	//Menu mode
	std::unique_ptr<sf::RectangleShape> m_Title;
	std::unique_ptr<sf::Texture> m_TitleTexture;
	std::map<std::string, std::unique_ptr<Button>> m_MenuButtons;
	std::unique_ptr<sf::Font> m_Font;
	std::unique_ptr<AnimatedSprite> m_Loading;
	std::unique_ptr<sf::Texture> m_LoadingTexture;
	std::unique_ptr<sf::Texture> m_BaseButtonTexture;

	//HighScore
	std::vector<std::unique_ptr<SpriteWithText>> m_HighScoreSprites;
	std::unique_ptr<sf::RectangleShape> m_HighScoreTitle;
	std::unique_ptr<sf::Texture> m_HighScoreTexture;
	std::unique_ptr<Button> m_HSBackButton;

	//Confirm
	std::unique_ptr<sf::RectangleShape> m_ConfirmationWindow;
	std::unique_ptr<sf::Texture> m_ConfirmationTexture;
	std::unique_ptr<sf::Text> m_ConfirmationText;

	//Settings
	std::unique_ptr<sf::RectangleShape> m_SettingsTitle;
	std::unique_ptr<sf::Texture> m_SettingsTexture;
	std::vector<std::unique_ptr<sf::Text>> m_SettingsText;
	std::vector<std::unique_ptr<Slider>> m_SettingSliders;
	std::unique_ptr<sf::Texture> m_CheckboxTexture;
	std::unique_ptr<Checkbox> m_FullscreenCheckbox;
};

