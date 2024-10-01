#include "CollisionDetection.h"

bool CollisionDetection::CircleLineCheck(const BubbleObject& a_Bubble, const LineObject& a_Line, float& a_Penetration, sf::Vector2f& a_Normal)
{

// is either end INSIDE the circle?
	// if so, return true immediately
	bool inside1 = PointCircleCheck(a_Line.GetStart(), a_Bubble.GetPosition(), a_Bubble.GetRadius());
	bool inside2 = PointCircleCheck(a_Line.GetEnd(), a_Bubble.GetPosition(), a_Bubble.GetRadius());
	if (inside1 || inside2) return true;

	// get length of the line
	float len = Distance(a_Line.GetStart(), a_Line.GetEnd());

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

	a_Penetration = distance;
	a_Normal = Normalize((sf::Vector2f(closestX, closestY) - a_Bubble.GetPosition()));
	return distance <= a_Bubble.GetRadius();
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

bool CollisionDetection::LinePointCheck(const LineObject& a_Line, sf::Vector2f a_Point)
{
	// get distance from the point to the two ends of the line
	float d1 = Distance(a_Point, a_Line.GetStart());
	float d2 = Distance(a_Point, a_Line.GetEnd());

	// get the length of the line
	float lineLen = Distance(a_Line.GetStart(), a_Line.GetEnd());

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

float CollisionDetection::Distance(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2)
{
	float dx = a_Point2.x - a_Point1.x;
	float dy = a_Point2.y - a_Point1.y;
	return sqrtf(dx * dx + dy * dy);
}

float CollisionDetection::Dot(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2)
{
	return a_Point1.x * a_Point2.x + a_Point1.y * a_Point2.y;
}

sf::Vector2f CollisionDetection::Normalize(const sf::Vector2f& vector) {
	float length = sqrtf(vector.x * vector.x + vector.y * vector.y);
	if (length == 0.0f) {
		return {0.0f, 0.0f}; // Avoid division by zero
	}
	return {vector.x / length, vector.y / length};
}