#pragma once
#include <SFML/System/Vector2.hpp>

#include "GameObject.h"

class LineObject : public GameObject
{
public:
	LineObject(const sf::Vector2f& a_StartPoint, const sf::Vector2f& a_EndPoint)
	{
		m_Position = BubbleMath::Lerp(a_StartPoint, a_EndPoint, 0.5f);
		m_Type = EOBJECT_TYPE::TYPE_LINE;
		m_StartPoint = a_StartPoint;
		m_EndPoint = a_EndPoint;
		m_Mass = 0;
		m_Rotation = 0;
		m_AngularVelocity = 0;
		m_InverseMass = 0;
		m_Inertia = 0;
		m_InverseInertia = 0;
		m_Restitution = 0.2f;
		m_DynamicFriction = 0.3f;
		m_StaticFriction = 0.3f;
	}

	sf::Vector2f GetStart() const { return m_StartPoint; }
	sf::Vector2f GetEnd() const { return m_EndPoint; }
	void ApplyImpulse(const sf::Vector2f& a_Impulse, const sf::Vector2f& a_ContactVector) override {}
	void CalculateInertia() override {}

private:
	sf::Vector2f m_StartPoint;
	sf::Vector2f m_EndPoint;
};
