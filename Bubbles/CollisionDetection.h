#pragma once
#include <memory>

#include "BubbleObject.h"
#include "CollisionManifold.h"
#include "LineObject.h"
#include "NailObject.h"

class CollisionDetection
{
public:

	//Checks circle against line segment, aka the container. Returns the number of hits
	static bool CircleLineCheck(const BubbleObject& a_Bubble, const LineObject& a_Line, const std::unique_ptr<CollisionManifold>& a_Manifold);
	static bool CircleCircleCheck(const BubbleObject& a_First, const BubbleObject& a_Second, const std::unique_ptr<CollisionManifold>& a_Manifold);
	static bool CircleRectCheck(const BubbleObject& a_First, const NailObject& a_Nail, const std::unique_ptr<CollisionManifold>& a_Manifold);

	static bool PointCircleCheck(const sf::Vector2f& a_Point, const sf::Vector2f& a_Center, float a_Radius);
	static bool LinePointCheck(const LineObject& a_Line, const sf::Vector2f& a_Point);

};

