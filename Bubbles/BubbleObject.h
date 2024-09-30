#pragma once
#include <SFML/System/Vector2.hpp>

#include "Declarations.h"
#include "GameObject.h"
#include <map>

class BubbleObject : public GameObject
{
public:
	BubbleObject(const sf::Vector2f& a_Position, float a_Rotation, EBUBBLE_TYPE a_Type)
		: m_Position(a_Position), m_Rotation(a_Rotation), m_BubbleType(a_Type), m_Radius(bubble_sizes.at(a_Type)) {
		m_Type = EOBJECT_TYPE::TYPE_CIRCLE;
	}
	BubbleObject(EBUBBLE_TYPE a_Type) : m_Rotation(0), m_BubbleType(a_Type), m_Radius(bubble_sizes.at(a_Type))
	{
		m_Type = EOBJECT_TYPE::TYPE_CIRCLE;
	}

	sf::Vector2f GetPosition() const { return m_Position; }
	float GetRadius() const { return m_Radius; }
	EBUBBLE_TYPE GetBubbleType() const { return m_BubbleType; }

private:
	sf::Vector2f m_Position;
	float m_Rotation;
	EBUBBLE_TYPE m_BubbleType;
	float m_Radius;
};