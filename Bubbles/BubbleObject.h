#pragma once
#include <SFML/System/Vector2.hpp>

#include "Declarations.h"
#include "GameObject.h"
#include <map>

#include "Settings.h"

class BubbleObject : public GameObject
{
public:
	BubbleObject(const sf::Vector2f& a_Position, float a_Rotation, EBUBBLE_TYPE a_Type)
		:  m_BubbleType(a_Type), m_Radius(Settings::get().BubbleSize(a_Type))
	{
		m_Position = a_Position;
		m_Rotation = a_Rotation;
		m_Type = EOBJECT_TYPE::TYPE_CIRCLE;
		m_Mass = 10;
		m_InverseMass = 1.f / m_Mass;
		m_Restitution = 0.4f;
		m_DynamicFriction = 0.4f;
		m_StaticFriction = 0.6f;
		BubbleObject::CalculateInertia();

	}

	BubbleObject(EBUBBLE_TYPE a_Type) : m_BubbleType(a_Type), m_Radius(Settings::get().BubbleSize(a_Type))
	{
		m_Rotation = 0;
		m_Type = EOBJECT_TYPE::TYPE_CIRCLE;
		m_Mass = bubble_weights.at(a_Type);
		m_InverseMass = 1.f / m_Mass;
		m_Restitution = 0.4f;
		m_DynamicFriction = 0.4f;
		m_StaticFriction = 0.5f;
		BubbleObject::CalculateInertia();

	}
	void CalculateInertia() override;

	float GetRadius() const { return m_Radius; }
	EBUBBLE_TYPE GetBubbleType() const { return m_BubbleType; }

private:
	EBUBBLE_TYPE m_BubbleType;
	float m_Radius;
};
