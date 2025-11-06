// ReSharper disable CppNoDiscardExpression
#include "Declarations.h"

#include "Rendering.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include "Audio.h"
#include "BubbleGame.h"
#include "BubbleObject.h"
#include "FilePaths.h"
#include "LineObject.h"
#include "Settings.h"
#include <SFML/Graphics/Image.hpp>


Rendering::Rendering(const int a_X, const int a_Y, std::vector<std::unique_ptr<AnimatedSprite>>& a_Wrapper):
	m_RenderedBubbles(a_Wrapper)
{
	sf::ContextSettings context;
	context.antiAliasingLevel = Settings::get().GetAaLevel();

	//if(Settings::get().IsFullscreen())
	//{
	//	m_Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(sf::VideoMode::getDesktopMode().size.x, sf::VideoMode::getDesktopMode().size.y)), "Bubbles!",
	//		sf::Style::Default,sf::State::Fullscreen, context);
	//	Settings::get().IncreaseIfFullScreen(static_cast<float>(sf::VideoMode::getDesktopMode().size.x), static_cast<float>(sf::VideoMode::getDesktopMode().size.y));

	//}
	//else
	//{
		std::pair<int, int> resSizeAsPair = Settings::get().GetResSizeAsPair(Settings::get().GetCurrentRes());

		Settings::get().IncreaseIfFullScreen(static_cast<float>(resSizeAsPair.first), static_cast<float>(resSizeAsPair.second));
		sf::Vector2u size = sf::Vector2u(static_cast<int>(Settings::get().GetWindowWidth()), static_cast<int>(Settings::get().GetWindowHeight()));
		m_Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(size.x, size.y)), "Bubbles!",
			sf::Style::Titlebar | sf::Style::Close
			,sf::State::Windowed, context);

	//}
	m_Window->setVerticalSyncEnabled(true);


	LoadBackground();
	LoadBubbleTextures();
	LoadNextUpTextures();
	LoadStorageTextures();
	CreatePointer();
	CreateMenuSprites();
	CreateHighScoreSprites();
	CreateScoreText();

	CreateDuck();
	CreatePlayModeButtons();
	CreateConfirmationWindow();
	CreateSettings();
	CreateCycleBottle();
	CreateStorageSprites();
	CreateNextUpSprites();
	CreateScoreNumberSprites();

	CreateGameOverSprite();

	m_Icon = std::make_unique<sf::Image>();
	if (!m_Icon->loadFromFile(ICON_PATH))
	{
		// Error handling...
	}

	m_Window->setIcon(m_Icon->getSize(), m_Icon->getPixelsPtr());

	m_DebugCircle = std::make_unique<sf::CircleShape>();
	m_DebugCircle->setFillColor(sf::Color::Red);
	m_DebugCircle->setRadius(10.f);
	m_DebugCircle->setOrigin(sf::Vector2f(5, 5));
	SetupView();
}

void Rendering::FinishMoveToStorage()
{
	EBUBBLE_TYPE toStore = m_TypeInStorage == EBUBBLE_TYPE::TYPE_SPIKY_BOMB ? EBUBBLE_TYPE::TYPE_BATH_BOMB : m_TypeInStorage;
	if (m_MovingDirection > 0)
	{
		m_StoredSprite->setTexture(*m_StorageTextures.at(toStore));
	}
	else
	{
		m_StoredSprite->setTexture(*m_StorageTextures.at(EBUBBLE_TYPE::TYPE_NULL));
	}

	m_MovingDirection = 0.0f;
	m_MovingStorageLerp = 0.0f;
}

void Rendering::PlayDraw(float a_Delta)
{
	m_ParticleSystem->Update(a_Delta);

	m_Window->draw(*m_BackgroundExtendedSprite);
	m_Window->draw(*m_BackgroundSprite);

	sf::Vector2f mousePosition = m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window));
	m_MenuButtons.at("Back to menu")->DetectHover(mousePosition);
	m_MenuButtons.at("Restart")->DetectHover(mousePosition);

	m_Window->draw(*m_Container);
	m_Window->draw(*m_Frame);
	m_Window->draw(*m_Line);

	m_Duck->Draw(*m_Window);
	m_Window->draw(*m_ScoreBackgroundInPlaySprite);

	m_PreviewBubbles.at(m_ActiveBubble)->Draw(*m_Window);
	
	m_Window->draw(*m_HighScoreSpriteInPlay);

	m_MenuButtons.at("Back to menu")->Draw(*m_Window);
	m_MenuButtons.at("Restart")->Draw(*m_Window);
	//m_Window->draw(*m_Score);
	m_Window->draw(*m_ScoreTitle);
	//m_Window->draw(*m_HighScoreTitleInPlay);
	m_Window->draw(*m_StoredSprite);
	m_Window->draw(*m_StorageText);


	float lastPosition = m_ScoreStartPosition;
	std::string scoreString = m_Score->getString();
	for (size_t i = 0; i < scoreString.size(); i++)
	{
		unsigned char asChar = scoreString[i];
		int value = asChar - '0';
		m_ScoreNumberSprites[value]->setPosition(sf::Vector2f(lastPosition, m_ScoreNumberSprites[value]->getPosition().y));
		m_Window->draw(*m_ScoreNumberSprites[value]);
		lastPosition += m_ScoreNumberSprites[value]->getGlobalBounds().size.x + 2;
	}

	if(m_MovingDirection != 0.0f)
	{
		if(m_MovingStorageLerp < 1.f && m_MovingStorageLerp > 0.f)
		{
			m_MovingStorageLerp += a_Delta * 2.f * m_MovingDirection;

			m_MovingStorageSprite->setPosition(BubbleMath::Lerp(m_Duck->GetPosition(), m_StoredSprite->getPosition(), 
				m_MovingStorageLerp));
			m_Window->draw(*m_MovingStorageSprite);
		}

		else
		{
			FinishMoveToStorage();
		}
	}

	m_Window->draw(*m_CycleSprite);
	m_Window->draw(*m_InstructionShape);
	m_Window->draw(*m_HighScoresInPlay[0]);
	m_Window->draw(*m_HighScoresInPlay[1]);
	m_Window->draw(*m_HighScoresInPlay[2]);
	m_Window->draw(*m_NextUpBubble);
	m_DebugCircle->setPosition(sf::Vector2f(m_Frame->getGlobalBounds().position.x + Settings::get().GetContainerLeft(), m_Frame->getGlobalBounds().position.y + Settings::get().GetContainerBottom()));
	m_Window->draw(*m_DebugCircle);


	RenderBubbles();



}

void Rendering::MenuDraw() const
{
	m_Window->draw(*m_MainBackgroundExtendedSprite);
	m_Window->draw(*m_MainBackgroundSprite);
	m_Window->draw(*m_Title);
	sf::Vector2f mousePosition = m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window));
	m_MenuButtons.at("Play")->DetectHover(mousePosition);
	m_MenuButtons.at("High_Score")->DetectHover(mousePosition);
	m_MenuButtons.at("Settings")->DetectHover(mousePosition);
	m_MenuButtons.at("Exit")->DetectHover(mousePosition);


	m_MenuButtons.at("Play")->Draw(*m_Window);
	m_MenuButtons.at("High_Score")->Draw(*m_Window);
	m_MenuButtons.at("Settings")->Draw(*m_Window);
	m_MenuButtons.at("Exit")->Draw(*m_Window);
}

void Rendering::GameOverAnimationDraw() const
{
	m_Window->draw(*m_BackgroundSprite);

	m_Window->draw(*m_Container);
	m_Window->draw(*m_Frame);
	m_Duck->Draw(*m_Window);

	for (auto& element : m_RenderedBubbles)
	{
		element->Draw(*m_Window);
	}

	m_Window->draw(*m_GameOver);
}

void Rendering::HighScoreDraw() const
{
	m_Window->draw(*m_MainBackgroundSprite);

	m_Window->draw(*m_HighScoreTitle);
	m_HSBackButton->Draw(*m_Window);
	m_Window->draw(*m_HSBackgroundSprite);
	for (int i = 0; i < 10; i++)
	{
		m_Window->draw(*(m_HighScoreSprites[i]));
	}
}

void Rendering::GameOverDraw() const
{
	m_Window->draw(*m_BackgroundSprite);

	m_Window->draw(*m_GameOver);

	float lastPosition = m_GOStartPosition;
	std::string scoreString = m_Score->getString();
	for (size_t i = 0; i < scoreString.size(); i++)
	{
		unsigned char asChar = scoreString[i];
		int value = asChar - '0';
		m_GOScoreNumberSprites[value]->setPosition(sf::Vector2f(lastPosition, m_GOScoreNumberSprites[value]->getPosition().y));
		m_Window->draw(*m_GOScoreNumberSprites[value]);
		lastPosition += m_GOScoreNumberSprites[value]->getGlobalBounds().size.x + 2;
	}
	m_Window->draw(*m_GOScoreCloudSprite);
	m_MenuButtons.at("PlayAgain")->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	m_MenuButtons.at("BackToMenu")->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	m_MenuButtons.at("BackToMenu")->Draw(*m_Window);
	m_MenuButtons.at("PlayAgain")->Draw(*m_Window);
	m_Window->draw(*m_GOHighScoreBackground);
	for (int i = 0; i < 3; i++)
	{
		m_Window->draw(*m_GOHighScores[i]);
	}
	//m_Duck->Draw(*m_Window);
}

void Rendering::ConfirmationDraw() const
{
	sf::RectangleShape temp;
	sf::Vector2f windowSize = BubbleMath::ToVector2f(m_Window->getSize());
	temp.setSize(sf::Vector2f(windowSize.x, windowSize.y));
	temp.setFillColor(sf::Color(0, 0, 0, 128));
	m_Window->draw(temp);
	sf::Vector2f mousePosition = m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window));
	m_MenuButtons.at("ConfirmConfirm")->DetectHover(mousePosition);
	m_MenuButtons.at("CancelConfirm")->DetectHover(mousePosition);

	m_Window->draw(*m_ConfirmationWindow);
	m_Window->draw(*m_ConfirmationText);
	m_MenuButtons.at("ConfirmConfirm")->Draw(*m_Window);
	m_MenuButtons.at("CancelConfirm")->Draw(*m_Window);
}

void Rendering::SettingsDraw() const
{
	m_Window->draw(*m_MainBackgroundSprite);

	for (auto& current : m_SettingSliders)
	{
		current->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
		m_Window->draw(*current);
	}
	for(auto& current : m_SettingsText)
	{
		m_Window->draw(*current);

	}

	m_MenuButtons.at("ApplySettings")->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	m_MenuButtons.at("Revert")->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	m_RightResArrow->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	//m_FullscreenCheckbox->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	//m_Window->draw(*m_FullscreenCheckbox);
	m_MenuButtons.at("ApplySettings")->Draw(*m_Window);
	m_MenuButtons.at("Revert")->Draw(*m_Window);
	m_RightResArrow->Draw(*m_Window);
	m_Window->draw(*m_Resolution_List);
	m_Window->draw(*m_SettingsTitle);
}

void Rendering::SetupView() {
	const float containerWidth = Settings::get().GetContainerWidth();
	const float containerHeight = Settings::get().GetContainerHeight();

	m_WorldView.setSize(sf::Vector2f(containerWidth, containerHeight));
	m_WorldView.setCenter(sf::Vector2f(containerWidth / 2.f, containerHeight / 2.f));
}

void Rendering::RenderBubbles()
{
	const float scale = Settings::get().GetScale();
	const float windowWidth = Settings::get().GetWindowWidth();
	const float windowHeight = Settings::get().GetWindowHeight();
	const float containerWidth = Settings::get().GetContainerWidth();
	const float containerHeight = Settings::get().GetContainerHeight();

	float viewportW = (containerWidth * scale) / windowWidth;
	float viewportH = (containerHeight * scale) / windowHeight;

	float scaledOffsetX = m_Container->getGlobalBounds().position.x + Settings::get().GetContainerLeft();
	float scaledOffsetY = (Settings::get().GetWindowHeight() / 2.f - ((Settings::get().GetFrameHeight() * Settings::get().GetScale()) / 2.f)) + Settings::get().GetContainerBottom();
	//std::cout << Settings::get().GetFrameWidth() << std::endl;;

	float viewportX = scaledOffsetX / windowWidth;
	float viewportY = scaledOffsetY / windowHeight;


	m_WorldView.setViewport(sf::FloatRect(sf::Vector2f(viewportX, viewportY), sf::Vector2f(viewportW, viewportH)));

	m_Window->setView(m_WorldView);
	for (auto& element : m_RenderedBubbles)
	{
		element->Draw(*m_Window);
	}
	m_ParticleSystem->Draw(*m_Window);
	m_Window->draw(*m_ComboText);

	m_Window->setView(m_Window->getDefaultView());

}

void Rendering::Draw(const EGAME_STATE a_State,float a_Delta)
{

	m_Window->clear();

	if (a_State == EGAME_STATE::STATE_PLAY)
	{
		PlayDraw(a_Delta);
	}
	if (a_State == EGAME_STATE::STATE_MENU) MenuDraw();
	if(a_State == EGAME_STATE::STATE_LOADING)
	{
		m_Loading->SetAnimate(true, true);
		if(CallAfterDelay::getInstance().HasFunction("SetPlayState"))
		{
			m_Window->draw(*m_BackgroundExtendedSprite);
			m_Window->draw(*m_BackgroundSprite);
			m_BackgroundSprite->setFillColor(sf::Color::White);
			m_BackgroundExtendedSprite->setFillColor(sf::Color::White);
			float remainingTime = CallAfterDelay::getInstance().GetRemainingTimeAsPercentage("SetPlayState");
			m_MainBackgroundSprite->setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(remainingTime * 255.f)));
			m_MainBackgroundExtendedSprite->setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(remainingTime * 255.f)));
			m_Window->draw(*m_MainBackgroundExtendedSprite);
			m_Window->draw(*m_MainBackgroundSprite);

		}
		else
		{
			m_Window->draw(*m_MainBackgroundExtendedSprite);
			m_Window->draw(*m_MainBackgroundSprite);
			m_MainBackgroundSprite->setFillColor(sf::Color::White);
			m_MainBackgroundExtendedSprite->setFillColor(sf::Color::White);
			float remainingTime = CallAfterDelay::getInstance().GetRemainingTimeAsPercentage("SetMenuState");
			m_BackgroundSprite->setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(remainingTime * 255.f)));
			m_BackgroundExtendedSprite->setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(remainingTime * 255.f)));
			m_Window->draw(*m_BackgroundExtendedSprite);
			m_Window->draw(*m_BackgroundSprite);

		}

		m_Loading->Draw(*m_Window);
	}
	if(a_State == EGAME_STATE::STATE_GAME_OVER_ANIMATION)
	{
		GameOverAnimationDraw();
	}
	if(a_State == EGAME_STATE::STATE_GAME_OVER)
	{
		GameOverDraw();
	}
	if(a_State == EGAME_STATE::STATE_HIGH_SCORE)
	{
		HighScoreDraw();
	}
	if(a_State == EGAME_STATE::STATE_MENU_CONFIRM || a_State == EGAME_STATE::STATE_RESTART_CONFIRM)
	{
		PlayDraw(a_Delta);
		ConfirmationDraw();
	}
	if(a_State == EGAME_STATE::STATE_EXIT_CONFIRM)
	{
		MenuDraw();
		ConfirmationDraw();
	}
	if(a_State == EGAME_STATE::STATE_SETTINGS)
	{
		SettingsDraw();
	}
	if(a_State == EGAME_STATE::STATE_SETTINGS_CONFIRM)
	{
		SettingsDraw();
		ConfirmationDraw();
	}

	m_Window->display();
}

void Rendering::MovePointerLine(const float a_X) const
{

	float scale = Settings::get().GetScale();
	float scaledMovement = a_X * scale;
	float newScreenX = m_Frame->getGlobalBounds().position.x + Settings::get().GetContainerLeft() + scaledMovement;

	sf::Vector2f temp = m_Line->getPosition();
	temp.x = newScreenX;

	m_Line->setPosition(temp);

	if(temp.x == m_Duck->GetPosition().x)  // NOLINT(clang-diagnostic-float-equal)
	{
		m_Duck->SetAnimate(false, false);
	}
	else
	{
		m_Duck->SetAnimate(true, true);
	}
	if (temp.x > m_Duck->GetPosition().x)
	{
		m_Duck->GetSprite()->setScale(sf::Vector2f(- abs(m_Duck->GetSprite()->getScale().x), m_Duck->GetSprite()->getScale().y));
	}
	if(temp.x <m_Duck->GetPosition().x)
	{
		m_Duck->GetSprite()->setScale(sf::Vector2f(abs(m_Duck->GetSprite()->getScale().x), m_Duck->GetSprite()->getScale().y));
	}
	m_Duck->SetPosition(temp);
}

void Rendering::MovePreviewBubble(const EBUBBLE_TYPE a_NewPreview)
{
	m_ActiveBubble = a_NewPreview;
	m_PreviewBubbles.at(m_ActiveBubble)->SetPosition(m_Line->getPosition());
}

void Rendering::ResetButtons() const
{
	for(const auto& element : m_MenuButtons)
	{
		element.second->ApplyBaseTexture();
	}
}

void Rendering::UpdateResolutionList()
{
	std::string currentRes = m_Resolution_List->getString();
	std::string temp = currentRes.substr(0, 4);
	int index = Settings::get().FindResIndex(stoi(temp)) + 1;
	if (index == -1) { index = 0; }
	std::string newIndex = Settings::get().GetResSizeAsString(index);
	m_Resolution_List->setString(newIndex);
}

void Rendering::Reset()
{
	m_StoredSprite->setTexture(*m_StorageTextures.at(EBUBBLE_TYPE::TYPE_NULL));
	m_ActiveBubble = EBUBBLE_TYPE::TYPE_STAR;
}

void Rendering::ResetSize()
{
	sf::Vector2u newSize = m_Window->getSize();
	if (newSize.x != Settings::get().GetWindowWidth()) 
	{
		float scale = newSize.x / Settings::get().GetWindowWidth();
		newSize.y = Settings::get().GetWindowHeight() * Settings::get().GetScale();
		m_Window->setSize(newSize);
	}
	
	
	
	
	
	
	
	
	
	m_BackgroundSprite->setSize(sf::Vector2f(m_BackgroundTexture->getSize().x * Settings::get().GetScale(), m_BackgroundTexture->getSize().y * Settings::get().GetScale()));
	m_BackgroundExtendedSprite->setSize(sf::Vector2f(m_Window->getSize().x, m_Window->getSize().y));
	//m_MainBackgroundSprite;
	//std::unique_ptr<sf::Texture> m_MainBackgroundTexture;
	//std::unique_ptr<sf::Texture> m_MainBackgroundExtendedTexture;
	//std::unique_ptr<sf::RectangleShape> m_MainBackgroundExtendedSprite;


	////Play mode
	//std::vector<std::unique_ptr<AnimatedSprite>>& m_RenderedBubbles;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
	//std::unique_ptr<sf::Texture> m_ContainerTexture;
	//std::unique_ptr<sf::RectangleShape> m_Container;
	//std::unique_ptr<sf::Texture> m_FrameTexture;
	//std::unique_ptr<sf::RectangleShape> m_Frame;
	//std::unique_ptr<ParticleSystem> m_ParticleSystem;
	//std::map<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>> m_BubbleTextures;
	//std::unique_ptr<sf::Texture> m_NextUpTexture;
	//EBUBBLE_TYPE m_ActiveBubble;
	//EBUBBLE_TYPE m_ActiveNextUp;
	//std::map<EBUBBLE_TYPE, std::unique_ptr<AnimatedSprite>> m_PreviewBubbles;
	//std::unique_ptr<sf::Sprite> m_NextUpBubble;

	//std::unique_ptr<sf::Texture> m_LineTexture;
	//std::unique_ptr<sf::RectangleShape> m_Line;
	//std::unique_ptr<sf::Texture> m_DuckTexture;
	//std::unique_ptr<AnimatedSprite> m_Duck;

	////Play mode UI
	//std::unique_ptr<sf::Texture> m_ScoreBackgroundTexture;
	//std::unique_ptr<sf::Sprite> m_ScoreBackgroundSprite;
	//std::unique_ptr<sf::Texture> m_ScoreBackgroundInPlayTexture;
	//std::unique_ptr<sf::Sprite> m_ScoreBackgroundInPlaySprite;
	//std::unique_ptr<sf::Text> m_ComboText;
	//std::unique_ptr<sf::Text> m_Score;
	//std::unique_ptr<sf::Sprite> m_ScoreTitle;
	//std::unique_ptr<sf::Texture> m_ScoreTitleTexture;
	//std::vector<std::unique_ptr<sf::Texture>> m_ScoreNumberTextures;
	//std::vector<std::unique_ptr<sf::Sprite>> m_ScoreNumberSprites;
	//float m_ScoreStartPosition;

	//std::unique_ptr<sf::Sprite> m_HighScoreTitleInPlay;
	//std::unique_ptr<sf::Texture> m_HighScoreTitleTextureInPlay;
	//std::vector<std::unique_ptr<sf::Text>> m_HighScoresInPlay;
	//std::unique_ptr<sf::Texture> m_HighScoreTextureInPlay;
	//std::unique_ptr<sf::Sprite> m_HighScoreSpriteInPlay;

	//std::unique_ptr<sf::Text> m_StorageText;
	//std::unique_ptr<sf::Sprite> m_StoredSprite;
	//std::unique_ptr<sf::Sprite> m_MovingStorageSprite;
	//float m_MovingStorageLerp = 0;
	//float m_MovingDirection = 0;
	//EBUBBLE_TYPE m_TypeInStorage = EBUBBLE_TYPE::TYPE_NULL;
	//std::map<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>> m_StorageTextures;

	//std::unique_ptr<sf::Texture> m_InstructionTexture;
	//std::unique_ptr<sf::Sprite> m_InstructionShape;

	//std::unique_ptr<sf::Texture> m_CycleTexture;
	//std::unique_ptr<sf::RectangleShape> m_CycleSprite;

	////Game over mode
	//std::unique_ptr<sf::RectangleShape> m_GameOver;
	//std::unique_ptr<sf::Texture> m_GameOverTexture;
	//std::unique_ptr<sf::Text> m_GameOverScoreText;
	//std::vector<std::unique_ptr<sf::Sprite>> m_GOScoreNumberSprites;
	//std::unique_ptr<sf::Sprite> m_GOScoreCloudSprite;
	//float m_GOStartPosition;
	//std::unique_ptr<sf::Sprite> m_GOHighScoreBackground;
	//std::vector<std::unique_ptr<sf::Text>> m_GOHighScores;

	////Menu mode
	//std::unique_ptr<sf::RectangleShape> m_Title;
	//std::unique_ptr<sf::Texture> m_TitleTexture;
	//std::map<std::string, std::unique_ptr<Button>> m_MenuButtons;
	//std::unique_ptr<sf::Font> m_Font;
	//std::unique_ptr<sf::Font> m_FontBold;
	//std::unique_ptr<AnimatedSprite> m_Loading;
	//std::unique_ptr<sf::Texture> m_LoadingTexture;
	//std::unique_ptr<sf::Texture> m_BaseButtonTexture;

	////HighScore
	//std::vector<std::unique_ptr<sf::Text>> m_HighScoreSprites;
	//std::unique_ptr<sf::RectangleShape> m_HighScoreTitle;
	//std::unique_ptr<sf::Texture> m_HighScoreTexture;
	//std::unique_ptr<Button> m_HSBackButton;
	//std::unique_ptr<sf::Texture> m_HSBackgroundTexture;
	//std::unique_ptr<sf::Sprite> m_HSBackgroundSprite;
	////Confirm
	//std::unique_ptr<sf::RectangleShape> m_ConfirmationWindow;
	//std::unique_ptr<sf::Texture> m_ConfirmationTexture;
	//std::unique_ptr<sf::Text> m_ConfirmationText;

	////Settings
	//std::unique_ptr<sf::RectangleShape> m_SettingsTitle;
	//std::unique_ptr<sf::Texture> m_SettingsTexture;
	//std::vector<std::unique_ptr<sf::Text>> m_SettingsText;
	//std::vector<std::unique_ptr<Slider>> m_SettingSliders;
	//std::unique_ptr<sf::Texture> m_CheckboxTexture;
	//std::unique_ptr<Checkbox> m_FullscreenCheckbox;

	//std::unique_ptr<sf::Image> m_Icon;
}

void Rendering::UpdateHighScores(const std::vector<unsigned int>& a_Scores) const
{
	for(int i = 0; i < 10; i++)
	{
		m_HighScoreSprites[i]->setString(std::to_string(a_Scores[i]));
	}
	m_Score->setString(m_HighScoreSprites[0]->getString());

}

void Rendering::UpdateConfirmText(EGAME_STATE a_NewState) const
{
	if(a_NewState == EGAME_STATE::STATE_EXIT_CONFIRM)
	{
		m_ConfirmationText->setString(CONFIRM_EXIT_GAME);
	}
	if(a_NewState == EGAME_STATE::STATE_MENU_CONFIRM)
	{
		m_ConfirmationText->setString(CONFIRM_BACK_TO_MENU);
	}
	if(a_NewState == EGAME_STATE::STATE_RESTART_CONFIRM)
	{
		m_ConfirmationText->setString(CONFIRM_RESTART);
	}
	if(a_NewState == EGAME_STATE::STATE_SETTINGS_CONFIRM)
	{
		m_ConfirmationText->setString(CONFIRM_SETTING_RESTART);
	}
	sf::FloatRect localBounds = m_ConfirmationText->getLocalBounds();
	sf::Vector2f position = localBounds.position;
	m_ConfirmationText->setOrigin(sf::Vector2f(position.x + localBounds.size.x / 2, position.y + localBounds.size.y / 2));
	m_ConfirmationText->setPosition(m_ConfirmationWindow->getPosition());

}

void Rendering::UpdateComboPosition(const sf::Vector2f& a_NewPos) const
{
	sf::FloatRect textBounds = m_ComboText->getLocalBounds();
	sf::FloatRect rectBounds = m_Container->getGlobalBounds();

	// Adjust coordinate to fit within rectangle bounds
	sf::Vector2f adjustedCoord = a_NewPos;
	adjustedCoord.x = std::max(rectBounds.position.x, std::min(rectBounds.position.x + rectBounds.size.x - textBounds.size.x, adjustedCoord.x));
	adjustedCoord.y = std::max(rectBounds.position.y, std::min(rectBounds.position.y + rectBounds.size.y - textBounds.size.y, adjustedCoord.y));

	// Position the text
	m_ComboText->setPosition(a_NewPos);
	sf::Color comboColor = sf::Color::Red;
	m_ParticleSystem->SpawnBurst(a_NewPos, 10, comboColor);


}

void Rendering::UpdateHighScore(const std::vector<unsigned int>& a_Scores) const
{
	for (size_t i = 0; i < a_Scores.size(); i++)
	{
		m_HighScoreSprites[i]->setString(std::to_string(a_Scores[i]));
	}

	m_HighScoresInPlay[0]->setString(std::to_string(a_Scores[0]));
	m_HighScoresInPlay[1]->setString(std::to_string(a_Scores[1]));
	m_HighScoresInPlay[2]->setString(std::to_string(a_Scores[2]));

	m_GOHighScores[0]->setString(std::to_string(a_Scores[0]));
	m_GOHighScores[1]->setString(std::to_string(a_Scores[1]));
	m_GOHighScores[2]->setString(std::to_string(a_Scores[2]));

}

void Rendering::ResetAllBackground()
{
	m_MainBackgroundSprite->setFillColor(sf::Color(255, 255, 255, 255));
	m_BackgroundSprite->setFillColor(sf::Color::White);
}

void Rendering::StartMoveToStorage(EBUBBLE_TYPE a_Type, bool a_ToStorage)
{
	if (a_Type == EBUBBLE_TYPE::TYPE_NULL)
	{
		return;
	}
	if (a_ToStorage) m_MovingStorageSprite->setTexture(*m_BubbleTextures.at(a_Type));
	else m_MovingStorageSprite->setTexture(*m_BubbleTextures.at(m_TypeInStorage));

	sf::Vector2i u = sf::Vector2i(m_BubbleTextures.at(a_Type)->getSize());
	m_MovingStorageSprite->setTextureRect(sf::IntRect(sf::Vector2i(0, 0),sf::Vector2i(u.x / 8, u.y)));
	float storage_box_width = Settings::get().GetStorageBoxWidth();
	sf::Vector2f size = BubbleMath::ToVector2f(m_MovingStorageSprite->getTexture().getSize());
	float factor_x = storage_box_width / (size.x / 8.f);
	float factor_y = storage_box_width / size.y;
	m_MovingStorageSprite->setScale(sf::Vector2f(factor_x, factor_y));

	m_TypeInStorage = a_ToStorage ? a_Type : EBUBBLE_TYPE::TYPE_NULL;
	m_MovingStorageLerp = a_ToStorage ? 0.000001f : 0.999999f;
	m_MovingDirection = a_ToStorage ? 1.f : -1.f;
	m_StoredSprite->setTexture(*m_StorageTextures.at(EBUBBLE_TYPE::TYPE_NULL));
}

void Rendering::LoadBubbleTextures()
{
	m_BubbleTextures = std::map<EBUBBLE_TYPE,std::unique_ptr<sf::Texture>>();

	for(int i = 0; i < 10; i++)
	{
		m_BubbleTextures.insert(std::pair(static_cast<EBUBBLE_TYPE>(i), std::make_unique<sf::Texture>()));
		m_BubbleTextures[static_cast<EBUBBLE_TYPE>(i)]->loadFromFile(BUBBLE_PATH + std::to_string(i) + ".png");
	}

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_BATH_BOMB, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_BATH_BOMB]->loadFromFile(BATH_BUBBLE_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_SPIKY_BOMB, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_SPIKY_BOMB]->loadFromFile(SPIKY_BUBBLE_FILENAME);
}

void Rendering::LoadNextUpTextures()
{
	m_NextUpTexture = std::make_unique<sf::Texture>();
	m_NextUpTexture->loadFromFile(NEXT_UP_FILENAME);
}

void Rendering::LoadStorageTextures()
{
	m_StorageTextures = std::map<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>();

	for (int i = 0; i < 4; i++)
	{
		m_StorageTextures.insert(std::pair(static_cast<EBUBBLE_TYPE>(i), std::make_unique<sf::Texture>()));
		m_StorageTextures[static_cast<EBUBBLE_TYPE>(i)]->loadFromFile(STORAGE_PATH + std::to_string(i) + ".png");
	}

	m_StorageTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_BATH_BOMB, std::make_unique<sf::Texture>()));
	m_StorageTextures[EBUBBLE_TYPE::TYPE_BATH_BOMB]->loadFromFile(STORAGE_SPECIAL_PATH);
	m_StorageTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_NULL, std::make_unique<sf::Texture>()));
	std::string temp = "/empty.png";
	m_StorageTextures[EBUBBLE_TYPE::TYPE_NULL]->loadFromFile(STORAGE_PATH + temp);

	m_CycleTexture = std::make_unique<sf::Texture>();
	m_CycleTexture->loadFromFile(SOAP_BOTTLE_FILENAME);
}

void Rendering::LoadBackground()
{
	//Game Background
	m_BackgroundTexture = std::make_unique<sf::Texture>();
	m_BackgroundTexture->loadFromFile(GAME_BACKGROUND_FILENAME);
	m_BackgroundTexture->setRepeated(false);
	m_BackgroundSprite = std::make_unique<sf::RectangleShape>();
	m_BackgroundSprite->setTexture(m_BackgroundTexture.get());
	sf::Vector2f windowSize = sf::Vector2f(static_cast<float>(m_Window->getSize().x),static_cast<float>(m_Window->getSize().y));

	float verticalScale = windowSize.y / static_cast<float>(m_BackgroundTexture->getSize().y);
	float horizontalScale = verticalScale;

	m_BackgroundSprite->setSize(BubbleMath::ToVector2f(m_BackgroundTexture->getSize()));
	m_BackgroundSprite->setScale(sf::Vector2f(horizontalScale, verticalScale));
	m_BackgroundSprite->setOrigin(sf::Vector2f(m_BackgroundSprite->getLocalBounds().size.x / 2, m_BackgroundSprite->getLocalBounds().size.y / 2));
	m_BackgroundSprite->setPosition(sf::Vector2f(windowSize.x / 2, windowSize.y / 2));

	m_BackgroundExtendedTexture = std::make_unique<sf::Texture>();
	m_BackgroundExtendedTexture->loadFromFile(MENU_BACKGROUND_EXTENDED_FILENAME);
	m_BackgroundExtendedTexture->setRepeated(true);
	m_BackgroundExtendedSprite = std::make_unique<sf::RectangleShape>();
	m_BackgroundExtendedSprite->setTexture(m_MainBackgroundExtendedTexture.get());
	m_BackgroundExtendedSprite->setSize(sf::Vector2f(static_cast<float>(m_Window->getSize().x), static_cast<float>(m_Window->getSize().y)));

	//Main Background
	m_MainBackgroundTexture = std::make_unique<sf::Texture>();
	m_MainBackgroundTexture->loadFromFile(MENU_BACKGROUND_FILENAME);
	m_MainBackgroundTexture->setRepeated(false);
	m_MainBackgroundSprite = std::make_unique<sf::RectangleShape>();
	m_MainBackgroundSprite->setTexture(m_MainBackgroundTexture.get());
	// Calculate scaling factors and set sprite size and origin
	sf::Vector2f backgroundTextureSize = BubbleMath::ToVector2f(m_MainBackgroundTexture->getSize());

	verticalScale = windowSize.y / backgroundTextureSize.y;
	horizontalScale = verticalScale;
	m_MainBackgroundSprite->setSize(backgroundTextureSize);
	m_MainBackgroundSprite->setScale(sf::Vector2f(horizontalScale, verticalScale));
	m_MainBackgroundSprite->setOrigin(sf::Vector2f(m_MainBackgroundSprite->getSize().x / 2.f, m_MainBackgroundSprite->getSize().y / 2.f));
	m_MainBackgroundSprite->setPosition(sf::Vector2f(windowSize.x / 2, windowSize.y / 2));


	m_MainBackgroundExtendedTexture = std::make_unique<sf::Texture>();
	m_MainBackgroundExtendedTexture->loadFromFile(MENU_BACKGROUND_EXTENDED_FILENAME);
	m_MainBackgroundExtendedTexture->setRepeated(true);
	m_MainBackgroundExtendedSprite = std::make_unique<sf::RectangleShape>();
	m_MainBackgroundExtendedSprite->setTexture(m_MainBackgroundExtendedTexture.get());
	m_MainBackgroundExtendedSprite->setSize(sf::Vector2f(static_cast<float>(m_Window->getSize().x), static_cast<float>(m_Window->getSize().y)));

	//Container
	m_ContainerTexture = std::make_unique<sf::Texture>();
	if(!m_ContainerTexture->loadFromFile(CONTAINER_FILENAME))
	{
		//throw std::exception("Failed to load container texture");
	}

	m_ContainerTexture->setRepeated(false);
	m_Container = std::make_unique<sf::RectangleShape>();
	m_Container->setTexture(m_ContainerTexture.get());
	m_FrameTexture = std::make_unique<sf::Texture>();
	m_FrameTexture->loadFromFile(FRAME_FILENAME);
	m_Frame = std::make_unique<sf::RectangleShape>();
	m_Frame->setTexture(m_FrameTexture.get());


	float width = Settings::get().GetFrameWidth() * Settings::get().GetScale();
	float height = Settings::get().GetFrameHeight() * Settings::get().GetScale();
	m_Container->setSize(sf::Vector2f(width, height));
	m_Frame->setSize(sf::Vector2f(width, height));
	m_Container->setOrigin(sf::Vector2f(width / 2.f, height / 2.f));
	m_Frame->setOrigin(sf::Vector2f(width / 2.f, height / 2.f));

	Settings::get().UpdateContainerPos(
		(height / m_ContainerTexture->getSize().y) * Settings::get().GetContainerBottom(),
		(width / m_ContainerTexture->getSize().x) * Settings::get().GetContainerLeft()
	);


	sf::Vector2f basePos = sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f);
	m_Container->setPosition(basePos);
	m_Frame->setPosition(basePos);
	//m_Frame->setPosition(sf::Vector2f(m_Container->getPosition().x + m_Container->getSize().x / 2, m_Container->getPosition().y + m_Container->getSize().y / 2 - Settings::get().GetFrameCorrection()));
}

void Rendering::CreateSprite(const EBUBBLE_TYPE a_Type, const sf::Vector2f& a_Position, const float a_Rotation, std::unique_ptr<AnimatedSprite>& a_NewSprite) const
{
	a_NewSprite = std::make_unique<AnimatedSprite>(m_BubbleTextures.at(a_Type).get(), Settings::get().GetBubbleAnimationTotalTime(), Settings::get().GetBubbleFrames());

	sf::Vector2f size = BubbleMath::ToVector2f(m_BubbleTextures.at(a_Type)->getSize());
	size.x /= static_cast<float>(Settings::get().GetBubbleFrames());
	float pixelToMeter = Settings::get().GetPixelToMeter();
	float factorX = (Settings::get().BubbleSize(a_Type) * pixelToMeter * 2) / size.x;
	float factorY = (Settings::get().BubbleSize(a_Type) * pixelToMeter * 2) / size.y;
	a_NewSprite->GetSprite()->setScale(sf::Vector2f(factorX, factorY));

	float x = Settings::get().BubbleSize(a_Type) * pixelToMeter / a_NewSprite->GetSprite()->getScale().x;
	float y = Settings::get().BubbleSize(a_Type) * pixelToMeter / a_NewSprite->GetSprite()->getScale().y;
	a_NewSprite->GetSprite()->setOrigin(sf::Vector2f(x, y));

	a_NewSprite->SetPosition(a_Position);
	a_NewSprite->SetRotation(a_Rotation);
}

void Rendering::CreatePointer()
{
	m_LineTexture = std::make_unique<sf::Texture>();
	m_LineTexture->loadFromFile(LINE_FILENAME);
	float containerHeight = Settings::get().GetContainerHeight() * Settings::get().GetScale();
	sf::Vector2f position =
		sf::Vector2f(m_Container->getGlobalBounds().position.x, (m_Container->getGlobalBounds().position.y) + (Settings::get().GetContainerBottom()));
	m_Line = std::make_unique<sf::RectangleShape>(sf::Vector2f((5 / 2.f) * Settings::get().GetScale(), containerHeight));
	m_Line->setTexture(m_LineTexture.get());
	m_Line->setPosition(position);

	sf::Vector2f shapeSize = m_Line->getSize();
	sf::Vector2f textureSize = BubbleMath::ToVector2f(m_LineTexture->getSize());
	float scaleX = shapeSize.x / textureSize.x;
	sf::IntRect intRect = sf::IntRect(
		sf::Vector2i(0, 0),
		sf::Vector2i(static_cast<int>(shapeSize.x / scaleX),
		static_cast<int>(shapeSize.y / scaleX))
	);
	m_Line->setTextureRect(intRect);
	m_LineTexture->setRepeated(true);
	m_Line->setTexture(m_LineTexture.get());
	for(size_t i = 0; i < 12; i++)
	{
		std::unique_ptr<AnimatedSprite> newSprite;
		CreateSprite(static_cast<EBUBBLE_TYPE>(i), sf::Vector2f(position), 0, newSprite);
		m_PreviewBubbles.insert(std::pair(static_cast<EBUBBLE_TYPE>(i), std::move(newSprite)));
		sf::Vector2f temp = m_PreviewBubbles.at(static_cast<EBUBBLE_TYPE>(i))->GetSprite()->getScale();
		sf::Vector2f size = BubbleMath::ToVector2f(m_BubbleTextures.at(static_cast<EBUBBLE_TYPE>(i))->getSize());
		size.x /= static_cast<float>(Settings::get().GetBubbleFrames());

		float factorX = (Settings::get().BubbleSize(static_cast<EBUBBLE_TYPE>(i)) * Settings::get().GetPixelToMeter() * 2) / size.x * Settings::get().GetScale();
		float factorY = (Settings::get().BubbleSize(static_cast<EBUBBLE_TYPE>(i)) * Settings::get().GetPixelToMeter() * 2) / size.y * Settings::get().GetScale();

		m_PreviewBubbles.at(static_cast<EBUBBLE_TYPE>(i))->GetSprite()->setScale(sf::Vector2f(factorX, factorY));
	}
}

void Rendering::CreateTitleSprite()
{
	m_TitleTexture = std::make_unique<sf::Texture>();
	m_TitleTexture->loadFromFile(TITLE_FILENAME);
	sf::Vector2f titleTextureSize = sf::Vector2f(Settings::get().GetTitleWidth(), Settings::get().GetTitleHeight());

	m_Title = std::make_unique<sf::RectangleShape>();
	m_Title = std::make_unique<sf::RectangleShape>();
	m_Title->setTexture(m_TitleTexture.get());
	m_Title->setSize(titleTextureSize);
	m_Title->setOrigin(sf::Vector2f(titleTextureSize.x / 2.f, titleTextureSize.y / 2.f));
	sf::Vector2f basePos = sf::Vector2f(static_cast<float>(m_Window->getSize().x) / 2.f, static_cast<float>(m_Window->getSize().y) / 4);
	m_Title->setPosition(basePos);
}

void Rendering::CreateGameOverSprite()
{
	m_GameOverTexture = std::make_unique<sf::Texture>();
	m_GameOverTexture->loadFromFile(GAME_OVER_FILENAME);
	m_GameOver = std::make_unique<sf::RectangleShape>();
	m_GameOver->setTexture(m_GameOverTexture.get());
	sf::Vector2f titleSize = m_Title->getSize();
	sf::Vector2f gameOverTextureSize = BubbleMath::ToVector2f(m_GameOverTexture->getSize());
	float scale = titleSize.y / gameOverTextureSize.y;
	titleSize.x = gameOverTextureSize.x * scale;
	m_GameOver->setSize(titleSize);
	m_GameOver->setOrigin(sf::Vector2f(titleSize.x / 2, titleSize.y / 2));
	m_GameOver->setPosition(sf::Vector2f(m_Window->getSize().x / 2.f,m_Window->getSize().y * 0.33f));

	sf::Vector2f BBTextureSize = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetMenuButtonWidth() / (BBTextureSize.x / 3.7f), Settings::get().GetMenuButtonHeight() / BBTextureSize.y);

	
	m_GOScoreNumberSprites = std::vector<std::unique_ptr<sf::Sprite>>(10);
	for (int i = 0; i < 10; i++)
	{
		m_GOScoreNumberSprites[i] = std::make_unique<sf::Sprite>(*m_ScoreNumberTextures[i]);
		m_GOScoreNumberSprites[i]->setScale(sf::Vector2f(0.3f * Settings::get().GetScale(), 0.3f * Settings::get().GetScale()));
		m_GOScoreNumberSprites[i]->setPosition(sf::Vector2f(m_GameOver->getGlobalBounds().position.x, m_GameOver->getPosition().y + m_GameOver->getGlobalBounds().size.y));
	}
	m_GOScoreCloudSprite = std::make_unique<sf::Sprite>(*m_ScoreBackgroundInPlayTexture);
	m_GOScoreCloudSprite->setOrigin(sf::Vector2f(m_GOScoreCloudSprite->getLocalBounds().size.x / 2, m_GOScoreCloudSprite->getLocalBounds().size.y / 2));
	m_GOScoreCloudSprite->setScale(sf::Vector2f(0.3f * Settings::get().GetScale(), 0.3f * Settings::get().GetScale()));
	float y = m_GOScoreNumberSprites[0]->getGlobalBounds().position.y + m_GOScoreNumberSprites[0]->getGlobalBounds().size.y;
	m_GOScoreCloudSprite->setPosition(sf::Vector2f(m_GameOver->getPosition().x  - m_GameOver->getGlobalBounds().size.x /4, y));

	
	
	m_GOHighScoreBackground = std::make_unique<sf::Sprite>(*m_HighScoreTextureInPlay); // HIII DOORTJE THIS TEXTURE WAS MISSING, SO I JUST ADDED THE GAME OvERE TEXTURE IN but idk what you wanted to show here so change it.


	m_GOHighScoreBackground = std::make_unique<sf::Sprite>(*m_HighScoreTextureInPlay);
	
	m_GOHighScoreBackground->setOrigin(sf::Vector2f(m_HighScoreSpriteInPlay->getLocalBounds().size.x / 2.f, m_HighScoreSpriteInPlay->getLocalBounds().size.y / 2.f));
	m_GOHighScoreBackground->setScale(sf::Vector2f(0.4f * Settings::get().GetScale(), 0.4f * Settings::get().GetScale()));
	sf::Vector2f position = sf::Vector2f(m_GameOver->getPosition().x + m_GameOver->getGlobalBounds().size.x / 2.f - m_GOHighScoreBackground->getGlobalBounds().size.x, 0);
	position.y = m_GameOver->getPosition().y + m_GameOver->getGlobalBounds().size.y / 2.f + m_GOHighScoreBackground->getGlobalBounds().size.y / 2.f + 5 * Settings::get().GetScale();
	m_GOHighScoreBackground->setPosition(position);
	int size = static_cast<int>(38.f * Settings::get().GetScale());
	sf::Color textColor = sf::Color(225, 142, 149, 255);

	position.y = m_GOHighScoreBackground->getGlobalBounds().getCenter().y - m_GOHighScoreBackground->getGlobalBounds().size.y / 4.1f;
	position.x = m_GOHighScoreBackground->getGlobalBounds().getCenter().x - m_GOHighScoreBackground->getGlobalBounds().size.x / 10;


	m_GOHighScores = std::vector<std::unique_ptr<sf::Text>>(3);
	m_GOHighScores[0] = std::make_unique<sf::Text>(*m_FontBold);

	m_GOHighScores[0]->setString(std::to_string(rand()));
	//position.x -= 78 * Settings::get().GetScale();
	position.y = m_GOHighScoreBackground->getGlobalBounds().getCenter().y - m_GOHighScoreBackground->getGlobalBounds().size.y / 4.1f;
	position.x = m_GOHighScoreBackground->getGlobalBounds().getCenter().x - m_GOHighScoreBackground->getGlobalBounds().size.x / 10;
	m_GOHighScores[0]->setPosition(position);
	m_GOHighScores[0]->setFont(*m_FontBold);
	m_GOHighScores[0]->setFillColor(textColor);
	m_GOHighScores[0]->setCharacterSize(size);
	position.y += m_GOHighScoreBackground->getGlobalBounds().size.y / 4;

	m_GOHighScores[1] = std::make_unique<sf::Text>(*m_FontBold);
	m_GOHighScores[1]->setString(std::to_string(rand()));
	//m_GOHighScores.y += m_HighScoreSpriteInPlay->getGlobalBounds().size.y / 4;
	m_GOHighScores[1]->setPosition(position);
	m_GOHighScores[1]->setFillColor(textColor);
	m_GOHighScores[1]->setCharacterSize(size);


	m_GOHighScores[2] = std::make_unique<sf::Text>(*m_FontBold);
	m_GOHighScores[2]->setString(std::to_string(rand()));
	position.y += m_GOHighScoreBackground->getGlobalBounds().size.y / 4;
	m_GOHighScores[2]->setPosition(position);
	m_GOHighScores[2]->setFillColor(textColor);
	m_GOHighScores[2]->setCharacterSize(size);


	sf::Vector2f basePos = m_GOHighScoreBackground->getPosition();
	basePos.x = m_GameOver->getGlobalBounds().position.x + Settings::get().GetMenuButtonWidth();
	basePos.y = m_GOHighScoreBackground->getGlobalBounds().position.y + m_GOHighScoreBackground->getGlobalBounds().size.y * 1.3;
	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Play again");
	newButton->ResizeCharacters(std::round(38.f * Settings::get().GetScale()));
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("PlayAgain", std::move(newButton)));

	basePos.x += m_MenuButtons.at("PlayAgain")->GetWidth() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Back to menu");
	newButton->ResizeCharacters(std::round(38.f * Settings::get().GetScale()));
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("BackToMenu", std::move(newButton)));


}

void Rendering::CreateMenuSprites()
{
	CreateTitleSprite();
	CreateMenuButtonSprites();
}

void Rendering::CreateMenuButtonSprites()
{
	m_Font = std::make_unique<sf::Font>();
	m_Font->openFromFile(FONT_FILENAME);

	m_FontBold = std::make_unique<sf::Font>();
	m_FontBold->openFromFile(FONT_BOLD_FILENAME);


	m_PlayButtonTexture = std::make_unique<sf::Texture>(HIGH_SCORES_BUTTON);
	m_HighScoresButtonTexture = std::make_unique<sf::Texture>(HIGH_SCORES_BUTTON);
	m_SettingsButtonTexture = std::make_unique<sf::Texture>(HIGH_SCORES_BUTTON);
	m_ExitButtonTexture = std::make_unique<sf::Texture>(HIGH_SCORES_BUTTON);


	sf::Vector2f basePos = sf::Vector2f(m_Title->getPosition());
	basePos.y += m_Title->getSize().y + 10;
	basePos.x -= m_Title->getSize().x / 3;
	m_BaseButtonTexture = std::make_unique<sf::Texture>();
	m_BaseButtonTexture->loadFromFile(BUTTON_FILENAME);
	m_MenuButtons = std::map<std::string, std::unique_ptr<Button>>();
	sf::Vector2f BBTextureSize = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetMenuButtonWidth() / (BBTextureSize.x / 3.f), Settings::get().GetMenuButtonHeight() / BBTextureSize.y);

	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos,*m_Font, m_PlayButtonTexture.get());
	newButton->SetText("Play");
	newButton->ResizeCharacters(Settings::get().GetMainButtonFontSize());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(),std::pair<std::string, std::unique_ptr<Button>>("Play", std::move(newButton)));

	basePos.y += Settings::get().GetMenuButtonHeight() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_HighScoresButtonTexture.get());
	newButton->SetText("High Scores");
	newButton->ResizeCharacters(Settings::get().GetMainButtonFontSize());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("High_Score", std::move(newButton)));

	basePos.x += m_Title->getSize().x / 1.5f;
	basePos.y = m_Title->getPosition().y + m_Title->getSize().y + 10;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_SettingsButtonTexture.get());
	newButton->SetText("Settings");
	newButton->ResizeCharacters(Settings::get().GetMainButtonFontSize());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Settings", std::move(newButton)));

	basePos.y += Settings::get().GetMenuButtonHeight() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_ExitButtonTexture.get());
	newButton->SetText("Exit");
	newButton->ResizeCharacters(Settings::get().GetMainButtonFontSize());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Exit", std::move(newButton)));

	m_LoadingTexture = std::make_unique<sf::Texture>();
	m_LoadingTexture->loadFromFile(LOADING_FILENAME);
	m_Loading = std::make_unique<AnimatedSprite>(m_LoadingTexture.get(), Settings::get().GetLoadTime(), Settings::get().GetLoadingFrames(), true);
	sf::Vector2f windowSize = BubbleMath::ToVector2f(m_Window->getSize());
	m_Loading->SetScale(0.5, 0.5);
	m_Loading->SetPosition(sf::Vector2f(windowSize.x - m_Loading->GetSize().x, windowSize.y - m_Loading->GetSize().y));
}

void Rendering::CreateScoreText()
{
	sf::Vector2f position = m_Frame->getPosition();
	position.x -= m_Frame->getSize().x / 1.5f;
	m_ScoreBackgroundTexture = std::make_unique<sf::Texture>();
	m_ScoreBackgroundTexture->loadFromFile(GENERIC_BACKGROUND_FILENAME);

	m_Score = std::make_unique<sf::Text>(*m_Font);
	m_Score->setFillColor(sf::Color(201,180,211));
	m_Score->setOutlineColor(sf::Color::White);
	m_Score->setOutlineThickness(4);
	m_Score->setCharacterSize(70 * Settings::get().GetScale());
	m_Score->setString("0");
	m_Score->setPosition(position);

	m_ComboText = std::make_unique<sf::Text>(*m_Font);
	m_ComboText->setCharacterSize(40 * Settings::get().GetScale());
	m_ComboText->setFillColor(sf::Color(201, 180, 211));
	m_ComboText->setOutlineColor(sf::Color::White);
	m_ComboText->setOutlineThickness(4);
	m_ComboText->setStyle(sf::Text::Bold);
	m_ComboText->setString("Combo:\n 0");
	m_ComboText->setOrigin(sf::Vector2f(m_ComboText->getGlobalBounds().position.y + m_ComboText->getGlobalBounds().size.y, 0));
	position = m_Container->getPosition();
	position.x += Settings::get().GetContainerWidth() * Settings::get().GetScale();
	position.y += 100;
	m_ComboText->setPosition(position);
	m_ParticleSystem = std::make_unique<ParticleSystem>();
}

void Rendering::CreateHighScoreSprites()
{
	m_HighScoreSprites = std::vector < std::unique_ptr<sf::Text>>(10);
	sf::Vector2f basePos = sf::Vector2f(m_Title->getPosition());

	m_HighScoreTexture = std::make_unique<sf::Texture>();
	m_HighScoreTexture->loadFromFile(HIGH_SCORE_TITLE_FILENAME);

	m_HighScoreTitle = std::make_unique<sf::RectangleShape>();
	m_HighScoreTitle->setTexture(m_HighScoreTexture.get());
	sf::Vector2f titleSize = m_Title->getSize();
	auto HSTextureSize = BubbleMath::ToVector2f(m_HighScoreTexture->getSize());
	float scale = titleSize.y / HSTextureSize.y;
	titleSize.x = HSTextureSize.x * scale;
	m_HighScoreTitle->setSize(titleSize);
	m_HighScoreTitle->setOrigin(sf::Vector2f(m_HighScoreTitle->getSize().x / 2.f, m_HighScoreTitle->getSize().y / 2.f));
	m_HighScoreTitle->setPosition(m_Title->getPosition());


	m_HSBackgroundTexture = std::make_unique<sf::Texture>(ALL_HIGHSCORES);
	m_HSBackgroundSprite = std::make_unique<sf::Sprite>(*m_HSBackgroundTexture);
	m_HSBackgroundSprite->setScale(sf::Vector2f(0.3f * Settings::get().GetScale(), 0.3f * Settings::get().GetScale()));
	m_HSBackgroundSprite->setOrigin(sf::Vector2f(m_HSBackgroundSprite->getLocalBounds().size.x / 2, m_HSBackgroundSprite->getLocalBounds().size.y / 2));
	float titleBottom = m_Title->getGlobalBounds().position.y + m_Title->getGlobalBounds().size.y;
	m_HSBackgroundSprite->setPosition(sf::Vector2f(m_Window->getSize().x / 2, titleBottom + 20 * Settings::get().GetScale() + m_HSBackgroundSprite->getGlobalBounds().size.y / 2 ));

	basePos.y += m_HSBackgroundSprite->getGlobalBounds().position.y - m_HSBackgroundSprite->getGlobalBounds().size.y / 2 + 5 *Settings::get().GetScale();

	for(int i = 0; i < 10; i ++)
	{
		m_HighScoreSprites[i] = std::make_unique<sf::Text>(*m_Font, std::to_string(rand()));
		m_HighScoreSprites[i]->setFillColor(sf::Color::Black);
		m_HighScoreSprites[i]->setCharacterSize(40 * Settings::get().GetScale());
		m_HighScoreSprites[i]->setPosition(basePos);
		m_HighScoreSprites[i]->setFillColor(sf::Color(201, 180, 211));
		m_HighScoreSprites[i]->setOutlineColor(sf::Color::White);
		m_HighScoreSprites[i]->setOutlineThickness(2);

		basePos.y += m_HighScoreSprites[i]->getCharacterSize() * 1.3f;
	}

	sf::Vector2f BBTexture = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	float x = BBTexture.x;
	sf::Vector2f(x / 2, BBTexture.y);
	basePos = BubbleMath::ToVector2f(m_Window->getSize());
	basePos.x = static_cast<float>(m_Window->getSize().x) / 10;
	basePos.y -= static_cast<float>(m_Window->getSize().y) / 10;
	m_HSBackButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	m_HSBackButton->SetText("Back");
	m_HSBackButton->ResizeCharacters(std::round(30.f * Settings::get().GetScale()));
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetButtonWidth() / (BBTexture.x / 3.5f), Settings::get().GetButtonHeight() / BBTexture.y);

	m_HSBackButton->SetScale(buttonScale);
}

void Rendering::CreateDuck()
{
	m_DuckTexture = std::make_unique<sf::Texture>();
	m_DuckTexture->loadFromFile(DUCK_FILENAME);

	m_Duck = std::make_unique<AnimatedSprite>(m_DuckTexture.get(), 1, 4, true, true);
	m_Duck->SetFrame(0);
	m_Duck->UpdateFrameToAnimate(2);
	//float y = Settings::get().GetDuckHeight();
	float y = m_Duck->GetSprite()->getGlobalBounds().size.y * 0.75f;
	float x = m_Duck->GetSprite()->getGlobalBounds().size.x * 0.75f;
	m_Duck->GetSprite()->setOrigin(sf::Vector2f(x, y));
	float factorX = Settings::get().GetDuckWidth() / (static_cast<float>(m_DuckTexture->getSize().x) / 4);
	float factorY = Settings::get().GetDuckHeight() / static_cast<float>(m_DuckTexture->getSize().y);
	m_Duck->GetSprite()->setScale(sf::Vector2f(factorX, factorY));

}

void Rendering::CreateNextUpSprites()
{
	m_NextUpBubble = std::make_unique<sf::Sprite>(*m_NextUpTexture);

	sf::Vector2f vector = BubbleMath::ToVector2f(m_NextUpTexture->getSize());
	sf::Vector2f size = sf::Vector2f(vector.x / 6.f, vector.y / 6.f);
	m_NextUpBubble->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(static_cast<int>(size.x),static_cast<int>(vector.y))));
	size = BubbleMath::ToVector2f(m_NextUpBubble->getTexture().getSize());


	float factorY = Settings::get().GetNextUpHeight() / size.y;
	m_NextUpBubble->setScale(sf::Vector2f(factorY,  factorY));


	float x = m_CycleSprite->getGlobalBounds().position.x;
	float y = m_CycleSprite->getGlobalBounds().position.y;
	m_NextUpBubble->setPosition(sf::Vector2f(x,y));

}

void Rendering::CreateHighScoreSpriteInPlay(sf::Vector2f& position)
{
	//m_HighScoreTitleTextureInPlay = std::make_unique<sf::Texture>();
	//m_HighScoreTitleTextureInPlay->loadFromFile(PLAY_MODE_HIGHSCORE_TITLE_FILENAME);

	//m_HighScoreTitleInPlay = std::make_unique<sf::Sprite>(*m_HighScoreTitleTextureInPlay);
	//m_HighScoreTitleInPlay->setOrigin(sf::Vector2f(m_HighScoreTitleInPlay->getLocalBounds().size.x / 2.f, m_HighScoreTitleInPlay->getLocalBounds().size.y / 2.f));
	//float scaleX = Settings::get().GetScale();
	//m_HighScoreTitleInPlay->setScale(sf::Vector2f(0.3f * scaleX, 0.3f * Settings::get().GetScale()));

	position = m_ScoreTitle->getPosition();
	position.y += 350 *Settings::get().GetScale();
	//m_HighScoreTitleInPlay->setPosition(position);
	m_HighScoreTextureInPlay = std::make_unique<sf::Texture>();m_HighScoreTextureInPlay->loadFromFile(PLAY_MODE_SCOREBOARD);

	m_HighScoreSpriteInPlay = std::make_unique<sf::Sprite>(*m_HighScoreTextureInPlay);
	m_HighScoreSpriteInPlay->setOrigin(sf::Vector2f(m_HighScoreSpriteInPlay->getLocalBounds().size.x / 2.f, m_HighScoreSpriteInPlay->getLocalBounds().size.y / 2.f));
	m_HighScoreSpriteInPlay->setScale(sf::Vector2f(0.4f * Settings::get().GetScale(), 0.4f * Settings::get().GetScale()));
	m_HighScoreSpriteInPlay->setPosition(position);
}

void Rendering::CreatePlayScoreSprites()
{
	m_ScoreTitleTexture = std::make_unique<sf::Texture>();
	m_ScoreTitleTexture->loadFromFile(PLAY_MODE_SCORE_TITLE_FILENAME);

	m_ScoreTitle = std::make_unique<sf::Sprite>(*m_ScoreTitleTexture);
	m_ScoreTitle->setScale(sf::Vector2f(0.6 * Settings::get().GetScale(), 0.6f * Settings::get().GetScale()));
	m_ScoreTitle->setOrigin(sf::Vector2f(m_ScoreTitle->getLocalBounds().size.x / 2.f, m_ScoreTitle->getLocalBounds().size.y / 2.f));

	auto position = sf::Vector2f(m_Frame->getGlobalBounds().position.x, m_Frame->getGlobalBounds().position.y);
	position.y += m_Frame->getGlobalBounds().size.y / 6.f;
	position.x -= m_ScoreTitle->getGlobalBounds().size.x;
	m_ScoreTitle->setPosition(position);

	position.y += m_ScoreTitle->getGlobalBounds().size.y * 1.1f;
	m_Score->setPosition(position);

	int size = static_cast<int>(38.f * Settings::get().GetScale());


	CreateHighScoreSpriteInPlay(position);

	sf::Color textColor = sf::Color(225, 142, 149, 255);

	m_HighScoresInPlay = std::vector<std::unique_ptr<sf::Text>>(3);
	m_HighScoresInPlay[0] = std::make_unique<sf::Text>(*m_FontBold);

	m_HighScoresInPlay[0]->setString(std::to_string(rand()));
	//position.x -= 78 * Settings::get().GetScale();
	position.y = m_HighScoreSpriteInPlay->getGlobalBounds().getCenter().y - m_HighScoreSpriteInPlay->getGlobalBounds().size.y / 4.1f;
	position.x = m_HighScoreSpriteInPlay->getGlobalBounds().getCenter().x - m_HighScoreSpriteInPlay->getGlobalBounds().size.x / 10;
	m_HighScoresInPlay[0]->setPosition(position);
	m_HighScoresInPlay[0]->setFont(*m_FontBold);
	m_HighScoresInPlay[0]->setFillColor(textColor);
	//m_HighScoresInPlay[0]->setOutlineColor(sf::Color::White);
	//m_HighScoresInPlay[0]->setOutlineThickness(3);
	m_HighScoresInPlay[0]->setCharacterSize(size);

	m_HighScoresInPlay[1] = std::make_unique<sf::Text>(*m_FontBold);
	m_HighScoresInPlay[1]->setString(std::to_string(rand()));
	position.y += m_HighScoreSpriteInPlay->getGlobalBounds().size.y / 4;
	m_HighScoresInPlay[1]->setPosition(position);
	m_HighScoresInPlay[1]->setFillColor(textColor);
	//m_HighScoresInPlay[1]->setOutlineColor(sf::Color::White);
	//m_HighScoresInPlay[1]->setOutlineThickness(3);
	m_HighScoresInPlay[1]->setCharacterSize(size);


	m_HighScoresInPlay[2] = std::make_unique<sf::Text>(*m_FontBold);
	m_HighScoresInPlay[2]->setString(std::to_string(rand()));
	position.y += m_HighScoreSpriteInPlay->getGlobalBounds().size.y / 4;
	m_HighScoresInPlay[2]->setPosition(position);
	m_HighScoresInPlay[2]->setFillColor(textColor);
	//m_HighScoresInPlay[2]->setOutlineColor(sf::Color::White);
	//m_HighScoresInPlay[2]->setOutlineThickness(3);
	m_HighScoresInPlay[2]->setCharacterSize(size);

}

void Rendering::CreatePlayModeButtons()
{
	sf::Vector2f basePos;
	sf::Vector2f BBTextureSize = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	float buttonWidth = (Settings::get().GetButtonWidth()) / (BBTextureSize.x / 3.f);
	basePos.x = m_Frame->getGlobalBounds().position.x - Settings::get().GetButtonWidth() * 2;
	basePos.y = m_Frame->getGlobalBounds().position.y + m_Frame->getGlobalBounds().size.y - Settings::get().GetButtonHeight() / 2;

	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos, *m_Font, m_HighScoresButtonTexture.get());
	newButton->SetText("menu");
	newButton->ResizeCharacters(std::round(35.f * Settings::get().GetScale()));
	sf::Vector2f buttonScale = sf::Vector2f(0.25f * Settings::get().GetScale(), 0.25f *Settings::get().GetScale());

	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Back to menu", std::move(newButton)));

	basePos.x += m_MenuButtons.at("Back to menu")->GetWidth() * 1.05f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_HighScoresButtonTexture.get());
	newButton->SetText("restart");
	newButton->ResizeCharacters(std::round(35.f * Settings::get().GetScale()));
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Restart", std::move(newButton)));

	CreatePlayScoreSprites();
}

void Rendering::CreateConfirmationWindow()
{
	m_ConfirmationTexture = std::make_unique<sf::Texture>();
	m_ConfirmationTexture->loadFromFile(CONFIRM_FILENAME);

	m_ConfirmationWindow = std::make_unique<sf::RectangleShape>();
	m_ConfirmationWindow->setTexture(m_ConfirmationTexture.get());
	m_ConfirmationWindow->setSize(sf::Vector2f(Settings::get().GetConfirmationWidth(), Settings::get().GetConfirmationHeight()));
	m_ConfirmationWindow->setOrigin(sf::Vector2f(m_ConfirmationWindow->getSize().x / 2, m_ConfirmationWindow->getSize().y / 3));

	sf::Vector2f size = BubbleMath::ToVector2f(m_Window->getSize());
	m_ConfirmationWindow->setPosition(sf::Vector2f(size.x/ 2, size.y / 2));

	sf::Vector2f BBTextureSize = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetButtonWidth() / (BBTextureSize.x / 4.f), Settings::get().GetButtonHeight() / BBTextureSize.y);
	sf::Vector2f basePos = m_ConfirmationWindow->getGlobalBounds().position;
	basePos.x += m_ConfirmationWindow->getGlobalBounds().size.x / 4;
	basePos.y += m_ConfirmationWindow->getGlobalBounds().size.y / 1.3f;
	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Yes");
	newButton->ResizeCharacters(std::round(30.f * Settings::get().GetScale()));
	newButton->SetScale(sf::Vector2f(buttonScale));
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("ConfirmConfirm", std::move(newButton)));

	basePos.x += m_ConfirmationWindow->getGlobalBounds().size.x / 2;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("No");
	newButton->ResizeCharacters(std::round(30.f * Settings::get().GetScale()));
	newButton->SetScale(sf::Vector2f(buttonScale));
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("CancelConfirm", std::move(newButton)));

	m_ConfirmationText = std::make_unique<sf::Text>(*m_Font);
	m_ConfirmationText->setFillColor(sf::Color::Black);
	m_ConfirmationText->setString(CONFIRM_EXIT_GAME);
	m_ConfirmationText->setOrigin(sf::Vector2f(m_ConfirmationText->getLocalBounds().position.x + m_ConfirmationText->getLocalBounds().size.x / 2,
		m_ConfirmationText->getLocalBounds().position.y + m_ConfirmationText->getLocalBounds().size.y / 2));
	m_ConfirmationText->setPosition(m_ConfirmationWindow->getPosition());
	m_ConfirmationText->setCharacterSize(std::round(25 * Settings::get().GetScale()));


}

void Rendering::CreateSettingsButtons()
{
	sf::Vector2f position = m_MenuButtons.at("Back to menu")->GetPosition();
	std::unique_ptr<Button> newButton = std::make_unique<Button>(position, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Apply");
	newButton->ResizeCharacters(std::round(30.f * Settings::get().GetScale()));
	sf::Vector2f BBTextureSize = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetMenuButtonWidth() / (BBTextureSize.x / 3.f), Settings::get().GetButtonHeight() / (BBTextureSize.y));

	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("ApplySettings", std::move(newButton)));

	position.x += m_MenuButtons.at("Back to menu")->GetWidth() * 2.f;
	newButton = std::make_unique<Button>(position, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Revert");
	newButton->ResizeCharacters(std::round(30.f * Settings::get().GetScale()));
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Revert", std::move(newButton)));
}

void Rendering::CreateSettings()
{
	m_SettingsTexture = std::make_unique<sf::Texture>();
	m_SettingsTexture->loadFromFile(SETTINGS_FILENAME);
	m_ArrowTexture = std::make_unique<sf::Texture>();
	m_ArrowTexture->loadFromFile(RESOLUTION_ARROW);

	m_SettingsTitle = std::make_unique<sf::RectangleShape>();
	m_SettingsTitle->setTexture(m_SettingsTexture.get());
	sf::Vector2f titleSize = m_Title->getSize();
	sf::Vector2f vector2U = BubbleMath::ToVector2f(m_SettingsTexture->getSize());
	float scale = titleSize.y / vector2U.y;
	titleSize.x = vector2U.x * scale;
	m_SettingsTitle->setSize(titleSize);
	m_SettingsTitle->setOrigin(sf::Vector2f(m_SettingsTitle->getSize().x / 2, m_SettingsTitle->getSize().y / 2));
	m_SettingsTitle->setPosition(m_Title->getPosition());

	sf::Vector2f position = sf::Vector2f(m_SettingsTitle->getPosition().x, m_SettingsTitle->getPosition().y + m_SettingsTitle->getGlobalBounds().size.y / 2.f);
	m_SettingSliders = std::vector<std::unique_ptr<Slider>>();
	m_SettingSliders.push_back(std::make_unique<Slider>(position, 100 * Settings::get().GetScale()));
	position.y += m_SettingsTitle->getGlobalBounds().size.y / 2.f;
	m_SettingSliders.push_back(std::make_unique<Slider>(position, 100 * Settings::get().GetScale()));

	position.y += m_SettingsTitle->getGlobalBounds().size.y / 2.f;

	m_CheckboxTexture = std::make_unique<sf::Texture>();
	m_CheckboxTexture->loadFromFile(CHECKBOX_FILENAME);
	m_FullscreenCheckbox = std::make_unique<Checkbox>(m_CheckboxTexture, position, 50 *Settings::get().GetScale());
	m_FullscreenCheckbox->SetEnableCheckbox(Settings::get().IsFullscreen());

	std::string temp = Settings::get().GetResSizeAsString(Settings::get().GetCurrentRes());
	m_Resolution_List = std::make_unique<sf::Text>(*m_Font, temp);
	m_Resolution_List->setFillColor(sf::Color::Black);
	position.y += m_SettingsTitle->getGlobalBounds().size.y / 1.5f;
	m_Resolution_List->setPosition(position);
	m_Resolution_List->setOrigin(sf::Vector2f(0, m_Resolution_List->getGlobalBounds().size.y));

	m_SettingsText = std::vector<std::unique_ptr<sf::Text>>();

	m_SettingsText.push_back(std::make_unique<sf::Text>(*m_Font,"Music volume:"));
	m_SettingsText[0]->setFillColor(sf::Color::Black);
	m_SettingsText[0]->setOrigin(sf::Vector2f(0, m_SettingsText[0]->getGlobalBounds().size.y));

	m_SettingsText.push_back(std::make_unique<sf::Text>(*m_Font,"Sound effects:"));
	m_SettingsText[1]->setFillColor(sf::Color::Black);
	m_SettingsText[1]->setOrigin(sf::Vector2f(0, m_SettingsText[1]->getGlobalBounds().size.y));

	//m_SettingsText.push_back(std::make_unique<sf::Text>(*m_Font,"Fullscreen:"));
	//m_SettingsText[2]->setFillColor(sf::Color::Black);
	//m_SettingsText[2]->setOrigin(sf::Vector2f(0, m_SettingsText[2]->getGlobalBounds().size.y));

	m_SettingsText.push_back(std::make_unique<sf::Text>(*m_Font, "Resolution:"));
	m_SettingsText[2]->setFillColor(sf::Color::Black);
	m_SettingsText[2]->setOrigin(sf::Vector2f(0, m_SettingsText[2]->getGlobalBounds().size.y));

	position.x += m_Resolution_List->getGlobalBounds().size.x * 1.3f;
	m_RightResArrow = std::make_unique<Button>(position, *m_Font, m_ArrowTexture.get());
	m_RightResArrow->SetScale(sf::Vector2f(0.3f, 0.3f));
	float lengths[] = {m_SettingsText[0]->getGlobalBounds().size.x, m_SettingsText[1]->getGlobalBounds().size.x, m_SettingsText[2]->getGlobalBounds().size.x};
	float max = std::max(lengths[0], lengths[1]);
	max = std::max(max, lengths[2]);
	m_SettingsText[0]->setPosition(sf::Vector2f(m_SettingsTitle->getPosition().x - max - m_SettingsTitle->getPosition().x / 8.f, m_SettingSliders[0]->GetSliderPosition().y));
	m_SettingsText[1]->setPosition(sf::Vector2f(m_SettingsTitle->getPosition().x - max - m_SettingsTitle->getPosition().x / 8.f, m_SettingSliders[1]->GetSliderPosition().y));
	//m_SettingsText[2]->setPosition(sf::Vector2f(m_SettingsTitle->getPosition().x - max - m_SettingsTitle->getPosition().x / 8.f, m_FullscreenCheckbox->GetPosition().y + 25 * Settings::get().GetScale()));
	m_SettingsText[2]->setPosition(sf::Vector2f(m_SettingsTitle->getPosition().x - max - m_SettingsTitle->getPosition().x / 8.f, m_Resolution_List->getPosition().y));


	CreateSettingsButtons();

}

void Rendering::CreateStorageSprites()
{
	m_StoredSprite = std::make_unique<sf::Sprite>(*m_StorageTextures.at(EBUBBLE_TYPE::TYPE_NULL));
	float size = Settings::get().GetStorageBoxWidth() / m_StoredSprite->getTexture().getSize().x;
	m_StoredSprite->setScale(sf::Vector2f(size, size));

	sf::FloatRect localBounds = m_StoredSprite->getLocalBounds();
	m_StoredSprite->setOrigin(sf::Vector2f(localBounds.position.x + localBounds.size.x / 2.f,
	                          localBounds.position.y + localBounds.size.y / 2.f));

	m_StoredSprite->setPosition(sf::Vector2f(m_Frame->getGlobalBounds().position.x + m_Frame->getGlobalBounds().size.x * 1.1f, 
		m_Frame->getGlobalBounds().position.y + m_Frame->getGlobalBounds().size.y - m_StoredSprite->getGlobalBounds().size.y / 2.f));

	m_MovingStorageSprite = std::make_unique<sf::Sprite>(*m_StorageTextures.at(EBUBBLE_TYPE::TYPE_NULL));

	localBounds = m_MovingStorageSprite->getLocalBounds();
	m_MovingStorageSprite->setOrigin(sf::Vector2f(localBounds.position.x + localBounds.size.x / 2.f,
									localBounds.position.y + localBounds.size.y / 2.f));

	m_StorageText = std::make_unique<sf::Text>(*m_Font);
	m_StorageText->setOutlineColor(sf::Color::White);
	m_StorageText->setOutlineThickness(3 * Settings::get().GetScale());
	m_StorageText->setCharacterSize(std::round(38 * Settings::get().GetScale()));
	m_StorageText->setFillColor(sf::Color(225, 142, 149, 255));
	m_StorageText->setStyle(sf::Text::Bold);
	m_StorageText->setString("Storage:");

	localBounds = m_StorageText->getLocalBounds();
	m_StorageText->setOrigin(sf::Vector2f(localBounds.position.x + localBounds.size.x / 2.f,
		localBounds.position.y));
	m_StorageText->setPosition(sf::Vector2f(m_StoredSprite->getPosition().x, m_StoredSprite->getPosition().y - m_StoredSprite->getGlobalBounds().size.y * 0.5f));


	m_InstructionTexture = std::make_unique<sf::Texture>();
	m_InstructionTexture->loadFromFile(INSTRUCTION_FILENAME);
	m_InstructionShape = std::make_unique<sf::Sprite>(*m_InstructionTexture);
	m_InstructionShape->setOrigin(sf::Vector2f(m_InstructionShape->getLocalBounds().size.x / 2.f, m_InstructionShape->getLocalBounds().size.y / 2.f));
	m_InstructionShape->setScale(sf::Vector2f(0.35f * Settings::get().GetScale(), 0.35f * Settings::get().GetScale()));

	float x = m_StoredSprite->getPosition().x + m_StoredSprite->getGlobalBounds().size.x / 1.3f + m_InstructionShape->getGlobalBounds().size.x / 2.f;
	m_InstructionShape->setPosition(sf::Vector2f(x, m_StoredSprite->getPosition().y));
}

void Rendering::CreateCycleBottle()
{
	m_CycleSprite = std::make_unique<sf::RectangleShape>();
	m_CycleSprite->setTexture(m_CycleTexture.get());

	sf::Vector2f cycleTextureSize = BubbleMath::ToVector2f(m_CycleTexture->getSize());

	float temp = (m_Container->getSize().y * 0.8f) / cycleTextureSize.y;
	sf::Vector2f f(m_CycleTexture->getSize().x * temp, m_CycleTexture->getSize().y * temp);
	m_CycleSprite->setSize(f);
	m_CycleSprite->setOrigin(sf::Vector2f(m_CycleSprite->getLocalBounds().position.x + m_CycleSprite->getLocalBounds().size.x / 2.f,
		m_CycleSprite->getLocalBounds().position.y + m_CycleSprite->getLocalBounds().size.y / 2.f));

	sf::Vector2f position;
	position.y = m_Frame->getPosition().y - m_Frame->getGlobalBounds().size.y / 8.f;
	position.x = m_Frame->getGlobalBounds().position.x + m_Frame->getGlobalBounds().size.x;
	position.x += m_CycleSprite->getLocalBounds().size.x / 2.f;
	m_CycleSprite->setPosition(position);
}

void Rendering::CreateScoreNumberSprites()
{
	m_ScoreNumberTextures = std::vector<std::unique_ptr<sf::Texture>>(10);
	m_ScoreNumberSprites = std::vector<std::unique_ptr<sf::Sprite>>(10);

	for(int i = 0; i < 10; i++)
	{
		m_ScoreNumberTextures[i] = std::make_unique<sf::Texture>();
		m_ScoreNumberTextures[i]->loadFromFile(SCORE_PATH + std::to_string(i) + ".png");
	}

	for (int i = 0; i < 10; i++)
	{
		m_ScoreNumberSprites[i] = std::make_unique<sf::Sprite>(*m_ScoreNumberTextures[i]);
		m_ScoreNumberSprites[i]->setScale(sf::Vector2f(0.3f * Settings::get().GetScale(), 0.3f * Settings::get().GetScale()));
		m_ScoreNumberSprites[i]->setPosition(sf::Vector2f(m_ScoreTitle->getPosition().x, m_ScoreTitle->getPosition().y + m_ScoreTitle->getGlobalBounds().size.y / 2 + 10));
		
	}

	m_ScoreBackgroundInPlayTexture = std::make_unique<sf::Texture>();
	m_ScoreBackgroundInPlayTexture->loadFromFile(SCORE_CLOUD_FILENAME);
	m_ScoreBackgroundInPlaySprite = std::make_unique<sf::Sprite>(*m_ScoreBackgroundInPlayTexture);
	m_ScoreBackgroundInPlaySprite->setOrigin(sf::Vector2f(m_ScoreBackgroundInPlaySprite->getLocalBounds().size.x / 2, m_ScoreBackgroundInPlaySprite->getLocalBounds().size.y / 2));
	m_ScoreBackgroundInPlaySprite->setScale(sf::Vector2f(0.3f * Settings::get().GetScale(), 0.3f * Settings::get().GetScale()));
	m_ScoreBackgroundInPlaySprite->setPosition(sf::Vector2f(m_ScoreTitle->getPosition().x, m_ScoreTitle->getPosition().y + m_ScoreNumberSprites[0]->getGlobalBounds().size.y
											+ m_ScoreBackgroundInPlaySprite->getGlobalBounds().size.y / 2));
}
