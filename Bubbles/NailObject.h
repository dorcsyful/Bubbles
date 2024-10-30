#pragma once
#include "Declarations.h"
#include "GameObject.h"

class NailObject : public GameObject
{
public:
	NailObject()
	{
		m_Type = EOBJECT_TYPE::TYPE_NAIL;
	}
};

