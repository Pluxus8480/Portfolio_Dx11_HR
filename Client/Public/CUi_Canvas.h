#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "CUi_Sprite.h"
#include "CUi_Font.h"
#include "CUi_PlayerHp.h"
#include "CUi_PlayerReverb.h"
#include "CUi_PlayerPartnerGauge.h"

BEGIN(Client)

class CUi_Canvas : public CUi_2D
{
public:
	static wstring ObjID;

protected:
	CUi_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_Canvas(const CUi_Canvas& rhs);

public:
	virtual ~CUi_Canvas();
	virtual void ReceiveUIEvent(UI_EVENT InUIEvent) {};

public:
	virtual HRESULT Initialize(void* pArg) override;
};

END
