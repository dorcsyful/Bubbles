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
		: m_Position(a_Position), m_Rotation(a_Rotation), m_BubbleType(a_Type), m_Radius(bubble_sizes.at(a_Type)),
		  m_AngularVelocity(0),
		  m_AngularAcceleration(0),
		  m_Torque(0)
	{
		m_Type = EOBJECT_TYPE::TYPE_CIRCLE;
		CalculateInertia();
	}

	BubbleObject(EBUBBLE_TYPE a_Type) : m_Rotation(0), m_BubbleType(a_Type), m_Radius(bubble_sizes.at(a_Type)),
	                                    m_AngularVelocity(0),
	                                    m_AngularAcceleration(0),
	                                    m_Torque(0)
	{
		m_Type = EOBJECT_TYPE::TYPE_CIRCLE;
		CalculateInertia();
	}

	sf::Vector2f GetPosition() const { return m_Position; }
	float GetRadius() const { return m_Radius; }
	EBUBBLE_TYPE GetBubbleType() const { return m_BubbleType; }

	float GetRestitution() const { return m_Restitution; }
	float GetDynamicFriction() const { return m_DynamicFriction; }
	float GetStaticFriction() const { return m_StaticFriction; }

	sf::Vector2f GetLinearVelocity() const { return m_LinearVelocity; }
	float GetAngularVelocity() const { return m_AngularVelocity; }

	sf::Vector2f GetImpulse() const { return m_Impulse; }
	void ApplyImpulse(const sf::Vector2f& a_Impulse, const sf::Vector2f& a_ContactVector);

	float GetInertia() { return m_Inertia; }
	float GetInverseInertia() { return m_InverseInertia; }

	void SetPosition(const sf::Vector2f& a_NewPosition) { m_Position = a_NewPosition; }

	void Integrate(float a_Delta);
	void IntegrateForces(float a_Delta);

	void CalculateInertia();

private:
	sf::Vector2f m_Position;
	float m_Rotation;
	EBUBBLE_TYPE m_BubbleType;
	float m_Radius;

	float m_Inertia;
	float m_InverseInertia;

	float m_Restitution = 0.2f;
	float m_DynamicFriction = 0.4f;
	float m_StaticFriction = 0.4f;

	sf::Vector2f m_LinearVelocity;
	sf::Vector2f m_LinearAcceleration;
	sf::Vector2f m_Force;
	sf::Vector2f m_Impulse;

	float m_AngularVelocity;
	float m_AngularAcceleration;
	float m_Torque;

};