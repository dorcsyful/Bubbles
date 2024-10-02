#include "Physics.h"

#include <iostream>

void Physics::Update(float a_Delta)
{
    for (const auto& current : m_Bubbles)
    {
        current->Integrate(a_Delta);
    }
	DetectCollision();
}

void Physics::DetectCollision()
{
    for (const auto& bubble : m_Bubbles) {

    	for (const auto& line : m_Lines) {
            std::shared_ptr<CollisionManifold> manifold = std::make_shared<CollisionManifold>();
            bool circleLineCheck = CollisionDetection::CircleLineCheck(*bubble, *line, manifold);

            if (circleLineCheck) {
                sf::Vector2f mtv = manifold->m_Normal * (manifold->m_Penetration - bubble->GetRadius());
	            // Recalculate the bubble's position based on the new point of contact
	            bubble->SetPosition(bubble->GetPosition() + mtv);

            }
        }
    }

}
