#pragma once
#include <chrono>
#include <ctime>
#include <memory>
#include <vector>

#include "BubbleObject.h"
#include "Settings.h"

class Gameplay
{
public:
	Gameplay() {
		time_t currentTime = time(nullptr);
		srand(static_cast<unsigned int>(currentTime));
		m_CurrentBubble = static_cast<EBUBBLE_TYPE>(rand() % 3);
		m_NextBubble = static_cast<EBUBBLE_TYPE>(rand() % 3);
		m_ContainerEdges[0] = (Settings::get().GetWindowWidth() / 2.f) - (Settings::get().GetContainerWidth() / 2.f);
		m_ContainerEdges[1] = m_ContainerEdges[0] + Settings::get().GetContainerWidth();
		Move(0);
		m_Score = 0;
		m_CombineCombo = 0;
	}

	void Update(float a_Delta);
	void CheatNextBubble(EBUBBLE_TYPE a_ToDrop);
	std::unique_ptr<BubbleObject> CombineBubble(const BubbleObject* a_First, const BubbleObject* a_Second);
	unsigned int GetComboScore() const { return m_CombineCombo; }
	unsigned int GetScore() const { return m_Score; }

	void Move(float a_Direction);
	std::unique_ptr<BubbleObject> Drop(const sf::Vector2f& a_Start);
	float GetCurrentPosition() const { return m_CurrentPosition; }
	EBUBBLE_TYPE GetCurrentBubble() const { return m_CurrentBubble; }
	EBUBBLE_TYPE GetNextBubble() const { return m_NextBubble; }
	std::chrono::time_point<std::chrono::system_clock> GetLastDrop() const { return m_LastDrop; }
	void SetLastDrop(const std::chrono::time_point<std::chrono::system_clock>& a_Time) { m_LastDrop = a_Time; }

	void Reset();

private:
	unsigned int m_CombineCombo;
	unsigned int m_Score;
	std::chrono::time_point<std::chrono::system_clock> m_LastDrop;
	float m_ContainerEdges[2];

	float m_CurrentPosition;
	EBUBBLE_TYPE m_CurrentBubble;
	EBUBBLE_TYPE m_NextBubble;
};

