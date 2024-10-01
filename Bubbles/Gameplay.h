#pragma once
#include <ctime>
#include <memory>
#include <vector>

#include "BubbleObject.h"

class Gameplay
{
public:
	Gameplay() {
		time_t current_time = time(nullptr);
		srand(current_time);
		m_CurrentBubble = static_cast<EBUBBLE_TYPE>(rand() % 3);
		m_NextBubble = static_cast<EBUBBLE_TYPE>(rand() % 3);
		m_ContainerEdges[0] = (static_cast<float>(WINDOW_WIDTH) / 2.f) - (CONTAINER_WIDTH / 2.f);
		m_ContainerEdges[1] = m_ContainerEdges[0] + CONTAINER_WIDTH;
		Move(0);
	}

	void Update(float a_Delta);
	void AddBubble(EBUBBLE_TYPE a_NewType);
	void CombineBubble(std::shared_ptr<BubbleObject> a_First, std::shared_ptr<BubbleObject> a_Second);
	
	void Move(float a_Direction);
	std::shared_ptr<BubbleObject> Drop(sf::Vector2f a_Start);
	float GetCurrentPosition() const { return m_CurrentPosition; }
	EBUBBLE_TYPE GetCurrentBubble() const { return m_CurrentBubble; }

private:

	float m_ContainerEdges[2];

	float m_CurrentPosition;
	EBUBBLE_TYPE m_CurrentBubble;
	EBUBBLE_TYPE m_NextBubble;
};

