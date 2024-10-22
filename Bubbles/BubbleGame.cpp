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
	m_Physics = std::make_unique<Physics>(m_Wrapper->GetGameBubbles());

	auto lines = m_Rendering->ConvertToLine();
	for(auto& lineObject : lines)
	{
		m_Physics->AddLine(lineObject);
	}

	std::unique_ptr<LineObject> topLine = m_Rendering->ConvertTopLine();
	m_Physics->AddTopLine(topLine);
}

void BubbleGame::PlayUpdate(float a_Delta)
{
	m_Gameplay->Update(a_Delta);
	m_Physics->Update(a_Delta);


	for (size_t i = 0; i < m_Physics->m_BubblesToCombine.size(); i++)
	{
		auto combined = m_Gameplay->CombineBubble(m_Physics->m_BubblesToCombine[i].first, m_Physics->m_BubblesToCombine[i].second);
		size_t markedWrapper[2] = { 0,0 };
		for (size_t j = 0; j < m_Wrapper->GetNumOfBubbles(); j++)
		{
			if (m_Wrapper->GetBubbleByIndex(j) == m_Physics->m_BubblesToCombine[i].first)
			{
				markedWrapper[0] = j;
			}
			if (m_Wrapper->GetBubbleByIndex(j) == m_Physics->m_BubblesToCombine[i].second)
			{
				markedWrapper[1] = j;
			}
		}

		m_Wrapper->RemoveBubbleByIndex(markedWrapper[0]);
		m_Wrapper->RemoveBubbleByIndex(markedWrapper[0]);

		CreateWrapper(combined);

	}

	m_Rendering->UpdateScore(m_Gameplay->GetScore());

	m_Wrapper->Update();

	m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
	m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());

	if (m_Physics->GetTouchedTopLine()) { GameOver();}
}

void BubbleGame::MenuUpdate(float a_Delta)
{
}

void BubbleGame::RestartGame()
{
	m_Gameplay->Reset();
	m_Physics->Reset();
	m_Rendering->Reset();
	m_Wrapper->Clear();
	CallAfterDelay::getInstance().AddFunction([this]() {StartLoading(); }, 0.2f, false);
	CallAfterDelay::getInstance().AddFunction([this]() {StartGame(); }, LOADING_TIME, false);

}

void BubbleGame::Update()
{
	sf::Clock dtClock;
	float delta = 0;
	while(m_Rendering->GetWindow()->isOpen())
	{
		delta = dtClock.restart().asSeconds();

		sf::Event event;
		while (m_Rendering->GetWindow()->pollEvent(event))
		{

			if (event.type == sf::Event::Closed)
				m_Rendering->GetWindow()->close();

			if (m_State == EGAME_STATE::STATE_PLAY && event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::Space)
			{
				std::chrono::duration<float> elapsedSeconds = std::chrono::system_clock::now() - m_Gameplay->GetLastDrop();
				if (elapsedSeconds.count() > 1)
				{
					m_Gameplay->SetLastDrop(std::chrono::system_clock::now());
					AddBubble(delta);
				}
			}
			else if(m_State == EGAME_STATE::STATE_MENU)
			{
				sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
				if (m_Rendering->GetMenuButtons().at("Play")->DetectClick(mousePosition))
				{
					CallAfterDelay::getInstance().AddFunction([this](){StartLoading(); }, 0.2f, false);
					CallAfterDelay::getInstance().AddFunction([this](){StartGame(); }, LOADING_TIME, false);
				}
			}
			else if(m_State == EGAME_STATE::STATE_GAME_OVER_ANIMATION)
			{
				if ( sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					m_State = EGAME_STATE::STATE_GAME_OVER;
				}
			}
			else if(m_State == EGAME_STATE::STATE_GAME_OVER)
			{
				sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
				if (m_Rendering->GetMenuButtons().at("PlayAgain")->DetectClick(mousePosition))
				{
					RestartGame();
				}
			}
		}


		if (m_State == EGAME_STATE::STATE_PLAY)
		{
			PlayUpdate(delta);
		}
		if(m_State == EGAME_STATE::STATE_MENU)
		{
			MenuUpdate(delta);
		}

		CallAfterDelay::getInstance().LoopThroughFunctions();
		m_Rendering->Draw(m_State);
	}
}

void BubbleGame::CreateWrapper(std::unique_ptr<BubbleObject>& a_NewBubble)
{
	sf::Vector2f temp = a_NewBubble->GetPosition();
	temp.x *= PIXEL_TO_METER;
	temp.y *= PIXEL_TO_METER;
	temp.y *= -1.f;
	std::unique_ptr<AnimatedSprite> newRendered;
	m_Rendering->CreateSprite(a_NewBubble->GetBubbleType(), temp, 0, newRendered);
	m_Wrapper->AddBubble(std::move(a_NewBubble), std::move(newRendered));
}

void BubbleGame::AddBubble(float a_Delta)
{
	sf::Vector2f start = m_Rendering->GetPreviewPosition();
	std::unique_ptr<BubbleObject> newBubble = m_Gameplay->Drop(start);
	CreateWrapper(newBubble);
	m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
	m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());
}

void BubbleGame::GameOver()
{
	std::cout << "GameOver \n";
	m_State = EGAME_STATE::STATE_GAME_OVER_ANIMATION;
	CallAfterDelay::getInstance().AddFunction([this](){RemoveAtEnd();}, GAME_OVER_ANIMATION_TOTAL_TIME / m_Wrapper->GetNumOfBubbles(), true);
}

void BubbleGame::RemoveAtEnd()
{
	if(m_Wrapper->GetNumOfBubbles() == 0)
	{
		CallAfterDelay::getInstance().RemoveFunction([this](){RemoveAtEnd();});
		m_State = EGAME_STATE::STATE_GAME_OVER;
		return;
	}
	int index = rand() % m_Wrapper->GetNumOfBubbles();
	m_Wrapper->RemoveBubbleByIndex(index);
}
