#pragma once
#include "Declarations.h"
#include "Rendering.h"

class BubbleGame
{
public:

	void Initialize();
	void Update();

	std::unique_ptr<Rendering> m_rendering;
};

