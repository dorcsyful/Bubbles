#include "Physics.h"

#include <iostream>

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
			manifold = std::make_unique<CollisionManifold>(m_GameObjects[i].get(), line.get());
			if (CollisionDetection::CircleLineCheck(*static_cast<BubbleObject*>(m_GameObjects[i].get()), *line, manifold)) {
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
				{
					m_Manifolds.emplace_back(std::move(manifold));
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

void Physics::CreateContainerLines()
{
	sf::Vector2f basePos = sf::Vector2f((static_cast<float>(WINDOW_WIDTH) / 2.f) - (CONTAINER_WIDTH / 2.f), ((static_cast<float>(WINDOW_HEIGHT) - CONTAINER_HEIGHT) / 2.f));

	sf::Vector2f start = basePos;
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	sf::Vector2f end = sf::Vector2f(start.x, (-(basePos.y + CONTAINER_HEIGHT) / PIXEL_TO_METER));
	std::unique_ptr<LineObject> temp = std::make_unique<LineObject>(start, end);
	m_Lines.push_back(std::move(temp));

	sf::Vector2f pos1 = basePos;
	pos1.x += CONTAINER_WIDTH;
	start = pos1;
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	end = sf::Vector2f(start.x, (-(pos1.y + CONTAINER_HEIGHT) / PIXEL_TO_METER));
	temp = std::make_unique<LineObject>(start, end);
	m_Lines.push_back(std::move(temp));

	sf::Vector2f pos2 = basePos;
	pos2.y += CONTAINER_HEIGHT;
	start = pos2;
	start.x /= PIXEL_TO_METER;
	start.y /= PIXEL_TO_METER;
	start.y *= -1;
	end = sf::Vector2f((pos2.x + CONTAINER_WIDTH) / PIXEL_TO_METER, start.y);
	temp = std::make_unique<LineObject>(start, end);
	m_Lines.push_back(std::move(temp));

	sf::Vector2f start1 = start;
	start1.y += (CONTAINER_HEIGHT / PIXEL_TO_METER);
	start1.y += 1;
	sf::Vector2f end1 = sf::Vector2f(start1.x + (CONTAINER_WIDTH / PIXEL_TO_METER), start1.y);
	temp = std::make_unique<LineObject>(start1, end1);
	m_TopLine = std::move(temp);
}