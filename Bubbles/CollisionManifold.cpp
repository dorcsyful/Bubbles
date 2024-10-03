#include "CollisionManifold.h"

#include "Declarations.h"

void CollisionManifold::ApplyImpulse()
{
    // Early out and positional correct if both objects have infinite mass
    //if (Equal(m_Objects[0]->im + m_Objects[1]->im, 0))
    //{
    //    InfiniteMassCorrection();
    //    return;
    //}

    for (int i = 0; i < 1; ++i)
    {
        // Calculate radii from COM to contact
        sf::Vector2f ra = m_CollisionPoints[i] - m_Objects[0]->GetPosition();
        sf::Vector2f rb = m_CollisionPoints[i] - m_Objects[1]->GetPosition();

        // Relative velocity
        sf::Vector2f rv = m_Objects[1]->GetLinearVelocity() + BubbleMath::Cross(m_Objects[1]->GetAngularVelocity(), rb) -
            m_Objects[0]->GetLinearVelocity() - BubbleMath::Cross(m_Objects[0]->GetAngularVelocity(), ra);

        // Relative velocity along the normal
        float contactVel = BubbleMath::Dot(rv, m_Normal);

        // Do not resolve if velocities are separating
        if (contactVel > 0)
            return;

        float raCrossN = BubbleMath::Cross(ra, m_Normal);
        float rbCrossN = BubbleMath::Cross(rb, m_Normal);
        float obj0InverseMass = 1 / bubble_sizes.at(m_Objects[0]->GetBubbleType());
        float obj1InverseMass = 1 / bubble_sizes.at(m_Objects[1]->GetBubbleType());
        float invMassSum = obj0InverseMass + obj1InverseMass + sqrtf(raCrossN) * m_Objects[0]->GetInverseInertia() + sqrtf(rbCrossN) * m_Objects[1]->GetInverseInertia();

        // Calculate impulse scalar
        float j = -(1.0f + m_AvgRestitution) * contactVel;
        j /= invMassSum;
        j /= 2.f;

        // Apply impulse
        sf::Vector2f impulse = m_Normal * j;
        m_Objects[0]->ApplyImpulse(-impulse, ra);
        m_Objects[1]->ApplyImpulse(impulse, rb);

        // Friction impulse
        rv = m_Objects[1]->GetLinearVelocity() + BubbleMath::Cross(m_Objects[1]->GetAngularVelocity(), rb) -
            m_Objects[0]->GetLinearVelocity() - BubbleMath::Cross(m_Objects[0]->GetAngularVelocity(), ra);

        sf::Vector2f t = rv - (m_Normal * BubbleMath::Dot(rv, m_Normal));
        t = BubbleMath::Normalize(t);

        // j tangent magnitude
        float jt = -BubbleMath::Dot(rv, t);
        jt /= invMassSum;
        jt /= 2.f;

        // Don't apply tiny friction impulses
        if (abs(jt) < FLT_EPSILON)
            return;

        // Coulumb's law
        sf::Vector2f tangentImpulse;
        if (std::abs(jt) < j * m_AvgStaticFriction)
            tangentImpulse = t * jt;
        else
            tangentImpulse = t * -j * m_AvgDynamicFriction;

        // Apply friction impulse
        m_Objects[0]->ApplyImpulse(-tangentImpulse, ra);
        m_Objects[1]->ApplyImpulse(tangentImpulse, rb);
    }
}
