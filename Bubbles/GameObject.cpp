#include "GameObject.h"
#include <SFML/System/Vector2.hpp>

#include "BubbleMath.h"
#include "Declarations.h"

void GameObject::Integrate(float a_Delta)
{
	m_Position += m_LinearVelocity * a_Delta;
	m_Rotation += m_AngularVelocity * a_Delta;
	IntegrateForces(a_Delta);
}

void GameObject::IntegrateForces(float a_Delta)
{
	sf::Vector2f gravity(0, GRAVITY);
	m_LinearVelocity += (m_Force * m_InverseMass + gravity) * (a_Delta / 2.0f);
}

void GameObject::ApplyImpulse(const sf::Vector2f& a_Impulse, const sf::Vector2f& a_ContactVector) {

	m_LinearVelocity += m_InverseMass * a_Impulse;
	m_AngularVelocity += m_InverseInertia * BubbleMath::Cross(a_ContactVector, a_Impulse);

}