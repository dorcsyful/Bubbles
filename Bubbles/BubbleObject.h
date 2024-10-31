#pragma once
#include <SFML/System/Vector2.hpp>

#include "Declarations.h"
#include "GameObject.h"
#include <map>
#include <stdexcept>

#include "BubbleMath.h"

class BubbleObject : public GameObject
{
public:
	BubbleObject(const sf::Vector2f& a_Position, float a_Rotation, EBUBBLE_TYPE a_Type)
		:  m_BubbleType(a_Type), m_Radius(bubble_sizes.at(a_Type))
	{
		m_Position = a_Position;
		m_Rotation = a_Rotation;
		m_Type = EOBJECT_TYPE::TYPE_CIRCLE;
		m_Mass = bubble_weights.at(a_Type);
		m_InverseMass = 1.f / m_Mass;
		BubbleObject::CalculateInertia();

	}

	BubbleObject(EBUBBLE_TYPE a_Type) : m_BubbleType(a_Type), m_Radius(bubble_sizes.at(a_Type))
	{
		m_Rotation = 0;
		m_Type = EOBJECT_TYPE::TYPE_CIRCLE;
		m_Mass = bubble_weights.at(a_Type);
		m_InverseMass = 1.f / m_Mass;

		BubbleObject::CalculateInertia();

	}

	void CalculateInertia() override;

	float GetRadius() const { return m_Radius; }
	EBUBBLE_TYPE GetBubbleType() const { return m_BubbleType; }

private:
	EBUBBLE_TYPE m_BubbleType;
	float m_Radius;



};