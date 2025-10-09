#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "BubbleObject.h"
#include "CollisionDetection.h"
#include "LineObject.h"

class Physics
{
public:

	Physics(std::vector<std::unique_ptr<GameObject>>& a_Objects, float a_WindowWidth, float a_WindowHeight) :
	m_GameObjects(a_Objects), m_WindowWidth(a_WindowWidth), m_WindowHeight(a_WindowHeight)
	{ m_Lines = std::vector<std::unique_ptr<LineObject>>(); }

	void Update(float a_Delta);

	void AddLine(std::unique_ptr<LineObject>& a_Line) { m_Lines.push_back(std::move(a_Line)); }
	void AddTopLine(std::unique_ptr<LineObject>& a_Line) { m_TopLine = std::move(a_Line); }

	bool GetTouchedTopLine() const { return m_TouchedTopLine; }
	float GetTopLineHeight() const { return m_TopLine->GetPosition().y; }
	void CreateContainerLines(sf::Vector2f a_TopLeft);

	void Reset();
	std::vector<std::pair<const BubbleObject*, const BubbleObject*>> m_BubblesToCombine;

	void ShakeBox(float shakeIntensity, float decayFactor) {
		for (auto& circle : m_GameObjects) {
			// Generate random force
			float angle = static_cast<float>(rand()) / RAND_MAX * 2.f * 3.14f;
			float forceMagnitude = shakeIntensity * static_cast<float>(rand()) / RAND_MAX;
			float forceX = forceMagnitude * cos(angle);
			float forceY = forceMagnitude * sin(angle);
			circle->AddForce(sf::Vector2f(forceX, forceY));
		}
	}

private:
	bool BubbleAlreadyInCombineList(const BubbleObject* a_Bubble) const;

	bool m_TouchedTopLine = false;

	std::unique_ptr<LineObject> m_TopLine;
	std::vector<std::unique_ptr<LineObject>> m_Lines;
	std::vector<std::unique_ptr<GameObject>>& m_GameObjects;
	std::vector<std::unique_ptr<CollisionManifold>> m_Manifolds;

	float m_WindowWidth;
	float m_WindowHeight;
};

