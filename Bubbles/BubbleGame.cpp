#include "BubbleGame.h"

#include <SFML/Window/Event.hpp>

#include "Helpers.h"
#include <chrono>

#include "Audio.h"

void BubbleGame::Initialize()
{
	CallAfterDelay::getInstance().ClearQueue();
	m_State = EGAME_STATE::STATE_MENU;
	m_Wrapper = std::make_unique<BubbleWrapper>();
	Audio::getInstance().SetMusicVolume(Settings::get().GetMusicVolume());
	Audio::getInstance().SetEffectsVolume(Settings::get().GetSoundEffectsVolume());

	m_Rendering = std::make_unique<Rendering>(Settings::get().GetWindowWidth(), Settings::get().GetWindowHeight(), m_Wrapper->GetRendered());
	m_Gameplay = std::make_unique<Gameplay>(m_Rendering->GetWindow()->getSize().x);
	m_Physics = std::make_unique<Physics>(m_Wrapper->GetGameObjects(), m_Rendering->GetWindow()->getSize().x, m_Rendering->GetWindow()->getSize().y);
	m_Save = std::make_unique<Save>();

	m_Physics->CreateContainerLines();

	m_Rendering->UpdateHighScores(m_Save->GetScores());
	m_Rendering->UpdateNextUp(m_Gameplay->GetNextBubble());
	m_Rendering->GetSettingSlider(0)->SetSliderValue(Settings::get().GetMusicVolume());
	m_Rendering->GetSettingSlider(1)->SetSliderValue(Settings::get().GetSoundEffectsVolume());
}

void BubbleGame::PlayUpdate(float a_Delta)
{
	m_Gameplay->Update(a_Delta);
	m_Physics->Update(a_Delta);

	for (size_t i = 0; i < m_Physics->m_BubblesToCombine.size(); i++)
	{
		const BubbleObject* bubble1 = m_Physics->m_BubblesToCombine[i].first;
		auto bubble2 = m_Physics->m_BubblesToCombine[i].second;

		m_ComboTextPositions = BubbleMath::Lerp(bubble1->GetPosition(),bubble2->GetPosition(), 0.5f);
		m_ComboTextPositions.x *= Settings::get().GetPixelToMeter();
		m_ComboTextPositions.y *= Settings::get().GetPixelToMeter();
		m_ComboTextPositions.y *= -1;
		m_Rendering->UpdateComboPosition(m_ComboTextPositions);

		m_Rendering->UpScaleComboText(0.5);
		m_ScaleTimer = sf::Clock();

		auto combined = m_Gameplay->CombineBubble(bubble1, bubble2);
		unsigned combo = Settings::get().GetComboScore() * m_Gameplay->GetComboScore();
		m_Rendering->UpdateCombo(combo);

		m_Wrapper->RemoveObjectByPointer(bubble1);
		m_Wrapper->RemoveObjectByPointer(bubble2);
		CreateWrapper(combined);

	}

	m_Wrapper->Update();
	NextComboText();
	m_Rendering->UpdateScore(m_Gameplay->GetScore());
	m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
	m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());
	if (m_Physics->GetTouchedTopLine()) { GameOver();}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) { GameOver(); }

}

void BubbleGame::RestartGame()
{
	m_Gameplay->Reset(m_Rendering->GetWindow()->getSize().x);
	m_Physics->Reset();
	m_Rendering->Reset();
	m_Wrapper->Clear();
	m_Rendering->GetDuck()->SetFrame(0);
	CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_LOADING; }, "SetLoadState", 0.1f, false);
	CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_PLAY; }, "SetPlayState", Settings::get().GetLoadTime(), false);
}

void BubbleGame::Update()
{
	sf::Clock dtClock;
	while(m_Rendering->GetWindow()->isOpen())
	{
		if(m_State == EGAME_STATE::STATE_START)
		{
			Initialize();
		}
		float delta = dtClock.restart().asSeconds();

		sf::Event event;
		while (m_Rendering->GetWindow()->pollEvent(event))
		{

			if (event.type == sf::Event::Closed)
				m_Rendering->GetWindow()->close();

			if (m_State == EGAME_STATE::STATE_PLAY )
			{
				PlayInput(event);
			}
			else if(m_State == EGAME_STATE::STATE_MENU)
			{
				MenuInput();
			}
			else if(m_State == EGAME_STATE::STATE_GAME_OVER_ANIMATION)
			{
				GameOverAnimationInput();
			}
			else if(m_State == EGAME_STATE::STATE_GAME_OVER)
			{
				GameOverInput();
			}
			else if(m_State == EGAME_STATE::STATE_HIGH_SCORE)
			{
				sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
				if(m_Rendering->GetHSBackButton()->DetectClick(mousePosition))
				{
					CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_MENU; },"SetMenuState", 0.2f, false);
				}
			}
			else if(m_State == EGAME_STATE::STATE_MENU_CONFIRM || m_State == EGAME_STATE::STATE_RESTART_CONFIRM
				|| m_State == EGAME_STATE::STATE_EXIT_CONFIRM || m_State == EGAME_STATE::STATE_SETTINGS_CONFIRM)
			{
				ConfirmInput();
			}
			else if(m_State == EGAME_STATE::STATE_SETTINGS)
			{
				SettingsInput();
			}
			
		}

		if (m_State == EGAME_STATE::STATE_PLAY)
		{
			PlayUpdate(delta);
		}

		CallAfterDelay::getInstance().LoopThroughFunctions();
		m_Rendering->Draw(m_State);
	}
}

void BubbleGame::CreateWrapper(std::unique_ptr<BubbleObject>& a_NewBubble) const
{
	sf::Vector2f temp = a_NewBubble->GetPosition();
	temp.x *= Settings::get().GetPixelToMeter();
	temp.y *= Settings::get().GetPixelToMeter();
	temp.y *= -1.f;
	std::unique_ptr<AnimatedSprite> newRendered;
	m_Rendering->CreateSprite(a_NewBubble->GetBubbleType(), temp, 0, newRendered);
	m_Wrapper->AddObject(std::move(a_NewBubble), std::move(newRendered));
}

void BubbleGame::AddBubble() const
{
	Audio::getInstance().PlayBubbleDrop();
	m_Rendering->GetDuck()->SetAnimate(false, false);
	m_Rendering->GetDuck()->SetFrame(2);
	CallAfterDelay::getInstance().AddFunction([this] { m_Rendering->GetDuck()->SetAnimate(true, true); }, "EnableDuckAnimate", 0.5f, false);
	m_Gameplay->SetLastDrop(std::chrono::system_clock::now());

	sf::Vector2f start = m_Rendering->GetPreviewPosition();
	start.x /= Settings::get().GetPixelToMeter();
	start.y = m_Physics->GetTopLineHeight();
	std::unique_ptr<BubbleObject> newBubble = m_Gameplay->Drop(start);
	CreateWrapper(newBubble);
	m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
	m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());
}

void BubbleGame::GameOver()
{
	m_State = EGAME_STATE::STATE_GAME_OVER_ANIMATION;
	m_Save->SaveIfHighScore(m_Gameplay->GetScore());
	Audio::getInstance().PlaySadGameOver();
	float delay = Settings::get().GetBubbleAnimationTotalTime() / 2.f;
	m_Rendering->GetDuck()->SetAnimate(false, false);
	m_Rendering->GetDuck()->SetFrame(3);
	CallAfterDelay::getInstance().RemoveFunction("EnableDuckAnimate");
	CallAfterDelay::getInstance().AddFunction([this](){ m_State = EGAME_STATE::STATE_GAME_OVER; CallAfterDelay::getInstance().RemoveFunction("RemoveBubbles"); }, "SetGameOverState", 
																delay * static_cast<float>(m_Wrapper->GetNumOfObjects()), false);
	CallAfterDelay::getInstance().AddFunction([this](){ RemoveAtEnd(); }, "RemoveBubbles" , delay, true);
	CallAfterDelay::getInstance().AddFunction([this]() { Audio::getInstance().PlayBackgroundMusic(); }, "RestartBackgroundMusic", 4.f, false);

}

void BubbleGame::RemoveAtEnd()
{
	for(size_t i = 0; i < m_Wrapper->GetNumOfObjects(); i++)
	{
		AnimatedSprite* shapeByIndex = m_Wrapper->GetShapeByIndex(i);
		if(!shapeByIndex->IsAnimated())
		{
			shapeByIndex->SetAnimate(true, false);
			return;
		}
	}

	CallAfterDelay::getInstance().RemoveFunction("SetGameOverState");
	CallAfterDelay::getInstance().RemoveFunction("RemoveBubbles");
	m_State = EGAME_STATE::STATE_GAME_OVER;
}

void BubbleGame::BackToMenu()
{
	m_Rendering->Reset();
		
	m_Gameplay->Reset(m_Rendering->GetWindow()->getSize().x);
	m_Physics->Reset();
	m_Wrapper->Clear();

	CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_LOADING; }, "SetLoadingState", 0.1f, false);
	CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_MENU;  }, "SetMenuState", Settings::get().GetLoadTime(), false);
}

void BubbleGame::NextComboText()
{
	if (m_ComboTextPositions == sf::Vector2f(INFINITY, INFINITY))
	{
		m_Rendering->UpdateCombo(0);
		m_Rendering->UpScaleComboText(0.5);

		return;
	}

	float initialScale = 0.5f;
	float targetScale = 1.0f;

	float elapsedTime = m_ScaleTimer.getElapsedTime().asSeconds();
	float scaleFactor = std::clamp(initialScale + (targetScale - initialScale) * (elapsedTime / 1),0.5f,1.f);
	m_Rendering->UpScaleComboText(scaleFactor);
	if (elapsedTime >= 1)
	{
		m_ScaleTimer = sf::Clock();
		m_ComboTextPositions = sf::Vector2f(INFINITY, INFINITY);
		m_Rendering->UpdateCombo(0);
	}
}

void BubbleGame::PlayInput(const sf::Event& a_Event)
{
	if(a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Space)
	{
		if (std::chrono::duration<float> elapsedSeconds = std::chrono::system_clock::now() - m_Gameplay->GetLastDrop(); elapsedSeconds.count() > 1)
		{
			m_ComboTextPositions = sf::Vector2f(INFINITY, INFINITY);
			AddBubble();
			m_Rendering->UpdateNextUp(m_Gameplay->GetNextBubble());
		}
	}

	sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
	std::map<std::string, std::unique_ptr<Button>>& buttons = m_Rendering->GetMenuButtons();
	if (buttons.at("Back to menu")->DetectClick(mousePosition))
	{
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_MENU_CONFIRM; m_Rendering->UpdateConfirmText(EGAME_STATE::STATE_MENU_CONFIRM); },
			"SetConfirmState1", 0.1f, false);
		//BackToMenu();
	}
	if(buttons.at("Restart")->DetectClick(mousePosition))
	{
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_RESTART_CONFIRM; m_Rendering->UpdateConfirmText(EGAME_STATE::STATE_RESTART_CONFIRM); },
			"SetConfirmState2", 0.1f, false);

		//RestartGame();
	}

	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Num0) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_STAR);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Num1) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_CRAB);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Num2) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_FISH);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Num3) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_FROG);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Num4) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_JELLY);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Num5) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_KRILL);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Num6) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_SEAL);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Num7) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_SHARK);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Num8) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_SQUID);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Num9) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_WHALE);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::Q) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_BATH_BOMB);
	if (a_Event.type == sf::Event::KeyPressed && a_Event.key.scancode == sf::Keyboard::Scan::W) m_Gameplay->CheatNextBubble(EBUBBLE_TYPE::TYPE_SPIKY_BOMB);
}

void BubbleGame::MenuInput()
{
	sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
	std::map<std::string, std::unique_ptr<Button>>& buttons = m_Rendering->GetMenuButtons();
	if (buttons.at("Play")->DetectClick(mousePosition))
	{
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_LOADING; }, "SetLoadingState", 0.1f, false);
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_PLAY; }, "SetPlayState", Settings::get().GetLoadTime(), false);
	}
	if (buttons.at("High_Score")->DetectClick(mousePosition))
	{
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_HIGH_SCORE; }, "SetHighScoreState", 0.1f, false);
	}

	if (buttons.at("Exit")->DetectClick(mousePosition))
	{
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_EXIT_CONFIRM; }, "Exit", 0.1f, false);
	}
	if (buttons.at("Settings")->DetectClick(mousePosition))
	{
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_SETTINGS; }, "SetSettingState", 0.1f, false);
	}
}

void BubbleGame::GameOverAnimationInput()
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		m_State = EGAME_STATE::STATE_GAME_OVER;
	}
}

void BubbleGame::GameOverInput()
{
	sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
	if (m_Rendering->GetMenuButtons().at("PlayAgain")->DetectClick(mousePosition))
	{
		RestartGame();
	}
	if(m_Rendering->GetMenuButtons().at("BackToMenu")->DetectClick(mousePosition))
	{
		m_Gameplay->Reset(m_Rendering->GetWindow()->getSize().x);
		m_Physics->Reset();
		m_Rendering->Reset();
		m_Wrapper->Clear();

		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_MENU; }, "BackToMenu", 0.2f, false);

	}
}

void BubbleGame::ConfirmInput()
{
	sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
	if (m_Rendering->GetMenuButtons().at("ConfirmConfirm")->DetectClick(mousePosition))
	{
		if (m_State == EGAME_STATE::STATE_EXIT_CONFIRM)
		{
			CallAfterDelay::getInstance().AddFunction([this]() { m_Rendering->GetWindow()->close(); }, "Exit", 0.2f, false);
		}
		if (m_State == EGAME_STATE::STATE_RESTART_CONFIRM)
		{
			CallAfterDelay::getInstance().AddFunction([this]() { RestartGame(); }, "Restart", 0.2f, false);
		}
		if (m_State == EGAME_STATE::STATE_MENU_CONFIRM)
		{
			CallAfterDelay::getInstance().AddFunction([this]() { BackToMenu(); }, "BackToMenu", 0.2f, false);
		}
		if (m_State == EGAME_STATE::STATE_SETTINGS_CONFIRM)
		{
			CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_START; }, "RestartGame", 0.2f, false);
		}
	}
	if (m_Rendering->GetMenuButtons().at("CancelConfirm")->DetectClick(mousePosition))
	{
		if (m_State == EGAME_STATE::STATE_EXIT_CONFIRM)
		{
			CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_MENU; }, "BackToMenu", 0.2f, false);
		}
		if (m_State == EGAME_STATE::STATE_RESTART_CONFIRM)
		{
			CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_PLAY; }, "BackToPlay", 0.2f, false);
		}
		if (m_State == EGAME_STATE::STATE_MENU_CONFIRM)
		{
			CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_PLAY; }, "BackToPlay", 0.2f, false);
		}
		if(m_State == EGAME_STATE::STATE_SETTINGS_CONFIRM)
		{
			CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_SETTINGS; }, "BackToSettings", 0.2f, false);
		}
	}
}

void BubbleGame::SettingsInput()
{
	sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));

	if(m_Rendering->GetSettingSlider(0)->DetectClick(mousePosition))
	{
		Audio::getInstance().SetMusicVolume(m_Rendering->GetSettingSlider(0)->GetSliderValue());
	}
	if (m_Rendering->GetSettingSlider(1)->DetectClick(mousePosition))
	{
		Audio::getInstance().SetEffectsVolume(m_Rendering->GetSettingSlider(1)->GetSliderValue());
	}
	if (m_Rendering->GetMenuButtons().at("ApplySettings")->DetectClick(mousePosition))
	{
		Settings::get().SetSoundEnabled(m_Rendering->GetSettingSlider(0)->GetSliderValue(), m_Rendering->GetSettingSlider(1)->GetSliderValue());
		m_Save->UpdateSettings(m_Rendering->GetFullscreenCheckbox()->IsChecked());

		if(m_Rendering->GetFullscreenCheckbox()->IsChecked() != Settings::get().IsFullscreen())
		{
			Settings::get().SetFullscreen(m_Rendering->GetFullscreenCheckbox()->IsChecked());
			m_Rendering->UpdateConfirmText(EGAME_STATE::STATE_SETTINGS_CONFIRM);
			CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_SETTINGS_CONFIRM; }, "ApplySettings", 0.1f, false);
		}
		else
			CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_MENU; },"ApplySettings", 0.1f, false);
	}
	if (m_Rendering->GetMenuButtons().at("Revert")->DetectClick(mousePosition))
	{
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_MENU; }, "RevertSettings", 0.1f, false);
	}

	m_Rendering->GetFullscreenCheckbox()->DetectClick(mousePosition);
}
