#include "BubbleObject.h"

#include <iostream>

#include "Declarations.h"

void BubbleObject::Integrate(float a_Delta)
{
	m_Position += m_LinearVelocity * a_Delta;
	m_Rotation += m_AngularVelocity * a_Delta;
	if (isnan(m_Position.y))
	{
		throw std::exception("NAN");
	}
	IntegrateForces(a_Delta);
}

void BubbleObject::IntegrateForces(float a_Delta)
{
	sf::Vector2f gravity(0,GRAVITY);
	m_LinearVelocity += (m_Force * m_InverseMass + gravity) * (a_Delta / 2.0f);
	//m_AngularVelocity += m_Torque * m_InverseInertia * (a_Delta / 2.0f);
	if (isnan(m_LinearVelocity.y))
	{
		throw std::exception("NAN");
	}
}

void BubbleObject::CalculateInertia()
{
	m_Inertia = m_Mass * m_Mass * m_Radius;
	m_InverseInertia = 1.f / m_Inertia;
}

void BubbleObject::ApplyImpulse(const sf::Vector2f& a_Impulse, const sf::Vector2f& a_ContactVector) {

	m_LinearVelocity += m_InverseMass * a_Impulse;
	m_AngularVelocity += m_InverseInertia * BubbleMath::Cross(a_ContactVector, a_Impulse);
	if (isnan(m_LinearVelocity.y))
	{
		throw std::exception("NAN");
	}

}