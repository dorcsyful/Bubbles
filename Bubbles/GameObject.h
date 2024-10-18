#pragma once




class GameObject
{
public:
	virtual ~GameObject() = default;
	virtual float GetRotation() { return m_Rotation; }

	float GetRestitution() const { return m_Restitution; }
	float GetDynamicFriction() const { return m_DynamicFriction; }
	float GetStaticFriction() const { return m_StaticFriction; }

	sf::Vector2f GetLinearVelocity() const { return m_LinearVelocity; }
	float GetAngularVelocity() const { return m_AngularVelocity; }

	float GetMass() const { return m_Mass; }
	float GetInverseMass() const { return m_InverseMass; }
	float GetRotation() const { return m_Rotation; }
	sf::Vector2f GetPosition() const { return m_Position; }
	virtual void ApplyImpulse(const sf::Vector2f& a_Impulse, const sf::Vector2f& a_ContactVector) = 0;

	float GetInertia() const { return m_Inertia; }
	float GetInverseInertia() const { return m_InverseInertia; }

	bool m_IsBubble = false;

protected:

	float m_Rotation = 0;
	sf::Vector2f m_Position;

	float m_Mass = 0;
	float m_InverseMass = 0;
	EOBJECT_TYPE m_Type = EOBJECT_TYPE::TYPE_NO_TYPE;
	float m_Restitution = 0.4f;
	float m_DynamicFriction = 0.7f;
	float m_StaticFriction = 0.5f;

	float m_Inertia = 0;
	float m_InverseInertia = 0;

	sf::Vector2f m_LinearVelocity;
	sf::Vector2f m_LinearAcceleration;
	sf::Vector2f m_Force;
	sf::Vector2f m_Impulse;

	float m_AngularVelocity = 0;
};