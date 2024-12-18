#pragma once
#include "Client_Defines.h"

BEGIN(Client)

struct MISCOBJ_DESC : public CTransform::TRANSFORM_DESC
{
	_bool bCollision = { true };
	_float4x4 worldMatrix = {};
};

class CMisc_Interface
{
public:
	virtual void Edit_Attribute() {}
	virtual void Save_Data(ofstream& os) {}
	virtual void Load_Data(ifstream& is) {}
};

END