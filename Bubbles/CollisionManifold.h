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
	CollisionManifold(const std::shared_ptr<GameObject>& a_First, const std::shared_ptr<GameObject>& a_Second):
		m_AvgRestitution(0),
		m_AvgStaticFriction(0),
		m_AvgDynamicFriction(0),
		m_Penetration(0)
	{
		m_Objects[0] = a_First;
		m_Objects[1] = a_Second;

		// Calculate average restitution
		m_AvgRestitution = std::min(m_Objects[0]->GetRestitution(), m_Objects[1]->GetRestitution());

		// Calculate static and dynamic friction
		m_AvgStaticFriction = sqrtf(m_Objects[0]->GetStaticFriction() * m_Objects[1]->GetStaticFriction());
		m_AvgDynamicFriction = sqrtf(m_Objects[0]->GetDynamicFriction() * m_Objects[1]->GetDynamicFriction());
	}

	void SetData();

	void ApplyImpulse();

	float m_AvgRestitution;
	float m_AvgStaticFriction;
	float m_AvgDynamicFriction;

	std::shared_ptr<GameObject> m_Objects[2];
	float m_Penetration;
	sf::Vector2f m_Normal;
	sf::Vector2f m_CollisionPoint;

	void PositionalCorrection()
	{
		const float k_slop = 0.05f; // Penetration allowance
		const float percent = 0.4f; // Penetration percentage to correct

		sf::Vector2f correction = (std::max(m_Penetration - k_slop, 0.0f) / (m_Objects[0]->GetInverseMass() + m_Objects[1]->GetInverseMass())) * m_Normal * percent;

		if(m_Objects[0]->m_IsBubble)
		{
			auto bubble = static_cast<BubbleObject*>(m_Objects[0].get());
			bubble->SetPosition(m_Objects[0]->GetPosition() - (correction * m_Objects[0]->GetInverseMass()));
		}
		if(m_Objects[1]->m_IsBubble)
		{
			auto bubble = static_cast<BubbleObject*>(m_Objects[1].get());
			bubble->SetPosition(m_Objects[1]->GetPosition() + (correction * m_Objects[1]->GetInverseMass()));
		}
	}
};
