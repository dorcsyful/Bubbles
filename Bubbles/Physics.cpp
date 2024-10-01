#include "Physics.h"

#include <iostream>

void Physics::Update(float a_Delta)
{
	Integrate(a_Delta);
	DetectCollision();
}

void Physics::DetectCollision()
{
    for (const auto& bubble : m_Bubbles) {

    	for (const auto& line : m_Lines) {
            float penetration;
            sf::Vector2f normal;
            bool circleLineCheck = CollisionDetection::CircleLineCheck(*bubble, *line, penetration, normal);

            if (circleLineCheck) {
                sf::Vector2f mtv = normal * (penetration - bubble->GetRadius());
	            // Recalculate the bubble's position based on the new point of contact
	            bubble->SetPosition(bubble->GetPosition() + mtv);

            }
        }
    }

}

void Physics::Integrate(float a_Delta)
{
	for (const auto& current : m_Bubbles)
	{
		current->ApplyGravity(a_Delta);
	}
}
