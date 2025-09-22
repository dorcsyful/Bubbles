#pragma once
#include "Declarations.h"

#include "BubbleWrapper.h"
#include "Gameplay.h"
#include "Physics.h"
#include "Rendering.h"
#include <windows.h>
#include "CloudSaves.h"
#include "LeaderBoard.h"
#include "Achievments.h"
class BubbleGame
{
public:

	BubbleGame() {}
	void Initialize();
	~BubbleGame();
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
	void PlayInput(const sf::Event& a_Event, float a_Delta);
	void MenuInput(const sf::Event& a_Event);
	void GameOverAnimationInput(const sf::Event& a_Event);
	void GameOverInput(const sf::Event& a_Event);
	void ConfirmInput(const sf::Event& a_Event);
	void SettingsInput(const sf::Event& a_Event);

	void GameOver();

	void RemoveAtEnd();
	void BackToMenu();

	void NextComboText();

	bool m_IsStorageButtonPressed = false;
	bool m_IsSpacePressed = false;
	bool m_IsMouseButtonPressed = false;
	sf::Vector2f m_ComboTextPositions = sf::Vector2f(INFINITY, INFINITY);
	sf::Clock m_ScaleTimer;

	std::unique_ptr<LeaderBoard> m_LeaderBoard;
	std::unique_ptr<BubbleWrapper> m_Wrapper;
	std::unique_ptr<CloudSaves> m_CloudSaves;
	std::unique_ptr<Achievments> m_Achievments;
};

