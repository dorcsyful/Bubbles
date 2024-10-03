#pragma once
#include <iostream>
#include <memory>
#include <SFML/System/Vector2.hpp>

#include "BubbleMath.h"
#include "BubbleObject.h"
#include "GameObject.h"

class CollisionManifold
{
public:
	CollisionManifold(const std::shared_ptr<BubbleObject>& a_First, const std::shared_ptr<BubbleObject>& a_Second):
		m_AvgRestitution(0),
		m_AvgStaticFriction(0),
		m_AvgDynamicFriction(0),
		m_Penetration(0)
	{
		m_Objects[0] = a_First;
		m_Objects[1] = a_Second;
	}

	void SetData(float a_Delta)
	{
		// Calculate average restitution
		m_AvgRestitution = std::min(m_Objects[0]->GetRestitution(), m_Objects[1]->GetRestitution());

		// Calculate static and dynamic friction
		m_AvgStaticFriction = sqrtf(m_Objects[0]->GetStaticFriction() * m_Objects[1]->GetStaticFriction());
		m_AvgDynamicFriction = sqrtf(m_Objects[0]->GetDynamicFriction() * m_Objects[1]->GetDynamicFriction());

		// Calculate radii from COM to contact
		sf::Vector2f ra = m_CollisionPoints[0] - m_Objects[0]->GetPosition();
		sf::Vector2f rb = m_CollisionPoints[0] - m_Objects[1]->GetPosition();

		sf::Vector2f rv = m_Objects[1]->GetLinearVelocity() + BubbleMath::Cross(m_Objects[1]->GetAngularVelocity(), rb) -
			m_Objects[0]->GetLinearVelocity() - BubbleMath::Cross(m_Objects[0]->GetAngularVelocity(), ra);


		// Determine if we should perform a resting collision or not
		// The idea is if the only thing moving this object is gravity,
		// then the collision should be performed without any restitution
		if (BubbleMath::LengthSquared(rv) < BubbleMath::LengthSquared(a_Delta * sf::Vector2f(0,GRAVITY)) + 0.000001)
			m_AvgRestitution = 0.0f;
	}

	void ApplyImpulse();

	float m_AvgRestitution;
	float m_AvgStaticFriction;
	float m_AvgDynamicFriction;

	std::shared_ptr<BubbleObject> m_Objects[2];
	float m_Penetration;
	sf::Vector2f m_Normal;
	sf::Vector2f m_CollisionPoints[2];

	void PositionalCorrection(void)
	{
		const float k_slop = 0.05f; // Penetration allowance
		const float percent = 0.4f; // Penetration percentage to correct
		float obj0InverseMass = 1.f / bubble_sizes.at(m_Objects[0]->GetBubbleType());
		float obj1InverseMass = 1.f / bubble_sizes.at(m_Objects[1]->GetBubbleType());
		sf::Vector2f correction = (std::max(m_Penetration - k_slop, 0.0f) / (obj0InverseMass + obj1InverseMass)) * m_Normal * percent;
		m_Objects[0]->SetPosition(m_Objects[0]->GetPosition() - (correction * obj0InverseMass));
		m_Objects[1]->SetPosition(m_Objects[1]->GetPosition() + (correction * obj1InverseMass));
	}
};
