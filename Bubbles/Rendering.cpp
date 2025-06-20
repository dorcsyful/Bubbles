#include "Rendering.h"

#include <ranges>
#include <SFML/Window/Event.hpp>

#include "Audio.h"
#include "BubbleObject.h"
#include "Declarations.h"
#include "FilePaths.h"
#include "LineObject.h"
#include "Settings.h"

Rendering::Rendering(const int a_X, const int a_Y, std::vector<std::unique_ptr<AnimatedSprite>>& a_Wrapper):
	m_RenderedBubbles(a_Wrapper)
{
	sf::ContextSettings context;
	context.antialiasingLevel = Settings::get().GetAaLevel();

	if(Settings::get().IsFullscreen())
	{
		m_Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), "Bubbles!",
													 sf::Style::Fullscreen, context);
		Settings::get().IncreaseIfFullScreen(static_cast<float>(sf::VideoMode::getDesktopMode().width), static_cast<float>(sf::VideoMode::getDesktopMode().height));

	}
	else
	{
		sf::Vector2u size = sf::Vector2u(static_cast<int>(Settings::get().GetWindowWidth()), static_cast<int>(Settings::get().GetWindowHeight()));
		m_Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(size.x, size.y), "Bubbles!",
			sf::Style::Default, context);

	}
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
	m_Window->draw(*m_BackgroundExtendedSprite);
	m_Window->draw(*m_BackgroundSprite);

	sf::Vector2f mousePosition = m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window));
	m_MenuButtons.at("Back to menu")->DetectHover(mousePosition);
	m_MenuButtons.at("Restart")->DetectHover(mousePosition);

	m_Window->draw(*m_Container);
	m_Window->draw(*m_Frame);
	m_Window->draw(*m_Line);

	m_Duck->Draw(*m_Window);

	m_Window->draw(*m_NextUpBubbles.at(m_ActiveNextUp));
	m_PreviewBubbles.at(m_ActiveBubble)->Draw(*m_Window);
	for (auto& element : m_RenderedBubbles)
	{
		element->Draw(*m_Window);
	}
	m_Window->draw(*m_HighScoreSpriteInPlay);

	m_MenuButtons.at("Back to menu")->Draw(*m_Window);
	m_MenuButtons.at("Restart")->Draw(*m_Window);
	//m_Window->draw(*m_Score);
	m_Window->draw(*m_ComboText);
	m_Window->draw(*m_ScoreTitle);
	m_Window->draw(*m_HighScoreTitleInPlay);
	m_Window->draw(*m_StoredSprite);
	m_Window->draw(*m_StorageText);

	m_Window->draw(*m_NextUpBubbles.at(m_ActiveNextUp));

	float lastPosition = m_ScoreStartPosition;
	std::string scoreString = m_Score->getString();
	for (size_t i = 0; i < scoreString.size(); i++)
	{
		unsigned char asChar = scoreString[i];
		int value = asChar - '0';
		m_ScoreNumberSprites[value]->setPosition(lastPosition, m_ScoreNumberSprites[value]->getPosition().y);
		m_Window->draw(*m_ScoreNumberSprites[value]);
		lastPosition += m_ScoreNumberSprites[value]->getGlobalBounds().width + 2;
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
	m_Window->draw(*m_ScoreBackgroundInPlaySprite);
	m_Window->draw(*m_CycleSprite);
	m_Window->draw(*m_InstructionShape);
	m_Window->draw(*m_HighScoresInPlay[0]);
	m_Window->draw(*m_HighScoresInPlay[1]);
	m_Window->draw(*m_HighScoresInPlay[2]);
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
	m_MenuButtons.at("How to play")->DetectHover(mousePosition);


	m_MenuButtons.at("Play")->Draw(*m_Window);
	m_MenuButtons.at("High_Score")->Draw(*m_Window);
	m_MenuButtons.at("Settings")->Draw(*m_Window);
	m_MenuButtons.at("Exit")->Draw(*m_Window);
	m_MenuButtons.at("How to play")->Draw(*m_Window);
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
		m_GOScoreNumberSprites[value]->setPosition(lastPosition, m_GOScoreNumberSprites[value]->getPosition().y);
		m_Window->draw(*m_GOScoreNumberSprites[value]);
		lastPosition += m_GOScoreNumberSprites[value]->getGlobalBounds().width + 2;
	}
	m_Window->draw(*m_GOScoreCloudSprite);
	m_MenuButtons.at("PlayAgain")->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	m_MenuButtons.at("BackToMenu")->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	m_MenuButtons.at("BackToMenu")->Draw(*m_Window);
	m_MenuButtons.at("PlayAgain")->Draw(*m_Window);
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
	m_FullscreenCheckbox->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	m_Window->draw(*m_FullscreenCheckbox);
	m_MenuButtons.at("ApplySettings")->Draw(*m_Window);
	m_MenuButtons.at("Revert")->Draw(*m_Window);

	m_Window->draw(*m_SettingsTitle);
}

void Rendering::HowToDraw() const
{
	for(auto& current : m_TutorialSprites)
	{
		m_Window->draw(*current);
	}

	for (auto& current : m_TutorialTexts)
	{
		m_Window->draw(*current);
	}
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

	sf::Vector2f temp = m_Line->getPosition();
	temp.x = a_X;
	m_Line->setPosition(temp);
	if(a_X == m_Duck->GetPosition().x)  // NOLINT(clang-diagnostic-float-equal)
	{
		m_Duck->SetAnimate(false, false);
	}
	else
	{
		m_Duck->SetAnimate(true, true);
	}
	if (a_X > m_Duck->GetPosition().x)
	{
		m_Duck->GetSprite()->setScale(-abs(m_Duck->GetSprite()->getScale().x), m_Duck->GetSprite()->getScale().y);
	}
	if(a_X <m_Duck->GetPosition().x)
	{
		m_Duck->GetSprite()->setScale(abs(m_Duck->GetSprite()->getScale().x), m_Duck->GetSprite()->getScale().y);
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

void Rendering::Reset()
{
	m_StoredSprite->setTexture(*m_StorageTextures.at(EBUBBLE_TYPE::TYPE_NULL));
	m_ActiveBubble = EBUBBLE_TYPE::TYPE_STAR;
}

void Rendering::UpdateHighScores(const std::vector<unsigned int>& a_Scores) const
{
	for(int i = 0; i < 10; i++)
	{
		m_HighScoreSprites[i]->SetText(std::to_string(a_Scores[i]));
	}
	m_Score->setString(m_HighScoreSprites[0]->GetText());

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
	sf::Vector2f position = localBounds.getPosition();
	m_ConfirmationText->setOrigin(position.x + localBounds.width / 2, position.y + localBounds.height / 2);
	m_ConfirmationText->setPosition(m_ConfirmationWindow->getPosition());

}

void Rendering::UpdateComboPosition(const sf::Vector2f& a_NewPos) const
{
	sf::FloatRect textBounds = m_ComboText->getLocalBounds();
	sf::FloatRect rectBounds = m_Container->getGlobalBounds();

	// Adjust coordinate to fit within rectangle bounds
	sf::Vector2f adjustedCoord = a_NewPos;
	adjustedCoord.x = std::max(rectBounds.left, std::min(rectBounds.left + rectBounds.width - textBounds.width, adjustedCoord.x));
	adjustedCoord.y = std::max(rectBounds.top, std::min(rectBounds.top + rectBounds.height - textBounds.height, adjustedCoord.y));

	// Position the text
	m_ComboText->setPosition(adjustedCoord);

}

void Rendering::UpdateHighScore(const std::vector<unsigned int>& a_Scores) const
{
	for (size_t i = 0; i < a_Scores.size(); i++)
	{
		m_HighScoreSprites[i]->SetText(std::to_string(a_Scores[i]));
	}

	m_HighScoresInPlay[0]->setString(std::to_string(a_Scores[0]));
	m_HighScoresInPlay[1]->setString(std::to_string(a_Scores[1]));
	m_HighScoresInPlay[2]->setString(std::to_string(a_Scores[2]));
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
	m_MovingStorageSprite->setTextureRect(sf::IntRect(0, 0, u.x / 8, u.y));
	float storage_box_width = Settings::get().GetStorageBoxWidth();
	sf::Vector2f size = BubbleMath::ToVector2f(m_MovingStorageSprite->getTexture()->getSize());
	float factor_x = storage_box_width / (size.x / 8.f);
	float factor_y = storage_box_width / size.y;
	m_MovingStorageSprite->setScale(factor_x, factor_y);

	m_TypeInStorage = a_ToStorage ? a_Type : EBUBBLE_TYPE::TYPE_NULL;
	m_MovingStorageLerp = a_ToStorage ? 0.000001f : 0.999999f;
	m_MovingDirection = a_ToStorage ? 1 : -1;
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
	m_NextUpTextures = std::map<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>();

	for(int i = 0; i < 4; i++)
	{
		m_NextUpTextures.insert(std::pair(static_cast<EBUBBLE_TYPE>(i), std::make_unique<sf::Texture>()));
		m_NextUpTextures[static_cast<EBUBBLE_TYPE>(i)]->loadFromFile(NEXT_UP_PATH + std::to_string(i) + ".png");
	}

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_BATH_BOMB, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_BATH_BOMB]->loadFromFile(NEXT_UP_SPECIAL_PATH);
	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_NULL, std::make_unique<sf::Texture>()));
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

	float verticalScale = windowSize.y / m_BackgroundTexture->getSize().y;
	float horizontalScale = verticalScale;

	m_BackgroundSprite->setSize(BubbleMath::ToVector2f(m_BackgroundTexture->getSize()));
	m_BackgroundSprite->setScale(horizontalScale, verticalScale);
	m_BackgroundSprite->setOrigin(m_BackgroundSprite->getLocalBounds().width / 2, m_BackgroundSprite->getLocalBounds().height / 2);
	m_BackgroundSprite->setPosition(windowSize.x / 2, windowSize.y / 2);

	m_BackgroundExtendedTexture = std::make_unique<sf::Texture>();
	m_BackgroundExtendedTexture->loadFromFile(MENU_BACKGROUND_EXTENDED_FILENAME);
	m_BackgroundExtendedTexture->setRepeated(true);
	m_BackgroundExtendedSprite = std::make_unique<sf::RectangleShape>();
	m_BackgroundExtendedSprite->setTexture(m_MainBackgroundExtendedTexture.get());
	m_BackgroundExtendedSprite->setSize(sf::Vector2f(m_Window->getSize().x, m_Window->getSize().y));

	//Main Background
	m_MainBackgroundTexture = std::make_unique<sf::Texture>();
	m_MainBackgroundTexture->loadFromFile(MENU_BACKGROUND_FILENAME);
	m_MainBackgroundTexture->setRepeated(false);
	m_MainBackgroundSprite = std::make_unique<sf::RectangleShape>();
	m_MainBackgroundSprite->setTexture(m_MainBackgroundTexture.get());
	// Calculate scaling factors and set sprite size and origin
	sf::Vector2f backgroundTextureSize = BubbleMath::ToVector2f(m_MainBackgroundTexture->getSize());

	verticalScale = (float)windowSize.y / backgroundTextureSize.y;
	horizontalScale = verticalScale;
	m_MainBackgroundSprite->setSize(backgroundTextureSize);
	m_MainBackgroundSprite->setScale(horizontalScale, verticalScale);
	m_MainBackgroundSprite->setOrigin(m_MainBackgroundSprite->getSize().x / 2.f, m_MainBackgroundSprite->getSize().y / 2.f);
	m_MainBackgroundSprite->setPosition(windowSize.x / 2, windowSize.y / 2);


	m_MainBackgroundExtendedTexture = std::make_unique<sf::Texture>();
	m_MainBackgroundExtendedTexture->loadFromFile(MENU_BACKGROUND_EXTENDED_FILENAME);
	m_MainBackgroundExtendedTexture->setRepeated(true);
	m_MainBackgroundExtendedSprite = std::make_unique<sf::RectangleShape>();
	m_MainBackgroundExtendedSprite->setTexture(m_MainBackgroundExtendedTexture.get());
	m_MainBackgroundExtendedSprite->setSize(sf::Vector2f(m_Window->getSize().x, m_Window->getSize().y));

	//Container
	m_ContainerTexture = std::make_unique<sf::Texture>();
	if(!m_ContainerTexture->loadFromFile(CONTAINER_FILENAME))
	{
		//throw std::exception("Failed to load container texture");
	}
	m_ContainerTexture->setRepeated(false);
	m_Container = std::make_unique<sf::RectangleShape>();
	m_Container->setTexture(m_ContainerTexture.get());
	float width = Settings::get().GetContainerWidth();
	float height = Settings::get().GetContainerHeight() ;
	m_Container->setSize(sf::Vector2f(width, height));
	sf::Vector2f basePos = sf::Vector2f((windowSize.x / 2.f) - (width / 2.f), ((windowSize.y - height) / 1.755f));
	m_Container->setPosition(basePos);

	m_FrameTexture = std::make_unique<sf::Texture>();
	m_FrameTexture->loadFromFile(FRAME_FILENAME);
	m_Frame = std::make_unique<sf::RectangleShape>();
	m_Frame->setTexture(m_FrameTexture.get());

	sf::Vector2f containerSize = BubbleMath::ToVector2f(m_ContainerTexture->getSize());
	sf::Vector2f frameSize = BubbleMath::ToVector2f(m_FrameTexture->getSize());
	float x = (Settings::get().GetContainerWidth() / containerSize.x) * frameSize.x;
	float y = (Settings::get().GetContainerHeight() / containerSize.y) * frameSize.y;
	m_Frame->setSize(sf::Vector2f(x, y));

	m_Frame->setOrigin(m_Frame->getSize().x / 2.f, m_Frame->getSize().y / 2.f);
	m_Frame->setPosition(m_Container->getPosition().x + m_Container->getSize().x / 2, m_Container->getPosition().y + m_Container->getSize().y / 2 - Settings::get().GetFrameCorrection());
}

void Rendering::CreateSprite(const EBUBBLE_TYPE a_Type, const sf::Vector2f& a_Position, const float a_Rotation, std::unique_ptr<AnimatedSprite>& a_NewSprite) const
{
	a_NewSprite = std::make_unique<AnimatedSprite>(m_BubbleTextures.at(a_Type).get(), Settings::get().GetBubbleAnimationTotalTime(), Settings::get().GetBubbleFrames());

	sf::Vector2f size = BubbleMath::ToVector2f(m_BubbleTextures.at(a_Type)->getSize());
	size.x /= static_cast<float>(Settings::get().GetBubbleFrames());
	float pixelToMeter = Settings::get().GetPixelToMeter();
	float factorX = Settings::get().BubbleSize(a_Type) * pixelToMeter * 2 / size.x;
	float factorY = Settings::get().BubbleSize(a_Type) * pixelToMeter * 2 / size.y;
	a_NewSprite->GetSprite()->setScale(factorX, factorY);

	float x = Settings::get().BubbleSize(a_Type) * pixelToMeter / a_NewSprite->GetSprite()->getScale().x;
	float y = Settings::get().BubbleSize(a_Type) * pixelToMeter / a_NewSprite->GetSprite()->getScale().y;
	a_NewSprite->GetSprite()->setOrigin(x, y);

	a_NewSprite->SetPosition(a_Position);
	a_NewSprite->SetRotation(a_Rotation);
}

void Rendering::CreatePointer()
{
	float containerHeight = Settings::get().GetContainerHeight();
	sf::Vector2f size = BubbleMath::ToVector2f(m_Window->getSize());
	sf::Vector2f position =
		sf::Vector2f(m_Container->getGlobalBounds().left, m_Container->getGlobalBounds().top);
	m_Line = std::make_unique<sf::RectangleShape>(sf::Vector2f(5 / 2.f, containerHeight));
	m_Line->setFillColor(sf::Color(255, 0, 0, 255));
	m_Line->setPosition(position);

	for(size_t i = 0; i < 12; i++)
	{
		std::unique_ptr<AnimatedSprite> newSprite;
		CreateSprite(static_cast<EBUBBLE_TYPE>(i), sf::Vector2f(position), 0, newSprite);
		m_PreviewBubbles.insert(std::pair(static_cast<EBUBBLE_TYPE>(i), std::move(newSprite)));
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
	m_Title->setOrigin(titleTextureSize.x / 2.f, titleTextureSize.y / 2.f);
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
	m_GameOver->setOrigin(titleSize.x / 2, titleSize.y / 2);
	m_GameOver->setPosition(sf::Vector2f(m_Title->getPosition()));

	sf::Vector2f BBTextureSize = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetMenuButtonWidth() / (BBTextureSize.x / 3.7f), Settings::get().GetMenuButtonHeight() / BBTextureSize.y);

	sf::Vector2f basePos = m_GameOver->getPosition();
	basePos.x = m_GameOver->getGlobalBounds().left + Settings::get().GetMenuButtonWidth();
	basePos.y += m_GameOver->getGlobalBounds().height / 2 + m_ScoreNumberSprites[0]->getGlobalBounds().height + m_ScoreBackgroundInPlaySprite->getGlobalBounds().height;
	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Play again");
	newButton->ResizeCharacters(38 * Settings::get().GetScale());
	newButton->SetScale(buttonScale);
	
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("PlayAgain", std::move(newButton)));

	basePos.x += m_MenuButtons.at("PlayAgain")->GetWidth() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Back to menu");
	newButton->ResizeCharacters(38 * Settings::get().GetScale());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("BackToMenu", std::move(newButton)));

	m_GOScoreNumberSprites = std::vector<std::unique_ptr<sf::Sprite>>(10);
	for (int i = 0; i < 10; i++)
	{
		m_GOScoreNumberSprites[i] = std::make_unique<sf::Sprite>();
		m_GOScoreNumberSprites[i]->setTexture(*m_ScoreNumberTextures[i]);
		m_GOScoreNumberSprites[i]->setScale(0.3f * Settings::get().GetScale(), 0.3f * Settings::get().GetScale());
		m_GOScoreNumberSprites[i]->setPosition(m_GameOver->getGlobalBounds().left, m_GameOver->getPosition().y + m_GameOver->getGlobalBounds().height);
	}
	m_GOScoreCloudSprite = std::make_unique<sf::Sprite>();

	m_GOScoreCloudSprite->setTexture(*m_ScoreBackgroundInPlayTexture);
	m_GOScoreCloudSprite->setOrigin(m_GOScoreCloudSprite->getLocalBounds().width / 2, m_GOScoreCloudSprite->getLocalBounds().height / 2);
	m_GOScoreCloudSprite->setScale(0.3f * Settings::get().GetScale(), 0.3f * Settings::get().GetScale());
	float y = m_GOScoreNumberSprites[0]->getGlobalBounds().top + m_GOScoreNumberSprites[0]->getGlobalBounds().height - m_GOScoreCloudSprite->getGlobalBounds().height / 3.f;
	m_GOScoreCloudSprite->setPosition(m_GameOver->getGlobalBounds().left + m_GOScoreCloudSprite->getGlobalBounds().width / 2, y);


}

void Rendering::CreateMenuSprites()
{
	CreateTitleSprite();
	CreateMenuButtonSprites();
}

void Rendering::CreateMenuButtonSprites()
{
	m_Font = std::make_unique<sf::Font>();
	m_Font->loadFromFile(FONT_FILENAME);

	m_FontBold = std::make_unique<sf::Font>();
	m_FontBold->loadFromFile(FONT_BOLD_FILENAME);

	sf::Vector2f basePos = sf::Vector2f(m_Title->getPosition());
	basePos.y += m_Title->getSize().y + 10;
	basePos.x -= m_Title->getSize().x / 4;
	m_BaseButtonTexture = std::make_unique<sf::Texture>();
	m_BaseButtonTexture->loadFromFile(BUTTON_FILENAME);
	m_MenuButtons = std::map<std::string, std::unique_ptr<Button>>();
	sf::Vector2f BBTextureSize = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetMenuButtonWidth() / (BBTextureSize.x / 3.f), Settings::get().GetMenuButtonHeight() / BBTextureSize.y);

	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos,*m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Play");
	newButton->ResizeCharacters(Settings::get().GetMainButtonFontSize());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(),std::pair<std::string, std::unique_ptr<Button>>("Play", std::move(newButton)));

	basePos.y += Settings::get().GetMenuButtonHeight() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("High Scores");
	newButton->ResizeCharacters(Settings::get().GetMainButtonFontSize());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("High_Score", std::move(newButton)));

	basePos.y += Settings::get().GetMenuButtonHeight() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("How to play");
	newButton->ResizeCharacters(Settings::get().GetMainButtonFontSize());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("How to play", std::move(newButton)));

	basePos.x += m_Title->getSize().x / 2;
	basePos.y = m_Title->getPosition().y + m_Title->getSize().y + 10;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Settings");
	newButton->ResizeCharacters(Settings::get().GetMainButtonFontSize());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Settings", std::move(newButton)));

	basePos.y += Settings::get().GetMenuButtonHeight() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Exit");
	newButton->ResizeCharacters(Settings::get().GetMainButtonFontSize());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Exit", std::move(newButton)));

	m_LoadingTexture = std::make_unique<sf::Texture>();
	m_LoadingTexture->loadFromFile(LOADING_FILENAME);
	m_Loading = std::make_unique<AnimatedSprite>(m_LoadingTexture.get(), Settings::get().GetLoadTime(), Settings::get().GetLoadingFrames(), true);
	sf::Vector2f windowSize = BubbleMath::ToVector2f(m_Window->getSize());
	sf::Vector2f loadingTextureSize = BubbleMath::ToVector2f(m_LoadingTexture->getSize());
	m_Loading->SetPosition(sf::Vector2f(windowSize.x - loadingTextureSize.x / 8, windowSize.y - loadingTextureSize.y));
}

void Rendering::CreateScoreText()
{
	sf::Vector2f position = m_Frame->getPosition();
	position.x -= m_Frame->getSize().x / 1.5f;
	m_ScoreBackgroundTexture = std::make_unique<sf::Texture>();
	m_ScoreBackgroundTexture->loadFromFile(GENERIC_BACKGROUND_FILENAME);

	m_Score = std::make_unique<sf::Text>();
	m_Score->setFont(*m_Font);
	m_Score->setFillColor(sf::Color::Black);
	m_Score->setCharacterSize(70);
	m_Score->setString("0");
	m_Score->setPosition(position);

	m_ComboText = std::make_unique<sf::Text>();
	m_ComboText->setFont(*m_Font);
	m_ComboText->setCharacterSize(40);
	m_ComboText->setFillColor(sf::Color::Black);
	m_ComboText->setStyle(sf::Text::Bold);
	m_ComboText->setString("Combo:\n 0");
	m_ComboText->setOrigin(m_ComboText->getGlobalBounds().getPosition().y + m_ComboText->getGlobalBounds().height, 0);
	position = m_Container->getPosition();
	position.x += Settings::get().GetContainerWidth();
	position.y += 100;
	m_ComboText->setPosition(position);

}

void Rendering::CreateHighScoreSprites()
{
	m_HighScoreSprites = std::vector < std::unique_ptr<SpriteWithText>>(10);
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
	m_HighScoreTitle->setOrigin(static_cast<float>(m_HighScoreTitle->getSize().x) / 2.f, static_cast<float>(m_HighScoreTitle->getSize().y) / 2.f);
	m_HighScoreTitle->setPosition(m_Title->getPosition());

	basePos.y += m_Title->getSize().y + 10;

	for(int i = 0; i < 10; i ++)
	{
		sf::Color textColor = i % 2 == 0 ? sf::Color::Blue : sf::Color::Green;
		sf::Color shapeColor = i % 2 == 0 ? sf::Color::Green : sf::Color::Blue;

		m_HighScoreSprites[i] = std::make_unique<SpriteWithText>(std::to_string(rand()), *m_Font, sf::Vector2f(Settings::get().GetHighScoreItemWidth(), Settings::get().GetHighScoreItemHeight()),
							basePos, textColor,shapeColor);
		basePos.y += Settings::get().GetHighScoreItemHeight();
	}

	sf::Vector2f BBTexture = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	float x = BBTexture.x;
	sf::Vector2f(x / 2, BBTexture.y);
	basePos = BubbleMath::ToVector2f(m_Window->getSize());
	basePos.x = static_cast<float>(m_Window->getSize().x) / 10;
	basePos.y -= static_cast<float>(m_Window->getSize().y) / 10;
	m_HSBackButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	m_HSBackButton->SetText("Back");
	m_HSBackButton->ResizeCharacters(30);
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
	float y = m_Duck->GetSprite()->getGlobalBounds().height * 0.75f;
	float x = m_Duck->GetSprite()->getGlobalBounds().width * 0.75f;
	m_Duck->GetSprite()->setOrigin(x, y);
	float factorX = Settings::get().GetDuckWidth() / (static_cast<float>(m_DuckTexture->getSize().x) / 4);
	float factorY = Settings::get().GetDuckHeight() / static_cast<float>(m_DuckTexture->getSize().y);
	m_Duck->GetSprite()->setScale(factorX, factorY);

}

void Rendering::CreateNextUpSprites()
{
	m_NextUpBubbles = std::map<EBUBBLE_TYPE, std::unique_ptr<sf::Sprite>>();
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_STAR, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_STAR))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_CRAB, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_CRAB))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_FISH, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_FISH))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_FROG, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_FROG))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_SPIKY_BOMB, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_BATH_BOMB))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_BATH_BOMB, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_BATH_BOMB))));


	sf::Vector2f size = BubbleMath::ToVector2f(m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_STAR)->getSize());
	float factorX = Settings::get().GetNextUpWidth() / size.x;

	for (int i = 0; i < 4; i++)
	{
		auto& sprite = m_NextUpBubbles.at(static_cast<EBUBBLE_TYPE>(i));
		size = BubbleMath::ToVector2f(sprite->getTexture()->getSize());
		sprite->setScale(Settings::get().GetNextUpWidth() / size.x,  factorX);
		sprite->setPosition(m_CycleSprite->getGlobalBounds().left + 5,m_CycleSprite->getGlobalBounds().top + 10);
	}

	std::unique_ptr<sf::Sprite>& sprite = m_NextUpBubbles.at(EBUBBLE_TYPE::TYPE_SPIKY_BOMB);
	size = BubbleMath::ToVector2f(sprite->getTexture()->getSize());
	sprite->setScale(Settings::get().GetNextUpWidth() / size.x, factorX);
	sprite->setPosition(m_CycleSprite->getGlobalBounds().left + 5, m_CycleSprite->getGlobalBounds().top + 10);

	std::unique_ptr<sf::Sprite>& sprite2 = m_NextUpBubbles.at(EBUBBLE_TYPE::TYPE_BATH_BOMB);
	size = BubbleMath::ToVector2f(sprite2->getTexture()->getSize());
	sprite2->setScale(Settings::get().GetNextUpWidth() / size.x, factorX);
	sprite2->setPosition(m_CycleSprite->getGlobalBounds().left + 5, m_CycleSprite->getGlobalBounds().top + 10);
}

void Rendering::CreateHighScoreSpriteInPlay(sf::Vector2f& position)
{
	m_HighScoreTitleTextureInPlay = std::make_unique<sf::Texture>();
	m_HighScoreTitleTextureInPlay->loadFromFile(PLAY_MODE_HIGHSCORE_TITLE_FILENAME);

	m_HighScoreTitleInPlay = std::make_unique<sf::Sprite>();
	m_HighScoreTitleInPlay->setTexture(*m_HighScoreTitleTextureInPlay);
	m_HighScoreTitleInPlay->setOrigin(m_HighScoreTitleInPlay->getLocalBounds().width / 2.f, m_HighScoreTitleInPlay->getLocalBounds().height / 2.f);
	float scaleX = Settings::get().GetScale();
	m_HighScoreTitleInPlay->setScale(0.3f * scaleX, 0.3f * Settings::get().GetScale());

	position = m_ScoreTitle->getPosition();
	position.y += 250 *Settings::get().GetScale();
	m_HighScoreTitleInPlay->setPosition(position);

	m_HighScoreTextureInPlay = std::make_unique<sf::Texture>();
	m_HighScoreTextureInPlay->loadFromFile(PLAY_MODE_SCOREBOARD);

	m_HighScoreSpriteInPlay = std::make_unique<sf::Sprite>();
	m_HighScoreSpriteInPlay->setTexture(*m_HighScoreTextureInPlay);
	m_HighScoreSpriteInPlay->setOrigin(m_HighScoreSpriteInPlay->getLocalBounds().width / 2.f, m_HighScoreSpriteInPlay->getLocalBounds().height / 2.f);
	m_HighScoreSpriteInPlay->setScale(0.3f * Settings::get().GetScale(), 0.3f * Settings::get().GetScale());
	m_HighScoreSpriteInPlay->setPosition(m_HighScoreTitleInPlay->getPosition().x, m_HighScoreTitleInPlay->getPosition().y + m_HighScoreSpriteInPlay->getGlobalBounds().height / 2.f - 20);
}

void Rendering::CreatePlayScoreSprites()
{
	m_ScoreTitleTexture = std::make_unique<sf::Texture>();
	m_ScoreTitleTexture->loadFromFile(PLAY_MODE_SCORE_TITLE_FILENAME);

	m_ScoreTitle = std::make_unique<sf::Sprite>();
	m_ScoreTitle->setTexture(*m_ScoreTitleTexture);
	m_ScoreTitle->setScale(0.5f * Settings::get().GetScale(), 0.5f * Settings::get().GetScale());
	m_ScoreTitle->setOrigin(m_ScoreTitle->getLocalBounds().width / 2.f, m_ScoreTitle->getLocalBounds().height / 2.f);

	auto position = sf::Vector2f(m_Frame->getGlobalBounds().left, m_Frame->getGlobalBounds().top);
	position.y += m_Frame->getGlobalBounds().height / 6.f;
	position.x -= m_ScoreTitle->getGlobalBounds().width;
	m_ScoreTitle->setPosition(position);

	position.y += m_ScoreTitle->getGlobalBounds().height * 1.1f;
	m_Score->setPosition(position);


	CreateHighScoreSpriteInPlay(position);

	sf::Color textColor = sf::Color(225, 142, 149, 255);

	m_HighScoresInPlay = std::vector<std::unique_ptr<sf::Text>>(3);
	m_HighScoresInPlay[0] = std::make_unique<sf::Text>();

	m_HighScoresInPlay[0]->setString(std::to_string(rand()));
	position.x -= 78 * Settings::get().GetScale();
	position.y = m_HighScoreSpriteInPlay->getGlobalBounds().top + (m_HighScoreSpriteInPlay->getGlobalBounds().height / 2.f) - 38 * Settings::get().GetScale() / 2;
	m_HighScoresInPlay[0]->setPosition(position);
	m_HighScoresInPlay[0]->setFont(*m_Font);
	m_HighScoresInPlay[0]->setFillColor(textColor);
	m_HighScoresInPlay[0]->setOutlineColor(sf::Color::White);
	m_HighScoresInPlay[0]->setOutlineThickness(3);
	m_HighScoresInPlay[0]->setCharacterSize(38 * Settings::get().GetScale());

	m_HighScoresInPlay[1] = std::make_unique<sf::Text>();
	m_HighScoresInPlay[1]->setString(std::to_string(rand()));
	position.y += 38 * Settings::get().GetScale();
	m_HighScoresInPlay[1]->setPosition(position);
	m_HighScoresInPlay[1]->setFont(*m_Font);
	m_HighScoresInPlay[1]->setFillColor(textColor);
	m_HighScoresInPlay[1]->setOutlineColor(sf::Color::White);
	m_HighScoresInPlay[1]->setOutlineThickness(3);
	m_HighScoresInPlay[1]->setCharacterSize(38 * Settings::get().GetScale());


	m_HighScoresInPlay[2] = std::make_unique<sf::Text>();
	m_HighScoresInPlay[2]->setString(std::to_string(rand()));
	position.y += 40 * Settings::get().GetScale();
	m_HighScoresInPlay[2]->setPosition(position);
	m_HighScoresInPlay[2]->setFont(*m_Font);
	m_HighScoresInPlay[2]->setFillColor(textColor);
	m_HighScoresInPlay[2]->setOutlineColor(sf::Color::White);
	m_HighScoresInPlay[2]->setOutlineThickness(3);
	m_HighScoresInPlay[2]->setCharacterSize(38 * Settings::get().GetScale());

}

void Rendering::CreatePlayModeButtons()
{
	sf::Vector2f basePos;
	sf::Vector2f BBTextureSize = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	float buttonWidth = (Settings::get().GetButtonWidth() + 15.f) / (BBTextureSize.x / 3.f);
	basePos.x = m_Frame->getGlobalBounds().left - Settings::get().GetButtonWidth() * 2;
	basePos.y = m_Frame->getGlobalBounds().top + m_Frame->getGlobalBounds().height - Settings::get().GetButtonHeight() / 2;

	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("menu");
	newButton->ResizeCharacters(35 * Settings::get().GetScale());
	sf::Vector2f buttonScale = sf::Vector2f(buttonWidth, Settings::get().GetButtonHeight() / (BBTextureSize.y));

	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Back to menu", std::move(newButton)));

	basePos.x += (Settings::get().GetButtonWidth() + 15.f) * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("restart");
	newButton->ResizeCharacters(35 * Settings::get().GetScale());
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
	m_ConfirmationWindow->setOrigin(m_ConfirmationWindow->getSize().x / 2, m_ConfirmationWindow->getSize().y / 3);

	sf::Vector2f size = BubbleMath::ToVector2f(m_Window->getSize());
	m_ConfirmationWindow->setPosition(size.x/ 2, size.y / 2);

	sf::Vector2f BBTextureSize = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetButtonWidth() / (BBTextureSize.x / 4.f), Settings::get().GetButtonHeight() / BBTextureSize.y);
	sf::Vector2f basePos = m_ConfirmationWindow->getGlobalBounds().getPosition();
	basePos.x += m_ConfirmationWindow->getGlobalBounds().width / 4;
	basePos.y += m_ConfirmationWindow->getGlobalBounds().height / 1.3f;
	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Yes");
	newButton->ResizeCharacters(30 * Settings::get().GetScale());
	newButton->SetScale(sf::Vector2f(buttonScale));
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("ConfirmConfirm", std::move(newButton)));

	basePos.x += m_ConfirmationWindow->getGlobalBounds().width / 2;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("No");
	newButton->ResizeCharacters(30 * Settings::get().GetScale());
	newButton->SetScale(sf::Vector2f(buttonScale));
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("CancelConfirm", std::move(newButton)));

	m_ConfirmationText = std::make_unique<sf::Text>();
	m_ConfirmationText->setFont(*m_Font);
	m_ConfirmationText->setFillColor(sf::Color::Black);
	m_ConfirmationText->setString(CONFIRM_EXIT_GAME);
	m_ConfirmationText->setOrigin(m_ConfirmationText->getLocalBounds().getPosition().x + m_ConfirmationText->getLocalBounds().width / 2,
		m_ConfirmationText->getLocalBounds().getPosition().y + m_ConfirmationText->getLocalBounds().height / 2);
	m_ConfirmationText->setPosition(m_ConfirmationWindow->getPosition());


}

void Rendering::CreateSettingsButtons()
{
	sf::Vector2f position = m_MenuButtons.at("Back to menu")->GetPosition();
	std::unique_ptr<Button> newButton = std::make_unique<Button>(position, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Apply");
	newButton->ResizeCharacters(30 * Settings::get().GetScale());
	sf::Vector2f BBTextureSize = BubbleMath::ToVector2f(m_BaseButtonTexture->getSize());
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetMenuButtonWidth() / (BBTextureSize.x / 3.f), Settings::get().GetButtonHeight() / (BBTextureSize.y));

	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("ApplySettings", std::move(newButton)));

	position.x += m_MenuButtons.at("Back to menu")->GetWidth() * 1.75f;
	newButton = std::make_unique<Button>(position, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Revert");
	newButton->ResizeCharacters(30 * Settings::get().GetScale());
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Revert", std::move(newButton)));
}

void Rendering::CreateSettings()
{

	m_SettingSliders = std::vector<std::unique_ptr<Slider>>();
	m_SettingsText = std::vector<std::unique_ptr<sf::Text>>();
	m_SettingsTexture = std::make_unique<sf::Texture>();
	m_SettingsTexture->loadFromFile(SETTINGS_FILENAME);

	m_SettingsTitle = std::make_unique<sf::RectangleShape>();
	m_SettingsTitle->setTexture(m_SettingsTexture.get());
	sf::Vector2f titleSize = m_Title->getSize();
	sf::Vector2f vector2U = BubbleMath::ToVector2f(m_SettingsTexture->getSize());
	float scale = titleSize.y / vector2U.y;
	titleSize.x = vector2U.x * scale;
	m_SettingsTitle->setSize(titleSize);
	m_SettingsTitle->setOrigin(m_SettingsTitle->getSize().x / 2, m_SettingsTitle->getSize().y / 2);
	m_SettingsTitle->setPosition(m_Title->getPosition());

	sf::Vector2f position = sf::Vector2f(m_SettingsTitle->getPosition().x, m_MenuButtons.at("Play")->GetPosition().y);
	sf::Vector2f size = sf::Vector2f(titleSize.x / 2, titleSize.y / 10);
	sf::Color baseColor = sf::Color(42, 112, 145, 255);
	sf::Color hoverColor = sf::Color(188, 236, 244, 255);
	sf::Color clickedColor = sf::Color(209, 226, 231, 255);
	sf::Color sliderColor = sf::Color(128, 128, 128, 255);
	m_SettingSliders.push_back(std::make_unique<Slider>(position, size, baseColor, hoverColor, clickedColor, sliderColor));
	position.y += 100;
	m_SettingSliders.push_back(std::make_unique<Slider>(position, size, baseColor, hoverColor, clickedColor, sliderColor));

	m_SettingsText.push_back(std::make_unique<sf::Text>("Music volume:", *m_Font));
	sf::Vector2f origin = sf::Vector2f(0, static_cast<float>(m_SettingsText[0]->getCharacterSize()) / 1.5f);
	sf::Vector2f textPosition = sf::Vector2f(m_SettingsTitle->getPosition().x - m_SettingsTitle->getSize().x / 2, position.y - 100);

	m_SettingsText[0]->setOrigin(origin);
	m_SettingsText[0]->setPosition(textPosition);
	m_SettingsText[0]->setFillColor(sf::Color::Black);

	textPosition.y = position.y;
	m_SettingsText.push_back(std::make_unique<sf::Text>("Sound effects:", *m_Font));
	m_SettingsText[1]->setOrigin(origin);
	m_SettingsText[1]->setPosition(textPosition);
	m_SettingsText[1]->setFillColor(sf::Color::Black);

	position.y += 100;
	position.x += size.x / 2;
	m_CheckboxTexture = std::make_unique<sf::Texture>();
	m_CheckboxTexture->loadFromFile(CHECKBOX_FILENAME);
	textPosition.y = position.y;
	m_SettingsText.push_back(std::make_unique<sf::Text>("Fullscreen:", *m_Font));
	m_SettingsText[2]->setOrigin(origin);
	m_SettingsText[2]->setPosition(textPosition);
	m_SettingsText[2]->setFillColor(sf::Color::Black);

	m_FullscreenCheckbox = std::make_unique<Checkbox>(m_CheckboxTexture, position, 50);
	m_FullscreenCheckbox->SetEnableCheckbox(Settings::get().IsFullscreen());

	CreateSettingsButtons();

}

void Rendering::CreateTutorial()
{
	m_TutorialTextures = std::vector<std::unique_ptr<sf::Texture>>(2);
	m_TutorialTextures[0] = std::make_unique<sf::Texture>();
	m_TutorialTextures[0]->loadFromFile(TUTORIAL_ONE_FILENAME);
	m_TutorialTextures[1] = std::make_unique<sf::Texture>();
	m_TutorialTextures[1]->loadFromFile(TUTORIAL_TWO_FILENAME);

	m_TutorialSprites = std::vector<std::unique_ptr<sf::RectangleShape>>(2);
	m_TutorialSprites[0] = std::make_unique<sf::RectangleShape>();
	m_TutorialSprites[0]->setTexture(m_TutorialTextures[0].get());
}

void Rendering::CreateStorageSprites()
{
	m_StoredSprite = std::make_unique<sf::Sprite>();
	m_StoredSprite->setTexture(*m_StorageTextures.at(EBUBBLE_TYPE::TYPE_NULL));
	sf::Vector2f size = BubbleMath::ToVector2f(m_StoredSprite->getTexture()->getSize());
	m_StoredSprite->setScale((Settings::get().GetStorageBoxWidth() / size.x), Settings::get().GetStorageBoxHeight() / size.y);

	sf::FloatRect localBounds = m_StoredSprite->getLocalBounds();
	m_StoredSprite->setOrigin(localBounds.left + localBounds.width / 2.f,
	                          localBounds.top + localBounds.height / 2.f);

	m_StoredSprite->setPosition(m_Frame->getGlobalBounds().left + m_Frame->getGlobalBounds().width * 1.1f, m_Frame->getGlobalBounds().top + m_Frame->getGlobalBounds().height - m_StoredSprite->getGlobalBounds().height / 2.f);

	m_MovingStorageSprite = std::make_unique<sf::Sprite>();
	m_MovingStorageSprite->setTexture(*m_StorageTextures.at(EBUBBLE_TYPE::TYPE_NULL));

	localBounds = m_MovingStorageSprite->getLocalBounds();
	m_MovingStorageSprite->setOrigin(localBounds.left + localBounds.width / 2.f,
									localBounds.top + localBounds.height / 2.f);

	m_StorageText = std::make_unique<sf::Text>();
	m_StorageText->setFont(*m_Font);
	m_StorageText->setOutlineColor(sf::Color(192, 102, 71, 255));
	m_StorageText->setOutlineThickness(3 * Settings::get().GetScale());
	m_StorageText->setCharacterSize(35 * Settings::get().GetScale());
	m_StorageText->setFillColor(sf::Color::White);
	m_StorageText->setStyle(sf::Text::Bold);
	m_StorageText->setString("Storage:");

	localBounds = m_StorageText->getLocalBounds();
	m_StorageText->setOrigin(localBounds.left + localBounds.width / 2.f,
		localBounds.top);
	m_StorageText->setPosition(m_StoredSprite->getPosition().x, m_StoredSprite->getPosition().y - m_StoredSprite->getGlobalBounds().height * 0.5f);


	m_InstructionTexture = std::make_unique<sf::Texture>();
	m_InstructionTexture->loadFromFile(INSTRUCTION_FILENAME);
	m_InstructionShape = std::make_unique<sf::Sprite>();
	m_InstructionShape->setTexture(*m_InstructionTexture);
	m_InstructionShape->setOrigin(m_InstructionShape->getLocalBounds().width / 2.f, m_InstructionShape->getLocalBounds().height / 2.f);
	m_InstructionShape->setScale(sf::Vector2f(0.35f * Settings::get().GetScale(), 0.35f * Settings::get().GetScale()));

	float x = m_StoredSprite->getPosition().x + m_StoredSprite->getGlobalBounds().width / 1.3f + m_InstructionShape->getGlobalBounds().width / 2.f;
	m_InstructionShape->setPosition(x, m_StoredSprite->getPosition().y);
}

void Rendering::CreateCycleBottle()
{
	m_CycleSprite = std::make_unique<sf::RectangleShape>();
	m_CycleSprite->setTexture(m_CycleTexture.get());

	sf::Vector2f cycleTextureSize = BubbleMath::ToVector2f(m_CycleTexture->getSize());

	float temp = (m_Container->getSize().y * 0.95f) / cycleTextureSize.y;
	sf::Vector2f f(cycleTextureSize.x * temp, m_Container->getSize().y * 0.95f);
	m_CycleSprite->setSize(f);
	m_CycleSprite->setOrigin(m_CycleSprite->getLocalBounds().left + m_CycleSprite->getLocalBounds().width / 2.f,
		m_CycleSprite->getLocalBounds().top + m_CycleSprite->getLocalBounds().height / 2.f);

	sf::Vector2f position;
	position.y = m_Frame->getPosition().y - m_Frame->getGlobalBounds().height / 8.f;
	position.x = m_Frame->getGlobalBounds().left + m_Frame->getGlobalBounds().width;
	position.x += m_CycleSprite->getLocalBounds().width / 2.f;
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
		m_ScoreNumberSprites[i] = std::make_unique<sf::Sprite>();
		m_ScoreNumberSprites[i]->setTexture(*m_ScoreNumberTextures[i]);
		m_ScoreNumberSprites[i]->setScale(0.3f * Settings::get().GetScale(), 0.3f * Settings::get().GetScale());
		m_ScoreNumberSprites[i]->setPosition(m_ScoreTitle->getPosition().x, m_ScoreTitle->getPosition().y + m_ScoreTitle->getGlobalBounds().height / 2 + 10);
		
	}

	m_ScoreBackgroundInPlayTexture = std::make_unique<sf::Texture>();
	m_ScoreBackgroundInPlayTexture->loadFromFile(SCORE_CLOUD_FILENAME);
	m_ScoreBackgroundInPlaySprite = std::make_unique<sf::Sprite>();
	m_ScoreBackgroundInPlaySprite->setTexture(*m_ScoreBackgroundInPlayTexture);
	m_ScoreBackgroundInPlaySprite->setOrigin(m_ScoreBackgroundInPlaySprite->getLocalBounds().width / 2, m_ScoreBackgroundInPlaySprite->getLocalBounds().height / 2);
	m_ScoreBackgroundInPlaySprite->setScale(0.3f * Settings::get().GetScale(), 0.3f * Settings::get().GetScale());
	m_ScoreBackgroundInPlaySprite->setPosition(m_ScoreTitle->getPosition().x, m_ScoreTitle->getPosition().y + m_ScoreTitle->getGlobalBounds().height / 2 
											+ m_ScoreBackgroundInPlaySprite->getGlobalBounds().height / 4);
}
