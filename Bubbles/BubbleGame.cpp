#include "BubbleGame.h"

void BubbleGame::Initialize()
{
	m_rendering = std::make_unique<Rendering>(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void BubbleGame::Update()
{
	while(m_rendering->GetWindow()->isOpen())
	{
		m_rendering->draw();
	}
}
