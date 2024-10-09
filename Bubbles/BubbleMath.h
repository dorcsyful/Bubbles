#pragma once
#include <SFML/System/Vector2.hpp>
#include <corecrt_math.h>

class BubbleMath
{
public:
	static float Distance(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2)
	{
		float dx = a_Point2.x - a_Point1.x;
		float dy = a_Point2.y - a_Point1.y;
		return sqrtf(dx * dx + dy * dy);
	}

	static float LengthSquared(const sf::Vector2f& a_Vector)
	{
		return a_Vector.x * a_Vector.x + a_Vector.y * a_Vector.y;
	}

	static float Dot(const sf::Vector2f& a_Point1, const sf::Vector2f& a_Point2)
	{
		return a_Point1.x * a_Point2.x + a_Point1.y * a_Point2.y;

	}
	static sf::Vector2f Normalize(const sf::Vector2f& a_Vector)
	{
		float length = sqrtf(a_Vector.x * a_Vector.x + a_Vector.y * a_Vector.y);
		if (length == 0.0f) {
			return { 0.0f, 0.0f }; // Avoid division by zero
		}
		return { a_Vector.x / length, a_Vector.y / length };
	}
	static sf::Vector2f Cross(const sf::Vector2f& a_Vector2, float a_Scalar)
	{
		return {a_Scalar * a_Vector2.y, -a_Scalar * a_Vector2.x};
	}

	static sf::Vector2f Cross(float a_Scalar, const sf::Vector2f& a_Vector2)
	{
		return {-a_Scalar * a_Vector2.y, a_Scalar * a_Vector2.x};
	}

	static float Cross(const sf::Vector2f& a_First, const sf::Vector2f& a_Second)
	{
		return a_First.x * a_Second.y - a_First.y * a_Second.x;
	}
	static sf::Vector2f Lerp(const sf::Vector2f& a_Start, const sf::Vector2f& a_End, float a_T) {
		return (1 - a_T) * a_Start + a_T * a_End;
	}
};
