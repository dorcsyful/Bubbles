#pragma once
#include "BubbleObject.h"
#include "LineObject.h"

class CollisionDetection
{
public:

	//Checks circle against line segment, aka the container. Returns the number of hits
	static bool CircleLineCheck(const BubbleObject& a_Bubble, const LineObject& a_Line, float& a_Penetration, sf::Vector2f& a_Normal);
	static bool CircleCircleCheck(const BubbleObject& a_First, const BubbleObject& a_Second, sf::Vector2f a_HitPointRef);

	static bool PointCircleCheck(const sf::Vector2f& a_Point, const sf::Vector2f& a_Center, float a_Radius);
	static bool LinePointCheck(const LineObject& a_Line, sf::Vector2f a_Point);
	static float Distance(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2);
	static float DistanceSquared(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2);
	static float Dot(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2);
	static sf::Vector2f Normalize(const sf::Vector2f& vector);
};

