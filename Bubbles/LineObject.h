#pragma once
#include <SFML/System/Vector2.hpp>

#include "GameObject.h"

class LineObject : public GameObject
{
public:
	LineObject(const sf::Vector2f& a_StartPoint, const sf::Vector2f& a_EndPoint)
	{
		m_Type = EOBJECT_TYPE::TYPE_LINE;
		m_StartPoint = a_StartPoint;
		m_EndPoint = a_EndPoint;
	}

	sf::Vector2f GetStart() const { return m_StartPoint; }
	sf::Vector2f GetEnd() const { return m_EndPoint; }

private:
	sf::Vector2f m_StartPoint;
	sf::Vector2f m_EndPoint;
};
