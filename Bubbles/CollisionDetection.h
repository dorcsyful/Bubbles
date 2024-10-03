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
	static bool CircleCircleCheck(const BubbleObject& a_First, const BubbleObject& a_Second, const std::shared_ptr<CollisionManifold>& a_Manifold);

	static bool PointCircleCheck(const sf::Vector2f& a_Point, const sf::Vector2f& a_Center, float a_Radius);
	static bool LinePointCheck(const LineObject& a_Line, const sf::Vector2f& a_Point);

};

