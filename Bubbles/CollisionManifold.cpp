#include "CollisionManifold.h"

#include "Declarations.h"

void CollisionManifold::SetData(float a_Delta)

{
    // Calculate average restitution
    m_AvgRestitution = std::min(m_Objects[0]->GetRestitution(), m_Objects[1]->GetRestitution());

    // Calculate static and dynamic friction
    m_AvgStaticFriction = sqrtf(m_Objects[0]->GetStaticFriction() * m_Objects[1]->GetStaticFriction());
    m_AvgDynamicFriction = sqrtf(m_Objects[0]->GetDynamicFriction() * m_Objects[1]->GetDynamicFriction());

    // Calculate radii from COM to contact
    sf::Vector2f ra = m_CollisionPoint - m_Objects[0]->GetPosition();
    sf::Vector2f rb = m_CollisionPoint - m_Objects[1]->GetPosition();

    sf::Vector2f rv = m_Objects[1]->GetLinearVelocity() + BubbleMath::Cross(m_Objects[1]->GetAngularVelocity(), rb) -
        m_Objects[0]->GetLinearVelocity() - BubbleMath::Cross(m_Objects[0]->GetAngularVelocity(), ra);


    // Determine if we should perform a resting collision or not
    // The idea is if the only thing moving this object is gravity,
    // then the collision should be performed without any restitution
    if (BubbleMath::LengthSquared(rv) < BubbleMath::LengthSquared(a_Delta * sf::Vector2f(0, GRAVITY)) + 0.000001)
        m_AvgRestitution = 0.0f;
}

void CollisionManifold::ApplyImpulse()
{
    sf::Vector2f ra = m_CollisionPoint - m_Objects[0]->GetPosition();
    sf::Vector2f rb = m_CollisionPoint - m_Objects[1]->GetPosition();

    // Relative velocity
    sf::Vector2f rv = m_Objects[1]->GetLinearVelocity() + BubbleMath::Cross(m_Objects[1]->GetAngularVelocity(), rb) -
        m_Objects[0]->GetLinearVelocity() - BubbleMath::Cross(m_Objects[0]->GetAngularVelocity(), ra);

    // Relative velocity along the normal
    float contactVel = BubbleMath::Dot(rv, m_Normal);

    // Already separating and will be resolved next frame
    if (contactVel > 0)
        return;

    float raCrossN = BubbleMath::Cross(ra, m_Normal);
    float rbCrossN = BubbleMath::Cross(rb, m_Normal);
	float x = raCrossN * raCrossN * m_Objects[0]->GetInverseInertia();
    float x1 = rbCrossN * rbCrossN * m_Objects[1]->GetInverseInertia();
    float invMassSum = m_Objects[0]->GetInverseMass() + m_Objects[1]->GetInverseMass() + x + x1;

    // Calculate impulse scalar
    float j = -(1.0f + m_AvgRestitution) * contactVel;
    j /= invMassSum;
    //j /= 2.f;

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