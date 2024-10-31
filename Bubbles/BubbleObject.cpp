#include "BubbleObject.h"

#include <iostream>

#include "Declarations.h"

void BubbleObject::CalculateInertia()
{
	m_Inertia = m_Mass * m_Radius * m_Radius;
	m_InverseInertia = 1.f / m_Inertia;
}
