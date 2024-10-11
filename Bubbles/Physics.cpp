#include "Physics.h"

#include <iostream>

void Physics::Update(float a_Delta)
{
    m_BubblesToCombine.clear();
    if (m_BubblesToCombine.size() > 0)
    {
        throw new std::exception("Bubbles to combine not empty!");
    }
    // Generate new collision info
    m_Manifolds.clear();
    for (unsigned int i = 0; i < m_Bubbles.size(); ++i)
    {
        for (const auto& line : m_Lines) {
            std::shared_ptr<CollisionManifold> manifold = std::make_shared<CollisionManifold>(m_Bubbles[i], line);
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
            std::shared_ptr<CollisionManifold> manifold = std::make_shared<CollisionManifold>(m_Bubbles[i],m_Bubbles[j]);
            if (CollisionDetection::CircleCircleCheck(*m_Bubbles[i], *m_Bubbles[j], manifold))
            {
                m_Manifolds.emplace_back(manifold);
	            if(m_Bubbles[i]->GetBubbleType() == m_Bubbles[j]->GetBubbleType())
	            {
                    std::pair<std::shared_ptr<BubbleObject>, std::shared_ptr<BubbleObject>> combination;
                    combination.first = m_Bubbles[j];
                    combination.second = m_Bubbles[i];
                    if(std::find(m_BubblesToCombine.begin(),m_BubblesToCombine.end(),combination) == m_BubblesToCombine.end())
                    {
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