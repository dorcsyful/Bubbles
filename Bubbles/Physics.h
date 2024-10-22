#pragma once
#include <memory>
#include <vector>

#include "BubbleObject.h"
#include "CollisionDetection.h"
#include "LineObject.h"

class Physics
{
public:

	Physics(std::vector<std::unique_ptr<BubbleObject>>& a_Bubbles) : m_Bubbles(a_Bubbles) {}

	void Update(float a_Delta);

	void AddLine(std::unique_ptr<LineObject>& a_Line) { m_Lines.push_back(std::move(a_Line)); }
	void AddTopLine(std::unique_ptr<LineObject>& a_Line) { m_TopLine = std::move(a_Line); }

	bool GetTouchedTopLine() const { return m_TouchedTopLine; }

	void Reset();
	std::vector<std::pair<const BubbleObject*, const BubbleObject*>> m_BubblesToCombine;
private:

	bool BubbleAlreadyInCombineList(const BubbleObject* a_Bubble) const;

	bool m_TouchedTopLine = false;

	std::unique_ptr<LineObject> m_TopLine;
	std::vector<std::unique_ptr<LineObject>> m_Lines;
	std::vector<std::unique_ptr<BubbleObject>>& m_Bubbles;
	std::vector<std::shared_ptr<CollisionManifold>> m_Manifolds;
};

