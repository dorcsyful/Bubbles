#pragma once
#include "BubbleObject.h"
#include "LineObject.h"

class CollisionDetection
{
public:

	//Checks circle against line segment, aka the container. Returns the number of hits
	static int CircleLineCheck(const BubbleObject& a_Bubble, const LineObject& a_Line, sf::Vector2f& a_HitPointRef1, sf::Vector2f& a_HitPointRef2);
	static bool CircleCircleCheck(const BubbleObject& a_First, const BubbleObject& a_Second, sf::Vector2f a_HitPointRef);

	static float TriangleArea(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2, const sf::Vector2f& a_Point3);
	static float CalculateDistance(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2);
	static float CalculateDotProduct(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2);
};

