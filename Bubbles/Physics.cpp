#include "Physics.h"

void Physics::Update(float a_Delta)
{
    m_Manifolds.clear();
    m_BubblesToCombine.clear();
    // Generate new collision info
    for (unsigned int i = 0; i < m_GameObjects.size(); ++i)
    {
    	GameObject* object1 = m_GameObjects[i].get();

        //Check for game over and return immediately if it is
        std::unique_ptr<CollisionManifold> manifold = std::make_unique<CollisionManifold>(object1, m_TopLine.get());
        if (m_GameObjects[i]->GetType() == EOBJECT_TYPE::TYPE_CIRCLE) {

            m_TouchedTopLine = CollisionDetection::CircleLineCheck(*static_cast<BubbleObject*>(m_GameObjects[i].get()), *m_TopLine, manifold);
            if (m_TouchedTopLine) return;
        }

        for (const auto& line : m_Lines) {
            if (m_GameObjects[i]->GetType() != EOBJECT_TYPE::TYPE_CIRCLE) break;
            manifold = std::make_unique<CollisionManifold>(m_GameObjects[i].get(), line.get());
            if (CollisionDetection::CircleLineCheck(*static_cast<BubbleObject*>(m_GameObjects[i].get()), *line, manifold)) {
                std::unique_ptr<CollisionManifold> newManifold = std::make_unique<CollisionManifold>(line.get(), m_GameObjects[i].get());
                newManifold->m_CollisionPoint = manifold->m_CollisionPoint;
                newManifold->m_Normal = -manifold->m_Normal;
                newManifold->m_Penetration = manifold->m_Penetration;
                m_Manifolds.emplace_back(std::move(newManifold));
                
                m_Manifolds.emplace_back(std::move(manifold));
            }
        }

    	for (unsigned int j = i + 1; j < m_GameObjects.size(); ++j)
        {
            manifold = std::make_unique<CollisionManifold>(m_GameObjects[i].get(), m_GameObjects[j].get());
            if ((m_GameObjects[i]->GetType() == EOBJECT_TYPE::TYPE_CIRCLE && m_GameObjects[j]->GetType() == EOBJECT_TYPE::TYPE_CIRCLE))
            {
                BubbleObject* bubble1 = static_cast<BubbleObject*>(m_GameObjects[i].get());
                BubbleObject* bubble2 = static_cast<BubbleObject*>(m_GameObjects[j].get());
                if(CollisionDetection::CircleCircleCheck(*bubble1, *bubble2, manifold))
                m_Manifolds.emplace_back(std::move(manifold));
	            {
		            if(bubble1->GetBubbleType() == bubble2->GetBubbleType())
		            {
		            	if(!BubbleAlreadyInCombineList(bubble1) && !BubbleAlreadyInCombineList(bubble2))
		            	{
		            		std::pair<const BubbleObject*, const BubbleObject*> combination;
		            		combination.first = bubble1;
		            		combination.second = bubble2;
		            		m_BubblesToCombine.emplace_back(combination);	                    
		            	}

		            }
	            }
            }
        }
    }

    // Integrate forces
    for (unsigned int i = 0; i < m_GameObjects.size(); ++i)
        m_GameObjects[i]->IntegrateForces(a_Delta);

    // Solve collisions
    for (unsigned int j = 0; j < 10; ++j)
    {
        for (unsigned int i = 0; i < m_Manifolds.size(); ++i)
        {
            m_Manifolds[i]->ApplyImpulse();
        }
    }

    // Integrate velocities
    for (unsigned int i = 0; i < m_GameObjects.size(); ++i)
        m_GameObjects[i]->Integrate(a_Delta);

    for (unsigned int i = 0; i < m_Manifolds.size(); ++i)
        m_Manifolds[i]->PositionalCorrection();
}

void Physics::Reset()
{
	m_Manifolds.clear();
	m_BubblesToCombine.clear();
	m_TouchedTopLine = false;
}

bool Physics::BubbleAlreadyInCombineList(const BubbleObject* a_Bubble) const
{
    for(const auto& bubblePair : m_BubblesToCombine)
	{
		if (bubblePair.first == a_Bubble || bubblePair.second == a_Bubble)
			return true;
	}
    return false;
}
