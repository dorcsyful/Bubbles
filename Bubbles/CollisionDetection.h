#pragma once
#include <memory>

#include "BubbleObject.h"
#include "CollisionManifold.h"
#include "LineObject.h"

class CollisionDetection
{
public:

	//Checks circle against line segment, aka the container. Returns the number of hits
	static bool CircleLineCheck(const BubbleObject& a_Bubble, const LineObject& a_Line, const std::shared_ptr<CollisionManifold>& a_Manifold);

	static bool PointCircleCheck(const sf::Vector2f& a_Point, const sf::Vector2f& a_Center, float a_Radius);
	static bool LinePointCheck(const LineObject& a_Line, const sf::Vector2f& a_Point);
	static float Distance(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2);
	static float Dot(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2);
	static sf::Vector2f Normalize(const sf::Vector2f& a_Vector);
};

