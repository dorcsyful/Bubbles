#include "Physics.h"

#include <iostream>

void Physics::Update(float a_Delta)
{

    m_Manifolds.clear();
    m_BubblesToCombine.clear();
    // Generate new collision info
    for (unsigned int i = 0; i < m_Bubbles.size(); ++i)
    {
        //Check for game over and return immediately if it is
        std::shared_ptr<CollisionManifold> manifold = std::make_shared<CollisionManifold>(m_Bubbles[i], m_TopLine);
        m_TouchedTopLine = CollisionDetection::CircleLineCheck(*m_Bubbles[i], *m_TopLine, manifold);
        if (m_TouchedTopLine) return;

        for (const auto& line : m_Lines) {
            manifold = std::make_shared<CollisionManifold>(m_Bubbles[i], line);
            if (CollisionDetection::CircleLineCheck(*m_Bubbles[i], *line, manifold)) {
                std::shared_ptr<CollisionManifold> newManifold = std::make_shared<CollisionManifold>(line, m_Bubbles[i]);
                newManifold->m_CollisionPoint = manifold->m_CollisionPoint;
                newManifold->m_Normal = -manifold->m_Normal;
                newManifold->m_Penetration = manifold->m_Penetration;
                m_Manifolds.emplace_back(newManifold);
                
                m_Manifolds.emplace_back(manifold);
            }
        }
        for (unsigned int j = i + 1; j < m_Bubbles.size(); ++j)
        {
            manifold = std::make_shared<CollisionManifold>(m_Bubbles[i],m_Bubbles[j]);
            if (CollisionDetection::CircleCircleCheck(*m_Bubbles[i], *m_Bubbles[j], manifold))
            {
                m_Manifolds.emplace_back(manifold);
	            if(m_Bubbles[i]->GetBubbleType() == m_Bubbles[j]->GetBubbleType())
	            {
                    if(!BubbleAlreadyInCombineList(m_Bubbles[i]) && !BubbleAlreadyInCombineList(m_Bubbles[j]))
                    {
	                    std::pair<std::shared_ptr<BubbleObject>, std::shared_ptr<BubbleObject>> combination;
	                    combination.first = m_Bubbles[i];
	                    combination.second = m_Bubbles[j];
	                    m_BubblesToCombine.emplace_back(combination);	                    
                    }

	            }
            }
        }
    }

    // Integrate forces
    for (unsigned int i = 0; i < m_Bubbles.size(); ++i)
        m_Bubbles[i]->IntegrateForces(a_Delta);

    // Solve collisions
    for (unsigned int j = 0; j < 10; ++j)
    {
        for (unsigned int i = 0; i < m_Manifolds.size(); ++i)
        {
            m_Manifolds[i]->ApplyImpulse();
        }
    }

    // Integrate velocities
    for (unsigned int i = 0; i < m_Bubbles.size(); ++i)
        m_Bubbles[i]->Integrate(a_Delta);

    for (unsigned int i = 0; i < m_Manifolds.size(); ++i)
        m_Manifolds[i]->PositionalCorrection();
}

bool Physics::BubbleAlreadyInCombineList(const std::shared_ptr<BubbleObject>& a_Bubble) const
{
    for(const auto& bubblePair : m_BubblesToCombine)
	{
		if (bubblePair.first == a_Bubble || bubblePair.second == a_Bubble)
			return true;
	}
    return false;
}
