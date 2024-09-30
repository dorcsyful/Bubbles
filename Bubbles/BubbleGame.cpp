#include "BubbleGame.h"

#include <SFML/Window/Event.hpp>

void BubbleGame::Initialize()
{
	m_Rendering = std::make_unique<Rendering>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_Gameplay = std::make_unique<Gameplay>();
	m_Wrappers = std::vector<std::shared_ptr<BubbleWrapper>>();
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
			if(event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::Space)
			{
				AddBubble(delta);
			}
		}

		m_Gameplay->Update(delta);

		m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
		m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());

		m_Rendering->Draw();
	}
}

void BubbleGame::AddBubble(float a_Delta)
{
	auto newBubble = m_Gameplay->Drop();
	auto newRendered = m_Rendering->AddSprite(newBubble->GetBubbleType(), newBubble->GetPosition(), 0);
	std::shared_ptr<BubbleWrapper> wrapper = std::make_shared <BubbleWrapper>(newBubble, newRendered);
	m_Wrappers.push_back(wrapper);

	m_Rendering->MovePointerLine(m_Gameplay->GetCurrentPosition());
	m_Rendering->MovePreviewBubble(m_Gameplay->GetCurrentBubble());


}
