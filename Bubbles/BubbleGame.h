#pragma once
#include "BubbleWrapper.h"
#include "Declarations.h"
#include "Gameplay.h"
#include "Rendering.h"

class BubbleGame
{
public:

	void Initialize();
	void Update();

	void AddBubble(float a_Delta);

	std::unique_ptr<Rendering> m_Rendering;
	std::unique_ptr<Gameplay> m_Gameplay;

private:
	std::vector<std::shared_ptr<BubbleWrapper>> m_Wrappers;
};

