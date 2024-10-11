#include "CollisionManifold.h"

void CollisionManifold::ApplyImpulse()
{
    // Relative velocity
    sf::Vector2f ra = m_CollisionPoint - m_Objects[0]->GetPosition();
    sf::Vector2f rb = m_CollisionPoint - m_Objects[1]->GetPosition();
    sf::Vector2f rv = m_Objects[1]->GetLinearVelocity() + BubbleMath::Cross(m_Objects[1]->GetAngularVelocity(), rb) -
					 m_Objects[0]->GetLinearVelocity() - BubbleMath::Cross(m_Objects[0]->GetAngularVelocity(), ra);

    // Relative velocity along the normal
    float contactVel = BubbleMath::Dot(rv, m_Normal);

    // Already separating and will be resolved next frame
    if (contactVel > 0)
        return;

    float raCrossN = BubbleMath::Cross(ra, m_Normal) * m_Objects[0]->GetInverseInertia();
    float rbCrossN = BubbleMath::Cross(rb, m_Normal) * m_Objects[1]->GetInverseInertia();
    float invMassSum = m_Objects[0]->GetInverseMass() + m_Objects[1]->GetInverseMass() + raCrossN + rbCrossN;

    // Calculate impulse scalar
    float j = (-(1.0f + m_AvgRestitution) * contactVel) / invMassSum;

    // Apply main impulse
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

    // Don't apply tiny friction impulses
    if (jt <= 0.0001f)
        return;

    sf::Vector2f tangentImpulse;
    if (std::abs(jt) < j * m_AvgStaticFriction)
        tangentImpulse = t * jt;
    else
        tangentImpulse = t * -j * m_AvgDynamicFriction;

    // Apply friction impulse
    m_Objects[0]->ApplyImpulse(-tangentImpulse, ra);
    m_Objects[1]->ApplyImpulse(tangentImpulse, rb);
}
