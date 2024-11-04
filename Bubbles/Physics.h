#pragma once
#include <memory>
#include <vector>

#include "BubbleObject.h"
#include "CollisionDetection.h"
#include "LineObject.h"

class Physics
{
public:

	Physics(std::vector<std::unique_ptr<GameObject>>& a_Objects) : m_GameObjects(a_Objects) { m_Lines = std::vector<std::unique_ptr<LineObject>>(); }

	void Update(float a_Delta);

	void AddLine(std::unique_ptr<LineObject>& a_Line) { m_Lines.push_back(std::move(a_Line)); }
	void AddTopLine(std::unique_ptr<LineObject>& a_Line) { m_TopLine = std::move(a_Line); }

	bool GetTouchedTopLine() const { return m_TouchedTopLine; }
	float GetTopLineHeight() const { return m_TopLine->GetPosition().y; }
	void CreateContainerLines();

	void Reset();
	std::vector<std::pair<const BubbleObject*, const BubbleObject*>> m_BubblesToCombine;

private:
	bool BubbleAlreadyInCombineList(const BubbleObject* a_Bubble) const;

	bool m_TouchedTopLine = false;

	std::unique_ptr<LineObject> m_TopLine;
	std::vector<std::unique_ptr<LineObject>> m_Lines;
	std::vector<std::unique_ptr<GameObject>>& m_GameObjects;
	std::vector<std::unique_ptr<CollisionManifold>> m_Manifolds;
};

