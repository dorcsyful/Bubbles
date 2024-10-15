#pragma once
#include "BubbleWrapper.h"
#include "Declarations.h"
#include "Gameplay.h"
#include "Physics.h"
#include "Rendering.h"

class BubbleGame
{
public:

	void Initialize();
	void Update();
	void CreateWrapper(const std::shared_ptr<BubbleObject>& a_NewBubble);

	void AddBubble(float a_Delta);
	void StartGame() { m_State = EGAME_STATE::STATE_PLAY; }
	void StartLoading() { m_State = EGAME_STATE::STATE_LOADING; }

	std::unique_ptr<Physics> m_Physics;
	std::unique_ptr<Rendering> m_Rendering;
	std::unique_ptr<Gameplay> m_Gameplay;

	EGAME_STATE m_State;

private:
	void PlayUpdate(float a_Delta);
	void MenuUpdate(float a_Delta);

	void GameOver();

	std::vector<std::shared_ptr<BubbleWrapper>> m_Wrappers;
};

