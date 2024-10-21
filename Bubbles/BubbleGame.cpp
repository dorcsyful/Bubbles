#include "BubbleGame.h"

#include <SFML/Window/Event.hpp>

#include "Helpers.h"
#include <chrono>

void BubbleGame::Initialize()
{
	m_State = EGAME_STATE::STATE_GAME_OVER;
	m_Rendering = std::make_unique<Rendering>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_Gameplay = std::make_unique<Gameplay>();
	m_Physics = std::make_unique<Physics>();

	auto lines = m_Rendering->ConvertToLine();
	for (const auto& lineObject : lines)
	{
		m_Physics->AddLine(lineObject);
	}

	m_Physics->AddTopLine(m_Rendering->ConvertTopLine());
	
	m_Wrappers = std::vector<std::shared_ptr<BubbleWrapper>>();
}

void BubbleGame::PlayUpdate(float a_Delta)
{
	m_Gameplay->Update(a_Delta);
	m_Physics->Update(a_Delta);


	for (size_t i = 0; i < m_Physics->m_BubblesToCombine.size(); i++)
	{
		auto combined = m_Gameplay->CombineBubble(m_Physics->m_BubblesToCombine[i].first, m_Physics->m_BubblesToCombine[i].second);
		m_Physics->AddBubble(combined);
		size_t markedWrapper[2] = { 0,0 };
		for (size_t j = 0; j < m_Wrappers.size(); j++)
		{
			if (m_Wrappers[j]->GetBubble() == m_Physics->m_BubblesToCombine[i].first)
			{
				markedWrapper[0] = j;
			}
			if (m_Wrappers[j]->GetBubble() == m_Physics->m_BubblesToCombine[i].second)
			{
				markedWrapper[1] = j;
			}
		}
		m_Physics->RemoveBubble(m_Physics->m_BubblesToCombine[i].first);
		m_Physics->RemoveBubble(m_Physics->m_BubblesToCombine[i].second);

		std::shared_ptr<BubbleWrapper> bubbleWrapper = m_Wrappers[markedWrapper[0]];
		m_Rendering->RemoveSprite(bubbleWrapper->GetShape());
		std::shared_ptr<BubbleWrapper> wrapper = m_Wrappers[markedWrapper[1]];
		m_Rendering->RemoveSprite(wrapper->GetShape());
		m_Wrappers.erase(std::find(m_Wrappers.begin(), m_Wrappers.end(), bubbleWrapper));
		m_Wrappers.erase(std::find(m_Wrappers.begin(), m_Wrappers.end(), wrapper));
		CreateWrapper(combined);

	}

	m_Rendering->UpdateScore(m_Gameplay->GetScore());

	for (const auto& bubbleWrapper : m_Wrappers)
	{
		bubbleWrapper->Update();
	}

	m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
	m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());

	if (m_Physics->GetTouchedTopLine()) { GameOver();}
}

void BubbleGame::MenuUpdate(float a_Delta)
{
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
			else if(m_State == EGAME_STATE::STATE_MENU || m_State == EGAME_STATE::STATE_GAME_OVER)
			{
				sf::Vector2f mousePosition = m_Rendering->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_Rendering->GetWindow()));
				for (const auto& button : m_Rendering->GetMenuButtons())
				{
					if (button.second->DetectClick(mousePosition))
					{
						StartLoading();
						CallAfterDelay::getInstance().AddFunction([this](){StartGame();}, LOADING_TIME, false);
					}
				}
			}
			else if(m_State == EGAME_STATE::STATE_GAME_OVER_ANIMATION)
			{
				if ( sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					m_State = EGAME_STATE::STATE_GAME_OVER;
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

void BubbleGame::CreateWrapper(const std::shared_ptr<BubbleObject>& a_NewBubble)
{
	sf::Vector2f temp = a_NewBubble->GetPosition();
	temp.x *= PIXEL_TO_METER;
	temp.y *= PIXEL_TO_METER;
	temp.y *= -1.f;
	std::shared_ptr<AnimatedSprite> newRendered = m_Rendering->AddSprite(a_NewBubble->GetBubbleType(), temp, 0);
	std::shared_ptr<BubbleWrapper> wrapper = std::make_shared <BubbleWrapper>(a_NewBubble, newRendered);
	m_Wrappers.push_back(wrapper);
}

void BubbleGame::AddBubble(float a_Delta)
{
	sf::Vector2f start = m_Rendering->GetPreviewPosition();
	std::shared_ptr<BubbleObject> newBubble = m_Gameplay->Drop(start);
	CreateWrapper(newBubble);
	m_Physics->AddBubble(newBubble);
	m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
	m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());
}

void BubbleGame::GameOver()
{
	std::cout << "GameOver \n";
	m_State = EGAME_STATE::STATE_GAME_OVER_ANIMATION;
	CallAfterDelay::getInstance().AddFunction([this](){RemoveAtEnd();}, GAME_OVER_ANIMATION_TOTAL_TIME / m_Wrappers.size(), true);
}

void BubbleGame::RemoveAtEnd()
{
	if(m_Wrappers.empty())
	{
		CallAfterDelay::getInstance().RemoveFunction([this](){RemoveAtEnd();});
		m_State = EGAME_STATE::STATE_GAME_OVER;
		return;
	}
	int index = rand() % m_Wrappers.size();
	std::shared_ptr<BubbleWrapper> bubbleWrapper = m_Wrappers[index];
	m_Wrappers.erase(std::find(m_Wrappers.begin(), m_Wrappers.end(), bubbleWrapper));

	m_Rendering->RemoveSprite(bubbleWrapper->GetShape());
}
