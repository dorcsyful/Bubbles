#pragma once
#include "BubbleWrapper.h"
#include "Declarations.h"
#include "Gameplay.h"
#include "Physics.h"
#include "Rendering.h"
#include "Save.h"

class BubbleGame
{
public:

	void Initialize();
	void Update();
	void CreateWrapper(std::unique_ptr<BubbleObject>& a_NewBubble) const;

	void AddBubble() const;

	std::unique_ptr<Physics> m_Physics;
	std::unique_ptr<Rendering> m_Rendering;
	std::unique_ptr<Gameplay> m_Gameplay;

	EGAME_STATE m_State;

private:
	void PlayUpdate(float a_Delta);
	void RestartGame();
	void PlayInput(const sf::Event& a_Event);
	void MenuInput();
	void GameOverAnimationInput();
	void GameOverInput();
	void ConfirmInput();

	void GameOver();

	void RemoveAtEnd();
	void BackToMenu();

	void NextComboText();

	sf::Vector2f m_ComboTextPositions = sf::Vector2f(INFINITY, INFINITY);
	sf::Clock m_ScaleTimer;

	std::unique_ptr<BubbleWrapper> m_Wrapper;
	std::unique_ptr<Save> m_Save;
};

