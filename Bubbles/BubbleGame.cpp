#define _CRT_SECURE_NO_WARNINGS
#include "steam_api.h"

#include <windows.h>
#include "BubbleGame.h"

#include <SFML/Window/Event.hpp>
#include "Helpers.h"
#include <chrono>

#include "Audio.h"



BubbleGame::BubbleGame()
{
	if (!SteamAPI_Init()) {
		std::cerr << "SteamAPI_Init() failed. Steam must be running.\n";
	}

	std::cout << "Steam API initialized for user: "
		<< SteamFriends()->GetPersonaName() << "\n";
	m_IsMouseButtonPressed = false;
	Random::getInstance().SetSeed(time(nullptr));
	CallAfterDelay::getInstance().ClearQueue();
	m_State = EGAME_STATE::STATE_MENU;
	m_Wrapper = std::make_unique<BubbleWrapper>();
	Audio::getInstance().SetMusicVolume(Settings::get().GetMusicVolume());
	Audio::getInstance().SetEffectsVolume(Settings::get().GetSoundEffectsVolume());
	
	m_Rendering = std::make_unique<Rendering>(Settings::get().GetWindowWidth(), Settings::get().GetWindowHeight(), m_Wrapper->GetRendered());
	m_Gameplay = std::make_unique<Gameplay>(m_Rendering->GetWindow()->getSize().x);
	m_Physics = std::make_unique<Physics>(m_Wrapper->GetGameObjects(), m_Rendering->GetWindow()->getSize().x, m_Rendering->GetWindow()->getSize().y);
	m_CloudSaves = std::make_unique<CloudSaves>();
	m_LeaderBoard = std::make_unique<LeaderBoard>();
	m_Physics->CreateContainerLines();

	m_CloudSaves->LoadPersonalTop10();
	m_Rendering->UpdateHighScore(m_CloudSaves->GetAllScores());
	m_Rendering->UpdateNextUp(m_Gameplay->GetNextBubble());
	m_Rendering->GetSettingSlider(0)->SetSliderValue(Settings::get().GetMusicVolume());
	m_Rendering->GetSettingSlider(1)->SetSliderValue(Settings::get().GetSoundEffectsVolume());
	m_CloudSaves->PrintTop10();
	m_LeaderBoard->DownloadTopScores();


}

BubbleGame::~BubbleGame()
{
	SteamAPI_Shutdown();

}

void BubbleGame::PlayUpdate(float a_Delta)
{


	m_Gameplay->Update(a_Delta);
	m_Physics->Update(a_Delta);

	for (size_t i = 0; i < m_Physics->m_BubblesToCombine.size(); i++)
	{
		const BubbleObject* bubble1 = m_Physics->m_BubblesToCombine[i].first;
		auto bubble2 = m_Physics->m_BubblesToCombine[i].second;

		if(bubble1->GetBubbleType() == EBUBBLE_TYPE::TYPE_SPIKY_BOMB || bubble2->GetBubbleType() == EBUBBLE_TYPE::TYPE_SPIKY_BOMB)
		{
			m_Wrapper->GetShapeByPointer(bubble1)->SetAnimate(true, false);
			m_Wrapper->RemoveObjectByPointer(bubble2);

			continue;
		}

		m_ComboTextPositions = BubbleMath::Lerp(bubble1->GetPosition(),bubble2->GetPosition(), 0.5f);
		m_ComboTextPositions.x *= Settings::get().GetPixelToMeter();
		m_ComboTextPositions.y *= Settings::get().GetPixelToMeter();
		m_ComboTextPositions.y *= -1;
		m_Rendering->UpdateComboPosition(m_ComboTextPositions);

		m_Rendering->UpScaleComboText(0.5);
		m_ScaleTimer = sf::Clock();


		if(bubble1->GetBubbleType() == EBUBBLE_TYPE::TYPE_BATH_BOMB || bubble2->GetBubbleType() == EBUBBLE_TYPE::TYPE_BATH_BOMB)
		{
			if(bubble1->GetBubbleType() == EBUBBLE_TYPE::TYPE_BATH_BOMB && bubble2->GetBubbleType() == EBUBBLE_TYPE::TYPE_BATH_BOMB)
			{
				auto combined = std::make_unique<BubbleObject>(EBUBBLE_TYPE::TYPE_BATH_BOMB);
				combined->SetPosition(BubbleMath::Lerp(bubble1->GetPosition(), bubble2->GetPosition(), 0.5f));
				CreateWrapper(combined);
			}
			else
			{
				if(bubble1->GetBubbleType() == EBUBBLE_TYPE::TYPE_BATH_BOMB)
				{
					const BubbleObject* temp = bubble1;
					bubble1 = bubble2;
					bubble2 = temp;
				}
				int random = Random::getInstance().GetRandomNumber(0, 100);
				std::cout << random << "\n";
				if(random <= 50)
				{
					auto combined = m_Gameplay->CombineBubble(bubble1, bubble2, true);
					CreateWrapper(combined);
				}
				else if(random > 50)
				{
					auto combined = m_Gameplay->CombineBubble(bubble1, bubble2, false);
					CreateWrapper(combined);
				}
				else
				{
					m_Physics->ShakeBox(1000, 10);
				}
			}
		}
		else
		{
			auto combined = m_Gameplay->CombineBubble(bubble1, bubble2, true);
			CreateWrapper(combined);			
		}


		unsigned combo = Settings::get().GetComboScore() * m_Gameplay->GetComboScore();
		m_Rendering->UpdateCombo(combo);

		m_Wrapper->RemoveObjectByPointer(bubble1);
		m_Wrapper->RemoveObjectByPointer(bubble2);

	}

	m_Wrapper->Update();
	NextComboText();
	m_Rendering->UpdateScore(m_Gameplay->GetScore());
	m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
	m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());
	if (m_Physics->GetTouchedTopLine()) { GameOver();}
	for (int i = 0; i < m_Wrapper->GetRendered().size(); i++)
	{
		if (m_Wrapper->GetRendered()[i]->IsAnimFinished())
		{
			m_Wrapper->RemoveObjectByIndex(i);
		}
	}

}

void BubbleGame::RestartGame()
{
	m_Gameplay->Reset(static_cast<float>(m_Rendering->GetWindow()->getSize().x));
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
	bool isFocused = true;
	const float SIM_DT = 1.0f / 60.0f;

	float simTime = 0.0f;
	float accumulator = 0.0f;

	while(m_Rendering->GetWindow()->isOpen())
	{
		SteamAPI_RunCallbacks();

		float frameTime = dtClock.restart().asSeconds();
		if (frameTime > 0.25f) frameTime = 0.25f; // avoid spiral of death
		accumulator += frameTime;

		while (const std::optional event = m_Rendering->GetWindow()->pollEvent())
		{
			sf::Event value = event.value();
			if(event->is<sf::Event::Closed>())
			{
				m_Rendering->GetWindow()->close();
			}
			else if(event->is<sf::Event::Resized>())
			{
				m_Rendering = std::make_unique<Rendering>(m_Rendering->GetWindow()->getSize().x, m_Rendering->GetWindow()->getSize().y, m_Wrapper->GetRendered());
			}
			else if(event->is<sf::Event::FocusLost>())
			{
				std::cout << "Focus Lost";
				isFocused = false;
			}
			else if(event->is<sf::Event::FocusGained>())
			{
				isFocused = true;
			}
			if(event->is<sf::Event::MouseButtonPressed>()&& event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
			{
				m_IsMouseButtonPressed = true;
			}
			else if(event->is<sf::Event::MouseButtonReleased>() && event->getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Left)
			{
				m_IsMouseButtonPressed = false;
			}
			if (event->is<sf::Event::Closed>())
				m_Rendering->GetWindow()->close();

			if (m_State == EGAME_STATE::STATE_PLAY )
			{
				PlayInput(value,frameTime);
			}
			else if(m_State == EGAME_STATE::STATE_MENU)
			{
				MenuInput(value);
			}
			else if(m_State == EGAME_STATE::STATE_GAME_OVER_ANIMATION)
			{
				GameOverAnimationInput(value);
			}
			else if(m_State == EGAME_STATE::STATE_GAME_OVER)
			{
				GameOverInput(value);
			}
			else if(m_State == EGAME_STATE::STATE_HIGH_SCORE)
			{
				sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
				if(m_IsMouseButtonPressed && m_Rendering->GetHSBackButton()->DetectClick(mousePosition))
				{
					CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_MENU; },"SetMenuState", 0.2f, false);
				}
			}
			else if(m_State == EGAME_STATE::STATE_MENU_CONFIRM || m_State == EGAME_STATE::STATE_RESTART_CONFIRM
				|| m_State == EGAME_STATE::STATE_EXIT_CONFIRM || m_State == EGAME_STATE::STATE_SETTINGS_CONFIRM)
			{
				ConfirmInput(value);
			}
			else if(m_State == EGAME_STATE::STATE_SETTINGS)
			{
				SettingsInput(value);
			}
			
		}

		while (accumulator>= SIM_DT)
		{
			simTime += frameTime;
			if (m_State == EGAME_STATE::STATE_PLAY)
			{
				PlayUpdate(SIM_DT);
			}
			accumulator -= SIM_DT;
		}

		CallAfterDelay::getInstance().LoopThroughFunctions();


		m_Rendering->Draw(m_State, frameTime);
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
	m_CloudSaves->SubmitScore(m_Gameplay->GetScore());
	m_LeaderBoard->SubmitScore(m_Gameplay->GetScore());
	m_Rendering->UpdateHighScore(m_CloudSaves->GetAllScores());
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
		
	m_Gameplay->Reset(static_cast<float>(m_Rendering->GetWindow()->getSize().x));
	m_Physics->Reset();
	m_Wrapper->Clear();

	CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_LOADING; }, "SetLoadingState", 0.1f, false);
	CallAfterDelay::getInstance().AddFunction([this]() { m_Rendering->ResetAllBackground(); m_State = EGAME_STATE::STATE_MENU;  }, "SetMenuState", Settings::get().GetLoadTime(), false);
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

void BubbleGame::PlayInput(const sf::Event& a_Event, float a_Delta)
{

	if (const auto* keyEvent = a_Event.getIf<sf::Event::KeyPressed>()) {
		sf::Keyboard::Key pressedCode = keyEvent->code;

		if (pressedCode == sf::Keyboard::Key::Space && !m_IsSpacePressed) 
		{
			m_IsSpacePressed = true;
			if (std::chrono::duration<float> elapsedSeconds = std::chrono::system_clock::now() - m_Gameplay->GetLastDrop(); elapsedSeconds.count() > 1)
			{
				m_ComboTextPositions = sf::Vector2f(INFINITY, INFINITY);
				AddBubble();
				m_Rendering->UpdateNextUp(m_Gameplay->GetNextBubble());
			}			
		}
		else if (pressedCode == sf::Keyboard::Key::D || pressedCode == sf::Keyboard::Key::Right)
		{
			m_Gameplay->UpdateMoveDirection(1.f);
		}
		else if (pressedCode == sf::Keyboard::Key::A || pressedCode == sf::Keyboard::Key::Left)
		{
			m_Gameplay->UpdateMoveDirection(-1.f);
		}
		else if (pressedCode == sf::Keyboard::Key::S || pressedCode == sf::Keyboard::Key::Down && !m_IsStorageButtonPressed)
		{
			if (m_Gameplay->GetStorage() != EBUBBLE_TYPE::TYPE_NULL)
			{
				std::cout << "Storage full! \n";
			}
			else
			{
				m_IsStorageButtonPressed = true;
				m_Rendering->StartMoveToStorage(m_Gameplay->GetCurrentBubble(), true);

				m_Gameplay->AddToStorage(m_Gameplay->GetCurrentBubble());
			}
		}
		else if ((pressedCode == sf::Keyboard::Key::W || pressedCode == sf::Keyboard::Key::Up) && !m_IsStorageButtonPressed)
		{
			if(m_Gameplay->GetStorage() !=EBUBBLE_TYPE::TYPE_NULL)
			{
				m_IsStorageButtonPressed = true;
				m_Rendering->StartMoveToStorage(m_Gameplay->GetCurrentBubble(), false);

				m_Gameplay->PullUpStorage();
				
			}
		}

		if (pressedCode == sf::Keyboard::Key::P)
		{
			GameOver();
		}
	}

	if(m_IsMouseButtonPressed)
	{
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

			RestartGame();
		}
	}


	if (const auto* keyEvent = a_Event.getIf<sf::Event::KeyReleased>()) 
	{
		sf::Keyboard::Key releasedCode = keyEvent->code;

		if (releasedCode == sf::Keyboard::Key::Space && m_IsSpacePressed)
		{
			m_IsSpacePressed = false;
		}
		else if ((releasedCode == sf::Keyboard::Key::D && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			|| (releasedCode == sf::Keyboard::Key::A && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			|| (releasedCode == sf::Keyboard::Key::Left && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) 
			|| (releasedCode == sf::Keyboard::Key::Right && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)))
		{
			m_Gameplay->UpdateMoveDirection(0.f);
		}
		else if (releasedCode == sf::Keyboard::Key::W || releasedCode == sf::Keyboard::Key::S
			|| releasedCode == sf::Keyboard::Key::Down || releasedCode == sf::Keyboard::Key::Up)
		{
			m_IsStorageButtonPressed = false;
		}
	}
}

void BubbleGame::MenuInput(const sf::Event& a_Event)
{
	if (m_IsMouseButtonPressed)
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
}

void BubbleGame::GameOverAnimationInput(const sf::Event& a_Event)
{
	if (a_Event.is<sf::Event::KeyPressed>())
	{
		m_State = EGAME_STATE::STATE_GAME_OVER;
	}
}

void BubbleGame::GameOverInput(const sf::Event& a_Event)
{
	if (m_IsMouseButtonPressed)
	{
		sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
		if (m_Rendering->GetMenuButtons().at("PlayAgain")->DetectClick(mousePosition))
		{
			RestartGame();
		}
		if (m_Rendering->GetMenuButtons().at("BackToMenu")->DetectClick(mousePosition))
		{
			m_Gameplay->Reset(static_cast<float>(m_Rendering->GetWindow()->getSize().x));
			m_Physics->Reset();
			m_Rendering->Reset();
			m_Wrapper->Clear();

			CallAfterDelay::getInstance().AddFunction([this]() { m_Rendering->ResetAllBackground(); m_State = EGAME_STATE::STATE_MENU; }, "BackToMenu", 0.2f, false);

		}
	}
}

void BubbleGame::ConfirmInput(const sf::Event& a_Event)
{
	if (m_IsMouseButtonPressed)
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
			if (m_State == EGAME_STATE::STATE_SETTINGS_CONFIRM)
			{
				CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_SETTINGS; }, "BackToSettings", 0.2f, false);
			}
		}
	}
}

void BubbleGame::SettingsInput(const sf::Event& a_Event)
{
	sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));

	if (m_Rendering->GetSettingSlider(0)->DetectClick(mousePosition, m_IsMouseButtonPressed))
	{
		Audio::getInstance().SetMusicVolume(m_Rendering->GetSettingSlider(0)->GetSliderValue());
	}
	if (m_Rendering->GetSettingSlider(1)->DetectClick(mousePosition, m_IsMouseButtonPressed))
	{
		Audio::getInstance().SetEffectsVolume(m_Rendering->GetSettingSlider(1)->GetSliderValue());
	}
	if (m_IsMouseButtonPressed && m_Rendering->GetMenuButtons().at("ApplySettings")->DetectClick(mousePosition))
	{
		Settings::get().SetSoundEnabled(m_Rendering->GetSettingSlider(0)->GetSliderValue(), m_Rendering->GetSettingSlider(1)->GetSliderValue());
	 
		Settings::get().UpdateSettings(m_Rendering->GetFullscreenCheckbox()->IsChecked());
		if (m_Rendering->GetFullscreenCheckbox()->IsChecked() != Settings::get().IsFullscreen())
		{
			Settings::get().SetFullscreen(m_Rendering->GetFullscreenCheckbox()->IsChecked());
			m_Rendering->UpdateConfirmText(EGAME_STATE::STATE_SETTINGS_CONFIRM);
			CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_SETTINGS_CONFIRM; }, "ApplySettings", 0.1f, false);
		}
		else
			CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_MENU; }, "ApplySettings", 0.1f, false);
	}
	if (m_IsMouseButtonPressed && m_Rendering->GetMenuButtons().at("Revert")->DetectClick(mousePosition))
	{
		CallAfterDelay::getInstance().AddFunction([this]()
		{
				m_Rendering->GetFullscreenCheckbox()->SetEnableCheckbox(Settings::get().IsFullscreen());
				m_Rendering->GetSettingSlider(0)->SetSliderValue(Settings::get().GetMusicVolume());
				Audio::getInstance().SetMusicVolume(Settings::get().GetMusicVolume());
				m_Rendering->GetSettingSlider(1)->SetSliderValue(Settings::get().GetSoundEffectsVolume());
				Audio::getInstance().SetEffectsVolume(Settings::get().GetSoundEffectsVolume());
				m_State = EGAME_STATE::STATE_MENU;
		}, "RevertSettings", 0.1f, false);
	}

	if(m_IsMouseButtonPressed) m_Rendering->GetFullscreenCheckbox()->DetectClick(mousePosition);
}
