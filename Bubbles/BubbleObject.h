#pragma once
#include <SFML/System/Vector2.hpp>

#include "Declarations.h"
#include "GameObject.h"
#include <map>

class BubbleObject : public GameObject
{
public:
	BubbleObject(const sf::Vector2f& a_Position, float a_Rotation, EBUBBLE_TYPE a_Type)
		: m_Position(a_Position), m_Rotation(a_Rotation), m_BubbleType(a_Type), m_Radius(bubble_sizes.at(a_Type)),
		  m_AngularVelocity(0),
		  m_AngularAcceleration(0),
		  m_Torque(0)
	{
		m_Type = EOBJECT_TYPE::TYPE_CIRCLE;
	}

	BubbleObject(EBUBBLE_TYPE a_Type) : m_Rotation(0), m_BubbleType(a_Type), m_Radius(bubble_sizes.at(a_Type)),
	                                    m_AngularVelocity(0),
	                                    m_AngularAcceleration(0),
	                                    m_Torque(0)
	{
		m_Type = EOBJECT_TYPE::TYPE_CIRCLE;
	}

	sf::Vector2f GetPosition() const { return m_Position; }
	float GetRadius() const { return m_Radius; }
	EBUBBLE_TYPE GetBubbleType() const { return m_BubbleType; }

	void SetPosition(const sf::Vector2f& a_NewPosition) { m_Position = a_NewPosition; }

	void Integrate(float a_Delta);

private:
	sf::Vector2f m_Position;
	float m_Rotation;
	EBUBBLE_TYPE m_BubbleType;
	float m_Radius;

	sf::Vector2f m_LinearVelocity;
	sf::Vector2f m_LinearAcceleration;
	sf::Vector2f m_Force;

	float m_AngularVelocity;
	float m_AngularAcceleration;
	float m_Torque;

};

inline void BubbleObject::Integrate(float a_Delta)
{
	m_LinearAcceleration.y = GRAVITY; 

	// Update velocity
	m_LinearVelocity += m_LinearAcceleration * a_Delta;
	// Update position
	m_Position += m_LinearVelocity * a_Delta;
}
