#include "CollisionDetection.h"

#include <iostream>

#include "BubbleMath.h"

bool CollisionDetection::CircleLineCheck(const BubbleObject& a_Bubble, const LineObject& a_Line, const std::shared_ptr<CollisionManifold>& a_Manifold)
{
	bool inside1 = PointCircleCheck(a_Line.GetStart(), a_Bubble.GetPosition(), a_Bubble.GetRadius());
	bool inside2 = PointCircleCheck(a_Line.GetEnd(), a_Bubble.GetPosition(), a_Bubble.GetRadius());
	if (inside1 || inside2) return true;

	// get length of the line
	float len = BubbleMath::Distance(a_Line.GetStart(), a_Line.GetEnd());

	float centerStartX = a_Bubble.GetPosition().x - a_Line.GetStart().x;
	float endStartX = a_Line.GetEnd().x - a_Line.GetStart().x;
	float centerStartY = a_Bubble.GetPosition().y - a_Line.GetStart().y;
	float endStartY = (a_Line.GetEnd().y - a_Line.GetStart().y);
	// get dot product of the line and circle
	float dot = (centerStartX * endStartX + (centerStartY * endStartY)) / pow(len, 2);

	// find the closest point on the line
	float closestX = a_Line.GetStart().x + (dot * (a_Line.GetEnd().x - a_Line.GetStart().x));
	float closestY = a_Line.GetStart().y + (dot * (a_Line.GetEnd().y - a_Line.GetStart().y));

	// is this point actually on the line segment?
	// if so keep going, but if not, return false
	bool onSegment = LinePointCheck(a_Line, sf::Vector2f(closestX, closestY));
	if (!onSegment) return false;

	// get distance to the closest point
	float distX = closestX - a_Bubble.GetPosition().x;
	float distY = closestY - a_Bubble.GetPosition().y;
	float distance = sqrtf((distX * distX) + (distY * distY));
	a_Manifold->m_Penetration = distance;
	a_Manifold->m_Normal = BubbleMath::Normalize((sf::Vector2f(closestX, closestY) - a_Bubble.GetPosition()));
	return distance <= a_Bubble.GetRadius();
}

bool CollisionDetection::CircleCircleCheck(const BubbleObject& a_First, const BubbleObject& a_Second,
	const std::shared_ptr<CollisionManifold>& a_Manifold)
{
	a_Manifold->m_Normal = a_Second.GetPosition() - a_First.GetPosition();
	float distance_squared = BubbleMath::LengthSquared(a_Manifold->m_Normal);
	float radius = a_First.GetRadius() + a_Second.GetRadius();

	if(distance_squared >= radius * radius)
	{
		return false;
	}

	float distance = sqrtf(distance_squared);

	if(distance == 0.0f)
	{
		a_Manifold->m_Penetration = a_First.GetRadius();
		a_Manifold->m_Normal = sf::Vector2f(1, 0);
		a_Manifold->m_CollisionPoints[0] = a_First.GetPosition();
	}
	else
	{
		a_Manifold->m_Penetration = radius - distance;
		a_Manifold->m_Normal = a_Manifold->m_Normal / distance;
		a_Manifold->m_CollisionPoints[0] = a_Manifold->m_Normal * a_First.GetRadius() + a_First.GetPosition();
	}

	return true;
	// if the distance is less than the sum of the circle's
	// radii, the circles are touching!
	

}

bool CollisionDetection::PointCircleCheck(const sf::Vector2f& a_Point, const sf::Vector2f& a_Center, float a_Radius)
{
	// get distance between the point and circle's center
	// using the Pythagorean Theorem
	float distX = a_Point.x - a_Center.x;
	float distY = a_Point.y - a_Center.y;
	float distance = sqrtf((distX * distX) + (distY * distY));

	// if the distance is less than the circle's
	// radius the point is inside!
	if (distance <= a_Radius) {
		return true;
	}
	return false;
}

bool CollisionDetection::LinePointCheck(const LineObject& a_Line, const sf::Vector2f& a_Point)
{
	// get distance from the point to the two ends of the line
	float d1 = BubbleMath::Distance(a_Point, a_Line.GetStart());
	float d2 = BubbleMath::Distance(a_Point, a_Line.GetEnd());

	// get the length of the line
	float lineLen = BubbleMath::Distance(a_Line.GetStart(), a_Line.GetEnd());

	// since floats are so minutely accurate, add
	// a little buffer zone that will give collision
	float buffer = 0.1f;    // higher # = less accurate

	// if the two distances are equal to the line's
	// length, the point is on the line!
	// note we use the buffer here to give a range,
	// rather than one #
	if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer) {
		return true;
	}
	return false;
}