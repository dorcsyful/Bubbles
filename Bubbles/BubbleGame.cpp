#include "BubbleGame.h"

#include <SFML/Window/Event.hpp>

#include "Helpers.h"
#include <chrono>

void BubbleGame::Initialize()
{
	m_Wrapper = std::make_unique<BubbleWrapper>();

	m_State = EGAME_STATE::STATE_MENU;
	m_Rendering = std::make_unique<Rendering>(WINDOW_WIDTH, WINDOW_HEIGHT, m_Wrapper->GetRendered());
	m_Gameplay = std::make_unique<Gameplay>();
	m_Physics = std::make_unique<Physics>(m_Wrapper->GetGameObjects());
	m_Save = std::make_unique<Save>();

	m_Physics->CreateContainerLines();

	m_Rendering->UpdateHighScores(m_Save->GetScores());
}

void BubbleGame::PlayUpdate(float a_Delta)
{
	m_Gameplay->Update(a_Delta);
	m_Physics->Update(a_Delta);

	for (size_t i = 0; i < m_Physics->m_BubblesToCombine.size(); i++)
	{
		auto combined = m_Gameplay->CombineBubble(m_Physics->m_BubblesToCombine[i].first, m_Physics->m_BubblesToCombine[i].second);

		m_Wrapper->RemoveObjectByPointer(m_Physics->m_BubblesToCombine[i].first);
		m_Wrapper->RemoveObjectByPointer(m_Physics->m_BubblesToCombine[i].second);

		CreateWrapper(combined);

	}

	m_Rendering->UpdateScore(m_Gameplay->GetScore());

	m_Wrapper->Update();

	m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
	m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());

	if (m_Physics->GetTouchedTopLine()) { GameOver();}
}

void BubbleGame::RestartGame()
{
	m_Gameplay->Reset();
	m_Physics->Reset();
	m_Rendering->Reset();
	m_Wrapper->Clear();
	CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_LOADING; }, 0.2f, false);
	CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_PLAY; }, LOADING_TIME, false);
}

void BubbleGame::Update()
{
	sf::Clock dtClock;
	while(m_Rendering->GetWindow()->isOpen())
	{
		float delta = dtClock.restart().asSeconds();

		sf::Event event;
		while (m_Rendering->GetWindow()->pollEvent(event))
		{

			if (event.type == sf::Event::Closed)
				m_Rendering->GetWindow()->close();

			if (m_State == EGAME_STATE::STATE_PLAY && event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::Space)
			{
				PlayInput(delta);
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
					CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_MENU; }, 0.2f, false);
				}
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
	temp.x *= PIXEL_TO_METER;
	temp.y *= PIXEL_TO_METER;
	temp.y *= -1.f;
	std::unique_ptr<AnimatedSprite> newRendered;
	m_Rendering->CreateSprite(a_NewBubble->GetBubbleType(), temp, 0, newRendered);
	m_Wrapper->AddObject(std::move(a_NewBubble), std::move(newRendered));
}

void BubbleGame::AddBubble() const
{
	sf::Vector2f start = m_Rendering->GetPreviewPosition();
	start.x /= PIXEL_TO_METER;
	start.y = m_Physics->GetTopLineHeight();
	std::unique_ptr<BubbleObject> newBubble = m_Gameplay->Drop(start);
	CreateWrapper(newBubble);
	m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
	m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());
}

void BubbleGame::GameOver()
{
	std::cout << "GameOver \n";
	m_State = EGAME_STATE::STATE_GAME_OVER_ANIMATION;
	m_Save->SaveIfHighScore(m_Gameplay->GetScore());
	CallAfterDelay::getInstance().AddFunction([this](){RemoveAtEnd();}, GAME_OVER_ANIMATION_TOTAL_TIME / m_Wrapper->GetNumOfObjects(), true);
}

void BubbleGame::RemoveAtEnd()
{
	if(m_Wrapper->GetNumOfObjects() == 0)
	{
		CallAfterDelay::getInstance().RemoveFunction([this](){RemoveAtEnd();});
		m_State = EGAME_STATE::STATE_GAME_OVER;
		return;
	}
	size_t index = rand() % m_Wrapper->GetNumOfObjects();
	m_Wrapper->RemoveObjectByIndex(index);
}

void BubbleGame::PlayInput(float a_Delta) const
{
	if (std::chrono::duration<float> elapsedSeconds = std::chrono::system_clock::now() - m_Gameplay->GetLastDrop(); elapsedSeconds.count() > 1)
	{
		m_Gameplay->SetLastDrop(std::chrono::system_clock::now());
		AddBubble();
	}
}

void BubbleGame::MenuInput()
{
	sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
	std::map<std::string, std::unique_ptr<Button>>& buttons = m_Rendering->GetMenuButtons();
	if (buttons.at("Play")->DetectClick(mousePosition))
	{
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_LOADING; }, 0.5f, false);
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_PLAY; }, LOADING_TIME, false);
	}
	if (buttons.at("High_Score")->DetectClick(mousePosition))
	{
		CallAfterDelay::getInstance().AddFunction([this]() { m_State = EGAME_STATE::STATE_HIGH_SCORE; }, 0.5f, false);
	}
}

void BubbleGame::GameOverAnimationInput()
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		m_State = EGAME_STATE::STATE_GAME_OVER;
	}
}

void BubbleGame::GameOverInput()
{
	if (sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow())); m_Rendering->GetMenuButtons().at("PlayAgain")->DetectClick(mousePosition))
	{
		RestartGame();
	}
}