#pragma once
#include <memory>
#include <SFML/System/Vector2.hpp>

#include "GameObject.h"

struct CollisionManifold
{
	bool m_Hit;
	std::shared_ptr<GameObject> m_Objects[2];
	float m_Penetration;
	sf::Vector2f m_Normal;
	sf::Vector2f m_CollisionPoints[2];
};
