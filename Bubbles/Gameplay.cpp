#include "Gameplay.h"

#include <random>

#include "BubbleMath.h"

void Gameplay::Update(float a_Delta)
{
		Move(Settings::get().GetPointerMoveSpeed() * m_MoveDirection * a_Delta);
}

void Gameplay::CheatNextBubble(EBUBBLE_TYPE a_ToDrop)
{
	m_CurrentBubble = a_ToDrop;
}

std::unique_ptr<BubbleObject> Gameplay::CombineBubble(const BubbleObject* a_First, const BubbleObject* a_Second)
{
	sf::Vector2f center = BubbleMath::Lerp(a_First->GetPosition(), a_Second->GetPosition(), 0.5f);
	int i = static_cast<int>(a_First->GetBubbleType()) + 1;
	if (i >= 10) { i = 0; }
	std::unique_ptr<BubbleObject> newBubble = std::make_unique<BubbleObject>(static_cast<EBUBBLE_TYPE>(i));
	newBubble->SetPosition(center);
	m_Score += static_cast<unsigned int>(bubble_weights.at(a_First->GetBubbleType())) * 10;
	
	m_CombineCombo++;
	m_Score += Settings::get().GetComboScore();
	return newBubble;
}

void Gameplay::Move(float a_Direction)
{
	float temp = m_CurrentPosition + a_Direction;

	float x = m_ContainerEdges[0] + Settings::get().BubbleSize(m_CurrentBubble) * Settings::get().GetPixelToMeter();
	if (temp < x) temp = x;

	x = m_ContainerEdges[1] - Settings::get().BubbleSize(m_CurrentBubble) * Settings::get().GetPixelToMeter();
	if (temp > x) temp = x;

	m_CurrentPosition = temp;
}

std::unique_ptr<BubbleObject> Gameplay::Drop(const sf::Vector2f& a_Start)
{
	std::unique_ptr<BubbleObject> newBubble = std::make_unique<BubbleObject>(m_CurrentBubble);
	sf::Vector2f temp = a_Start;
	temp.y -= Settings::get().BubbleSize(m_CurrentBubble) + 0.001f;

	newBubble->SetPosition(temp);
	m_CurrentBubble = m_NextBubble;
	m_NextBubble = GenerateRandom();
	Move(0);
	m_CombineCombo = 0;
	return newBubble;
}

void Gameplay::Reset(float a_WindowWidth)
{
	time_t currentTime = time(nullptr);
	srand(static_cast<unsigned int>(currentTime));
	m_CurrentBubble = GenerateRandom();
	m_NextBubble = GenerateRandom();

	m_ContainerEdges[0] = a_WindowWidth / 2.f - Settings::get().GetContainerWidth() / 2.f;
	m_ContainerEdges[1] = m_ContainerEdges[0] + Settings::get().GetContainerWidth();
	m_CurrentPosition = 0;
	Move(0);
	m_Score = 0;
}

EBUBBLE_TYPE Gameplay::GenerateRandom()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 99);

	int randomNum = dis(gen);

	if (randomNum < 70) {
		// 70% chance: 0, 1, or 2
		return static_cast<EBUBBLE_TYPE>(randomNum % 3);
	}
	if (randomNum < 90) {
		// 20% chance: 3 or 4
		return static_cast<EBUBBLE_TYPE>(3);
	}
	// 10% chance: 5 or 6
	return static_cast<EBUBBLE_TYPE>(10 + randomNum % 2);
}
