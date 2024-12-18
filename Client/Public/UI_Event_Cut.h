#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUI_Event_Cut : public CUi_2D
{
public:
	enum class CutSceneType{ RekkaPowerUp, RekkaFinal, MimosaIntro, MimosaFinal,
	KalePowerUp, KaleMacaron, KaleKorsica, KalePepper, KaleChai, CutRobotArm, Korsica, End };

public:
	static wstring ObjID;

public:
	using Desc = struct CUI_Event_CutInfo
	{
		CutSceneType eCutSceneType = { CutSceneType::End };
	};

protected:
	CUI_Event_Cut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Event_Cut(const CUi_2D& rhs);


public:
	virtual ~CUI_Event_Cut();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	



private:


public:
	static shared_ptr<CUI_Event_Cut> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
