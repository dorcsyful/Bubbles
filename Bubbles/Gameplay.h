#pragma once
#include <chrono>
#include <ctime>
#include <memory>
#include <vector>

#include "BubbleObject.h"

class Gameplay
{
public:
	Gameplay() {
		time_t currentTime = time(nullptr);
		srand(static_cast<unsigned int>(currentTime));
		m_CurrentBubble = static_cast<EBUBBLE_TYPE>(rand() % 3);
		m_NextBubble = static_cast<EBUBBLE_TYPE>(rand() % 3);
		m_ContainerEdges[0] = (static_cast<float>(WINDOW_WIDTH) / 2.f) - (CONTAINER_WIDTH / 2.f);
		m_ContainerEdges[1] = m_ContainerEdges[0] + CONTAINER_WIDTH;
		Move(0);
		m_Score = 0;
	}

	void Update(float a_Delta);
	std::shared_ptr<BubbleObject> CombineBubble(const std::shared_ptr<BubbleObject>& a_First, const std::shared_ptr<BubbleObject>& a_Second);

	unsigned int GetScore() const { return m_Score; }

	void Move(float a_Direction);
	std::shared_ptr<BubbleObject> Drop(const sf::Vector2f& a_Start);
	float GetCurrentPosition() const { return m_CurrentPosition; }
	EBUBBLE_TYPE GetCurrentBubble() const { return m_CurrentBubble; }
	std::chrono::time_point<std::chrono::system_clock> GetLastDrop() const { return m_LastDrop; }
	void SetLastDrop(const std::chrono::time_point<std::chrono::system_clock>& a_Time) { m_LastDrop = a_Time; }

private:
	unsigned int m_Score;
	std::chrono::time_point<std::chrono::system_clock> m_LastDrop;
	float m_ContainerEdges[2];

	float m_CurrentPosition;
	EBUBBLE_TYPE m_CurrentBubble;
	EBUBBLE_TYPE m_NextBubble;
};

