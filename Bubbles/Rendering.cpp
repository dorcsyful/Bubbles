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
	}
	else
	{
		sf::Vector2u size = sf::Vector2u(static_cast<int>(Settings::get().GetWindowWidth()), static_cast<int>(Settings::get().GetWindowHeight()));
		m_Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(size.x, size.y), "Bubbles!",
			sf::Style::Default, context);

	}

	LoadBackground();
	LoadBubbleTextures();
	LoadNextUpTextures();
	CreatePointer();
	CreateMenuSprites();
	CreateHighScoreSprites();
	CreateScoreText();

	CreateGameOverSprite();

	CreateDuck();
	CreateNextUpSprites();
	CreatePlayModeButtons();
	CreateConfirmationWindow();
	CreateSettings();

}

void Rendering::PlayDraw() const
{
	m_Window->draw(*m_BackgroundSprite);

	sf::Vector2f mousePosition = m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window));
	m_MenuButtons.at("Back to menu")->DetectHover(mousePosition);
	m_MenuButtons.at("Restart")->DetectHover(mousePosition);

	m_Window->draw(*m_Container);
	m_Window->draw(*m_Frame);
	m_Duck->Draw(*m_Window);

	m_Window->draw(*m_Line);
	m_Window->draw(*m_NextUpBubbles.at(m_ActiveNextUp));
	m_PreviewBubbles.at(m_ActiveBubble)->Draw(*m_Window);
	for (auto& element : m_RenderedBubbles)
	{
		element->Draw(*m_Window);
	}

	m_MenuButtons.at("Back to menu")->Draw(*m_Window);
	m_MenuButtons.at("Restart")->Draw(*m_Window);
	m_Window->draw(*m_Score);
	m_Window->draw(*m_ComboText);
	m_Window->draw(*m_ScoreTitle);

}

void Rendering::MenuDraw() const
{
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
	m_MenuButtons.at("PlayAgain")->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	m_MenuButtons.at("BackToMenu")->DetectHover(m_Window->mapPixelToCoords(sf::Mouse::getPosition(*m_Window)));
	m_Window->draw(*m_GOScoreSprite);
	m_MenuButtons.at("BackToMenu")->Draw(*m_Window);
	m_MenuButtons.at("PlayAgain")->Draw(*m_Window);
	//m_Duck->Draw(*m_Window);
}

void Rendering::ConfirmationDraw() const
{
	sf::RectangleShape temp;
	temp.setSize(sf::Vector2f(m_Window->getSize().x, m_Window->getSize().y));
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

void Rendering::Draw(const EGAME_STATE a_State) const
{
	m_Window->clear();

	if (a_State == EGAME_STATE::STATE_PLAY)
	{
		PlayDraw();
	}
	if (a_State == EGAME_STATE::STATE_MENU) MenuDraw();
	if(a_State == EGAME_STATE::STATE_LOADING)
	{
		if(CallAfterDelay::getInstance().HasFunction("SetPlayState"))
		{
			m_Window->draw(*m_BackgroundSprite);
			m_BackgroundSprite->setFillColor(sf::Color::White);
			float remainingTime = CallAfterDelay::getInstance().GetRemainingTimeAsPercentage("SetPlayState");
			m_MainBackgroundSprite->setFillColor(sf::Color(255, 255, 255, remainingTime * 255.f));
			m_Window->draw(*m_MainBackgroundSprite);

		}
		else
		{
			m_Window->draw(*m_MainBackgroundSprite);
			m_MainBackgroundSprite->setFillColor(sf::Color::White);
			float remainingTime = CallAfterDelay::getInstance().GetRemainingTimeAsPercentage("SetMenuState");
			std::cout << remainingTime << "\n";
			m_BackgroundSprite->setFillColor(sf::Color(255, 255, 255, remainingTime * 255.f));
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
		PlayDraw();
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

void Rendering::CreateSprite(const EBUBBLE_TYPE a_Type, const sf::Vector2f& a_Position, const float a_Rotation, std::unique_ptr<AnimatedSprite>& a_NewSprite) const
{
	a_NewSprite = std::make_unique<AnimatedSprite>(m_BubbleTextures.at(a_Type).get(),Settings::get().GetBubbleAnimationTotalTime() , Settings::get().GetBubbleFrames());

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

void Rendering::UpdateHighScores(const std::vector<unsigned int>& a_Scores) const
{
	for(int i = 0; i < 10; i++)
	{
		m_HighScoreSprites[i]->SetText(std::to_string(a_Scores[i]));
	}
	m_Score->setString("0" + m_HighScoreSprites[0]->GetText());

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

void Rendering::LoadBubbleTextures()
{
	m_BubbleTextures = std::map<EBUBBLE_TYPE,std::unique_ptr<sf::Texture>>();
	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_STAR,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_STAR]->loadFromFile(STAR_FILENAME);
	
	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_CRAB, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_CRAB]->loadFromFile(CRAB_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_FISH,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_FISH]->loadFromFile(FISH_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_FROG,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_FROG]->loadFromFile(FROG_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_JELLY,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_JELLY]->loadFromFile(JELLY_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_KRILL,std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_KRILL]->loadFromFile(KRILL_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_SEAL, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_SEAL]->loadFromFile(SEAL_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_SHARK, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_SHARK]->loadFromFile(SHARK_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_SQUID, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_SQUID]->loadFromFile(SQUID_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_WHALE, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_WHALE]->loadFromFile(WHALE_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_BATH_BOMB, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_BATH_BOMB]->loadFromFile(BATH_BOMB_FILENAME);

	m_BubbleTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_SPIKY_BOMB, std::make_unique<sf::Texture>()));
	m_BubbleTextures[EBUBBLE_TYPE::TYPE_SPIKY_BOMB]->loadFromFile(SPIKY_BOMB_FILENAME);
}

void Rendering::LoadNextUpTextures()
{
	m_NextUpTextures = std::map<EBUBBLE_TYPE, std::unique_ptr<sf::Texture>>();
	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_STAR, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_STAR]->loadFromFile(STAR_NEXT_FILENAME);

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_CRAB, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_CRAB]->loadFromFile(CRAB_NEXT_FILENAME);

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_FISH, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_FISH]->loadFromFile(FISH_NEXT_FILENAME);

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_FROG, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_FROG]->loadFromFile(FROG_NEXT_FILENAME);

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_JELLY, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_JELLY]->loadFromFile(JELLY_NEXT_FILENAME);

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_KRILL, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_KRILL]->loadFromFile(KRILL_NEXT_FILENAME);

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_SEAL, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_SEAL]->loadFromFile(SEAL_NEXT_FILENAME);

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_SHARK, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_SHARK]->loadFromFile(SHARK_NEXT_FILENAME);

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_SQUID, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_SQUID]->loadFromFile(SQUID_NEXT_FILENAME);

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_WHALE, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_WHALE]->loadFromFile(WHALE_NEXT_FILENAME);

	m_NextUpTextures.insert(std::pair(EBUBBLE_TYPE::TYPE_BATH_BOMB, std::make_unique<sf::Texture>()));
	m_NextUpTextures[EBUBBLE_TYPE::TYPE_BATH_BOMB]->loadFromFile("Assets/Shapes/NextUp/special.png");
}

void Rendering::Reset()
{
	m_ActiveBubble = EBUBBLE_TYPE::TYPE_STAR;
}

void Rendering::LoadBackground()
{
	//Game Background
	m_BackgroundTexture = std::make_unique<sf::Texture>();
	m_BackgroundTexture->loadFromFile(GAME_BACKGROUND_FILENAME);
	m_BackgroundTexture->setRepeated(true);
	m_BackgroundSprite = std::make_unique<sf::RectangleShape>();
	m_BackgroundSprite->setTexture(m_BackgroundTexture.get());
	sf::Vector2f windowSize = sf::Vector2f(static_cast<float>(m_Window->getSize().x),static_cast<float>(m_Window->getSize().y));
	m_BackgroundSprite->setSize(windowSize);

	//Main Background
	m_MainBackgroundTexture = std::make_unique<sf::Texture>();
	m_MainBackgroundTexture->loadFromFile(MENU_BACKGROUND_FILENAME);
	m_MainBackgroundTexture->setRepeated(true);
	m_MainBackgroundSprite = std::make_unique<sf::RectangleShape>();
	m_MainBackgroundSprite->setTexture(m_MainBackgroundTexture.get());
	// Calculate scaling factors and set sprite size and origin
	float verticalScale = (float)windowSize.y / m_MainBackgroundTexture->getSize().y;
	float horizontalScale = verticalScale;
	m_MainBackgroundSprite->setSize(BubbleMath::ToVector2f(m_MainBackgroundTexture->getSize()));
	m_MainBackgroundSprite->setScale(horizontalScale, verticalScale);
	m_MainBackgroundSprite->setOrigin(m_MainBackgroundSprite->getSize().x / 2.f, m_MainBackgroundSprite->getSize().y / 2.f);
	m_MainBackgroundSprite->setPosition(windowSize.x / 2, windowSize.y / 2);

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
	sf::Vector2f basePos = sf::Vector2f((windowSize.x / 2.f) - (width / 2.f), ((windowSize.y - height) / 2.f));
	m_Container->setPosition(basePos);

	m_FrameTexture = std::make_unique<sf::Texture>();
	m_FrameTexture->loadFromFile(FRAME_FILENAME);
	m_Frame = std::make_unique<sf::RectangleShape>();
	m_Frame->setTexture(m_FrameTexture.get());
	m_Frame->setSize(sf::Vector2f(Settings::get().GetFrameWidth(), Settings::get().GetFrameHeight()));
	m_Frame->setOrigin(m_Frame->getSize().x / 2.f, m_Frame->getSize().y / 2.f);
	sf::Vector2f position = m_Container->getGlobalBounds().getPosition();
	sf::Vector2f size = m_Container->getGlobalBounds().getSize();
	m_Frame->setPosition(position.x + size.x / 2.f, position.y + size.y / 2.f -15);

}

void Rendering::CreatePointer()
{
	float containerHeight = Settings::get().GetContainerHeight();
	sf::Vector2f position = sf::Vector2f((static_cast<float>(m_Window->getSize().x) / 2.f) - (Settings::get().GetContainerWidth() / 2.f), ((static_cast<float>(m_Window->getSize().y) - containerHeight) / 2.f));
	m_Line = std::make_unique<sf::RectangleShape>(sf::Vector2f(5 / 2.f, containerHeight - 5));
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
	auto vector2U = m_GameOverTexture->getSize();
	float scale = titleSize.y / vector2U.y;
	titleSize.x = vector2U.x * scale;
	m_GameOver->setSize(titleSize);

	sf::Vector2f basePos = BubbleMath::ToVector2f(m_GameOverTexture->getSize());
	m_GameOver->setOrigin(titleSize.x / 2, titleSize.y / 2);

	basePos = BubbleMath::ToVector2f(m_Window->getSize());
	m_GameOver->setPosition(sf::Vector2f(m_Title->getPosition()));

	basePos = m_GameOver->getPosition();
	basePos.y += m_GameOver->getSize().y;
	auto size = m_GameOver->getSize();
	size.x /= 5;
	m_GOScoreSprite = std::make_unique<SpriteWithText>(m_Score->getString(), *m_Font, size,basePos,sf::Color::Black,m_ScoreBackgroundTexture.get());

	basePos.y += size.y * 1.1f;
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetMenuButtonWidth() / (m_BaseButtonTexture->getSize().x / 3.7f), Settings::get().GetMenuButtonHeight() / m_BaseButtonTexture->getSize().y);

	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Play again");
	newButton->ResizeCharacters(45);
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("PlayAgain", std::move(newButton)));

	basePos.y += Settings::get().GetMenuButtonHeight();
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Back to menu");
	newButton->ResizeCharacters(45);
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
	m_Font->loadFromFile(FONT_FILENAME);

	sf::Vector2f basePos = sf::Vector2f(m_Title->getPosition());
	basePos.y += m_Title->getSize().y + 10;
	basePos.x -= m_Title->getSize().x / 4;
	m_BaseButtonTexture = std::make_unique<sf::Texture>();
	m_BaseButtonTexture->loadFromFile(BUTTON_FILENAME);
	m_MenuButtons = std::map<std::string, std::unique_ptr<Button>>();
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetMenuButtonWidth() / (m_BaseButtonTexture->getSize().x / 3.f), Settings::get().GetMenuButtonHeight() / m_BaseButtonTexture->getSize().y);

	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos,*m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Play");
	newButton->ResizeCharacters(38);
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(),std::pair<std::string, std::unique_ptr<Button>>("Play", std::move(newButton)));

	basePos.y += Settings::get().GetMenuButtonHeight() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("High Scores");
	newButton->ResizeCharacters(38);
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("High_Score", std::move(newButton)));

	basePos.y += Settings::get().GetMenuButtonHeight() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("How to play");
	newButton->ResizeCharacters(38);
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("How to play", std::move(newButton)));

	basePos.x += m_Title->getSize().x / 2;
	basePos.y = m_Title->getPosition().y + m_Title->getSize().y + 10;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Settings");
	newButton->ResizeCharacters(38);
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Settings", std::move(newButton)));

	basePos.y += Settings::get().GetMenuButtonHeight() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Exit");
	newButton->ResizeCharacters(38);
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
	position.x -= m_Frame->getSize().x / 2 + 150;
	m_ScoreBackgroundTexture = std::make_unique<sf::Texture>();
	m_ScoreBackgroundTexture->loadFromFile(SCORE_FILENAME);

	m_Score = std::make_unique<sf::Text>();
	m_Score->setFont(*m_Font);
	m_Score->setFillColor(sf::Color::Black);
	m_Score->setCharacterSize(70);
	m_Score->setString(" 0 " + m_HighScoreSprites[0]->GetText());
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
	auto vector2U = m_HighScoreTexture->getSize();
	float scale = titleSize.y / vector2U.y;
	titleSize.x = vector2U.x * scale;
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

	float x = static_cast<float>(m_BaseButtonTexture->getSize().x);
	sf::Vector2f buttonTextureSize = sf::Vector2f(x / 2, static_cast<float>(m_BaseButtonTexture->getSize().y));
	basePos = BubbleMath::ToVector2f(m_Window->getSize());
	basePos.x = static_cast<float>(m_Window->getSize().x) / 10;
	basePos.y -= m_Window->getSize().y / 10;
	m_HSBackButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	m_HSBackButton->SetText("Back");
	m_HSBackButton->ResizeCharacters(40);
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetButtonWidth() / (m_BaseButtonTexture->getSize().x / 3.5f), Settings::get().GetButtonHeight() / m_BaseButtonTexture->getSize().y);

	m_HSBackButton->SetScale(buttonScale);
}

void Rendering::CreateDuck()
{
	m_DuckTexture = std::make_unique<sf::Texture>();
	m_DuckTexture->loadFromFile(DUCK_FILENAME);

	m_Duck = std::make_unique<AnimatedSprite>(m_DuckTexture.get(), 1, 4, true, true);
	m_Duck->SetFrame(0);
	m_Duck->UpdateFrameToAnimate(2);
	float factorX = Settings::get().GetDuckWidth() / (static_cast<float>(m_DuckTexture->getSize().x) / 4);
	float factorY = Settings::get().GetDuckHeight() / static_cast<float>(m_DuckTexture->getSize().y);
	m_Duck->GetSprite()->setScale(factorX, factorY);
	float x = Settings::get().GetDuckWidth() * 4.f;
	float y = Settings::get().GetDuckHeight() * 4.f;
	m_Duck->GetSprite()->setOrigin(x, y);
}

void Rendering::CreateNextUpSprites()
{
	m_NextUpBubbles = std::map<EBUBBLE_TYPE, std::unique_ptr<sf::Sprite>>();
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_STAR, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_STAR))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_CRAB, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_CRAB))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_FISH, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_FISH))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_FROG, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_FROG))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_JELLY, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_JELLY))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_KRILL, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_KRILL))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_SEAL, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_SEAL))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_SHARK, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_SHARK))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_SQUID, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_SQUID))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_WHALE, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_WHALE))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_BATH_BOMB, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_BATH_BOMB))));
	m_NextUpBubbles.insert(std::pair(EBUBBLE_TYPE::TYPE_SPIKY_BOMB, std::make_unique<sf::Sprite>(*m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_BATH_BOMB))));

	sf::Vector2f size = BubbleMath::ToVector2f(m_NextUpTextures.at(EBUBBLE_TYPE::TYPE_STAR)->getSize());
	float factorX = Settings::get().GetNextUpWidth() / size.x;
	float factorY = Settings::get().GetNextUpHeight() / size.y;

	for (auto& val : m_NextUpBubbles | std::views::values)
	{
		val->setScale(factorX, factorY);
		val->setPosition(m_Container->getSize().x * 1.1f + m_Container->getPosition().x, m_Container->getPosition().y + m_Container->getSize().y / 2.f);
	}
}

void Rendering::CreatePlayModeButtons()
{
	sf::Vector2f basePos;
	float buttonWidth = Settings::get().GetMenuButtonWidth() / (m_BaseButtonTexture->getSize().x / 3.f);
	basePos.x = m_Frame->getGlobalBounds().left - Settings::get().GetButtonWidth();
	basePos.y = m_Frame->getGlobalBounds().top + m_Frame->getGlobalBounds().height + Settings::get().GetButtonHeight() / 2;
	if(Settings::get().IsFullscreen())
	{
		float height = static_cast<float>(sf::VideoMode::getDesktopMode().height);
		float width = static_cast<float>(sf::VideoMode::getDesktopMode().width);
		if(height > Settings::get().GetWindowHeight())
		{
			basePos.y = height - Settings::get().GetButtonHeight() * 1.5f;
		}
		basePos.x = Settings::get().GetButtonWidth() * 1.2f;

	}

	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Menu");
	newButton->ResizeCharacters(40);
	sf::Vector2f buttonScale = sf::Vector2f(buttonWidth, Settings::get().GetButtonHeight() / (m_BaseButtonTexture->getSize().y));

	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Back to menu", std::move(newButton)));

	basePos.x += Settings::get().GetMenuButtonWidth() * 1.1f;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Restart");
	newButton->ResizeCharacters(40);
	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("Restart", std::move(newButton)));

	m_ScoreTitleTexture = std::make_unique<sf::Texture>();
	m_ScoreTitleTexture->loadFromFile(SCORE_TITLE_FILENAME);

	m_ScoreTitle = std::make_unique<sf::RectangleShape>();
	m_ScoreTitle->setTexture(m_ScoreTitleTexture.get());
	float i = static_cast<float>(m_ScoreTitleTexture->getSize().y) / static_cast<float>(m_ScoreTitleTexture->getSize().x);
	float y = Settings::get().GetScoreTitleWidth() * i;
	m_ScoreTitle->setSize(sf::Vector2f(Settings::get().GetScoreTitleWidth(), y));

	auto position = m_Score->getPosition();
	//position.x = m_Container->getGlobalBounds().left - m_Score->getGlobalBounds().width;
	//position.y -= m_Score->getGlobalBounds().height;
	m_ScoreTitle->setPosition(position);
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

	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetButtonWidth() / (m_BaseButtonTexture->getSize().x / 4.f), Settings::get().GetButtonHeight() / m_BaseButtonTexture->getSize().y);
	sf::Vector2f basePos = m_ConfirmationWindow->getGlobalBounds().getPosition();
	basePos.x += m_ConfirmationWindow->getGlobalBounds().width / 4;
	basePos.y += m_ConfirmationWindow->getGlobalBounds().height / 1.3f;
	std::unique_ptr<Button> newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Yes");
	newButton->ResizeCharacters(40);
	newButton->SetScale(sf::Vector2f(buttonScale));
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("ConfirmConfirm", std::move(newButton)));

	basePos.x += m_ConfirmationWindow->getGlobalBounds().width / 2;
	newButton = std::make_unique<Button>(basePos, *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("No");
	newButton->ResizeCharacters(40);
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
	std::unique_ptr<Button> newButton = std::make_unique<Button>(m_MenuButtons.at("Back to menu")->GetPosition(), *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Apply");
	newButton->ResizeCharacters(40);
	sf::Vector2f buttonScale = sf::Vector2f(Settings::get().GetMenuButtonWidth() / (m_BaseButtonTexture->getSize().x / 3.f), Settings::get().GetButtonHeight() / (m_BaseButtonTexture->getSize().y));

	newButton->SetScale(buttonScale);
	m_MenuButtons.insert(m_MenuButtons.begin(), std::pair<std::string, std::unique_ptr<Button>>("ApplySettings", std::move(newButton)));

	newButton = std::make_unique<Button>(m_MenuButtons.at("Restart")->GetPosition(), *m_Font, m_BaseButtonTexture.get());
	newButton->SetText("Revert");
	newButton->ResizeCharacters(40);
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
	auto vector2U = m_SettingsTexture->getSize();
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

