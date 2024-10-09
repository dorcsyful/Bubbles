#pragma once
#include <memory>
#include <vector>

#include "BubbleObject.h"
#include "CollisionDetection.h"
#include "LineObject.h"

class Physics
{
public:

	void Update(float a_Delta);

	void AddLine(const std::shared_ptr<LineObject>& a_Line) { m_Lines.push_back(a_Line); }
	void AddBubble(const std::shared_ptr<BubbleObject>& a_Bubble) { m_Bubbles.push_back(a_Bubble); }
	void RemoveBubble(const std::shared_ptr<BubbleObject>& a_Bubble) { m_Bubbles.erase(std::find(m_Bubbles.begin(), m_Bubbles.end(), a_Bubble)); }

private:

	std::vector<std::shared_ptr<LineObject>> m_Lines;
	std::vector<std::shared_ptr<BubbleObject>> m_Bubbles;
	std::vector<std::shared_ptr<CollisionManifold>> m_Manifolds;
};

