#include "BubbleObject.h"

#include "Declarations.h"

void BubbleObject::Integrate(float a_Delta)
{
	m_Position += m_LinearVelocity * a_Delta;
	m_Rotation += m_AngularVelocity * a_Delta;
	IntegrateForces(a_Delta);
}

void BubbleObject::IntegrateForces(float a_Delta)
{
	m_LinearVelocity += (m_Force * (1.f / bubble_weights.at(m_BubbleType)) + sf::Vector2f(0,GRAVITY)) * (a_Delta / 2.0f);
	m_AngularVelocity += m_Torque * m_InverseInertia * (a_Delta / 2.0f);
}

void BubbleObject::CalculateInertia()
{
	m_Inertia = bubble_weights.at(m_BubbleType) * bubble_sizes.at(m_BubbleType) * bubble_sizes.at(m_BubbleType);
	m_InverseInertia = 1.f / m_Inertia;
}

void BubbleObject::ApplyImpulse(const sf::Vector2f& a_Impulse, const sf::Vector2f& a_ContactVector) {
	m_LinearVelocity += (1.0f / bubble_weights.at(m_BubbleType)) * a_Impulse;
	m_AngularVelocity += 1.0f / m_Inertia * BubbleMath::Cross(a_ContactVector, a_Impulse);

}