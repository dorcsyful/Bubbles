#pragma once
#include <SFML/System/Vector2.hpp>
#include "BubbleObject.h"
#include "GameObject.h"

class CollisionManifold
{
public:
	CollisionManifold(GameObject* a_First, GameObject* a_Second);

	void ApplyImpulse();

	float m_AvgRestitution;
	float m_AvgStaticFriction;
	float m_AvgDynamicFriction;

	GameObject* m_Objects[2];
	float m_Penetration;
	sf::Vector2f m_Normal;
	sf::Vector2f m_CollisionPoint;

	void FixFinalPenetrations() const
	{
		const float k_slop = 0.001f; // Penetration allowance
		const float percent = 0.5f; // Penetration percentage to correct

		sf::Vector2f correction = (std::max(m_Penetration - k_slop, 0.0f) / (m_Objects[0]->GetInverseMass() + m_Objects[1]->GetInverseMass())) * m_Normal * percent;

		if(m_Objects[0]->GetType() == EOBJECT_TYPE::TYPE_CIRCLE)
		{
			BubbleObject* bubble = static_cast<BubbleObject*>(m_Objects[0]);
			bubble->SetPosition(m_Objects[0]->GetPosition() - (correction * m_Objects[0]->GetInverseMass()));
		}
		if(m_Objects[1]->GetType() == EOBJECT_TYPE::TYPE_CIRCLE)
		{
			auto bubble = static_cast<BubbleObject*>(m_Objects[1]);
			bubble->SetPosition(m_Objects[1]->GetPosition() + (correction * m_Objects[1]->GetInverseMass()));
		}
	}
};
