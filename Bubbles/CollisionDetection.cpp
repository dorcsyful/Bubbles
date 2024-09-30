#include "CollisionDetection.h"

int CollisionDetection::CircleLineCheck(const BubbleObject& a_Bubble, const LineObject& a_Line,
	sf::Vector2f& a_HitPointRef1, sf::Vector2f& a_HitPointRef2)
{
	float min_distance = INFINITY;
	float max_distance = std::max(CalculateDistance(a_Bubble.GetPosition(), a_Line.GetStart()), CalculateDistance(a_Bubble.GetPosition(), a_Line.GetEnd()));

	sf::Vector2f os = a_Bubble.GetPosition() - a_Line.GetStart();
	sf::Vector2f oe = a_Bubble.GetPosition() - a_Line.GetEnd();
	sf::Vector2f se = a_Line.GetEnd() - a_Line.GetStart();
	sf::Vector2f es = a_Line.GetStart() - a_Line.GetEnd();

	if(CalculateDotProduct(os, es) > 0 && CalculateDotProduct(oe,se) > 0)
	{
		min_distance = 2 * TriangleArea(a_Bubble.GetPosition(), a_Line.GetStart(), a_Line.GetEnd()) / CalculateDistance(a_Line.GetEnd(), a_Line.GetStart());
	}
	else
	{
		min_distance = std::min(CalculateDistance(a_Bubble.GetPosition(), a_Line.GetStart()), CalculateDistance(a_Bubble.GetPosition(), a_Line.GetEnd()));
	}

	if (min_distance <= a_Bubble.GetRadius() && max_distance >= a_Bubble.GetRadius()) return true;
	else return false;
}

float CollisionDetection::TriangleArea(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2, const sf::Vector2f& a_Point3)
{
    return 0.5f * abs((a_Point1.x * (a_Point2.y - a_Point3.y)) + (a_Point2.x * (a_Point3.y - a_Point1.y)) + (a_Point3.x * (a_Point1.y - a_Point2.y)));
}

float CollisionDetection::CalculateDistance(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2)
{
	float dx = a_Point2.x - a_Point1.x;
	float dy = a_Point2.y - a_Point1.y;
	return sqrtf(dx * dx + dy * dy);
}

float CollisionDetection::CalculateDotProduct(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2)
{
	return a_Point1.x * a_Point2.x + a_Point1.y * a_Point2.y;
}
