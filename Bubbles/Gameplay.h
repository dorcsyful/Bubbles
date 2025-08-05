#pragma once
#include <chrono>
#include <memory>

#include "BubbleObject.h"
#include "Random.h"
#include "Settings.h"

class Gameplay
{
public:
	explicit Gameplay(float a_WindowWidth) {
		m_CurrentBubble = static_cast<EBUBBLE_TYPE>(Random::getInstance().GetRandomNumber(0,3));
		m_NextBubble = static_cast<EBUBBLE_TYPE>(Random::getInstance().GetRandomNumber(0,3));
		m_ContainerEdges[0] = (a_WindowWidth / 2.f) - (Settings::get().GetContainerWidth() / 2.f);
		m_ContainerEdges[1] = m_ContainerEdges[0] + Settings::get().GetContainerWidth();
		Move(0);
		m_Score = 0;
		m_CombineCombo = 0;
		m_Storage = EBUBBLE_TYPE::TYPE_NULL;
	}

	void Update(float a_Delta);
	void CheatNextBubble(EBUBBLE_TYPE a_ToDrop);
	std::unique_ptr<BubbleObject> CombineBubble(const BubbleObject* a_First, const BubbleObject* a_Second, bool a_Enlarge);
	unsigned int GetComboScore() const { return m_CombineCombo; }
	unsigned int GetScore() const { return m_Score; }
	void AddScore(unsigned int a_Amount);
	void Move(float a_Direction);
	std::unique_ptr<BubbleObject> Drop(const sf::Vector2f& a_Start);
	float GetCurrentPosition() const { return m_CurrentPosition; }
	EBUBBLE_TYPE GetCurrentBubble() const { return m_CurrentBubble; }
	EBUBBLE_TYPE GetNextBubble() const { return m_NextBubble; }
	std::chrono::time_point<std::chrono::system_clock> GetLastDrop() const { return m_LastDrop; }
	void SetLastDrop(const std::chrono::time_point<std::chrono::system_clock>& a_Time) { m_LastDrop = a_Time; }
	void UpdateMoveDirection(float a_Direction) { m_MoveDirection = a_Direction; }
	void Reset(float a_WindowWidth);

	EBUBBLE_TYPE GetStorage() const { return m_Storage; }
	void AddToStorage(EBUBBLE_TYPE a_Type)
	{
		m_Storage = a_Type;
		m_CurrentBubble = m_NextBubble;
		m_NextBubble = GenerateRandom();
	}
	EBUBBLE_TYPE PullUpStorage()
	{
		m_NextBubble = m_CurrentBubble;
		m_CurrentBubble = m_Storage;
		
		EBUBBLE_TYPE temp = m_Storage;
		m_Storage = EBUBBLE_TYPE::TYPE_NULL;
		return temp;
	}
	void EmptyStorage() { m_Storage = EBUBBLE_TYPE::TYPE_NULL; }

private:

	EBUBBLE_TYPE m_Storage;

	static EBUBBLE_TYPE GenerateRandom();
	float m_MoveDirection = 0;
	unsigned int m_CombineCombo;
	unsigned int m_Score;
	std::chrono::time_point<std::chrono::system_clock> m_LastDrop;
	float m_ContainerEdges[2];

	float m_CurrentPosition;
	EBUBBLE_TYPE m_CurrentBubble;
	EBUBBLE_TYPE m_NextBubble;
};

