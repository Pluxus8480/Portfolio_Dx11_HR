#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_Default : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_Default(const CUi_Default& rhs);


public:
	virtual ~CUi_Default();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


private:
	void Initialize_BeatmeterResource();
	void Initialize_BeatMarkerResource();
	void Initialize_RankResource();
	void Initialize_ScoreBackResource();
	void Initialize_FinalResultBackResource();
	void Initialize_StageResultResource();
	void Initialize_HpResource();
	void Initialize_GearResource();
	void Initialize_StoreResource();
	void Initialize_CutSceneResource();
	void Initialize_RekkaResource();
	void Initialize_KorsicaResource();
	void Initialize_EnemySpecialAttackResource();
	void Initialize_MapInterActionResource();
	void Initialize_SuccessResource();
	void Initialize_GeneratorResource();
	void Initialize_StunGaugeResource();
	void Initialize_KaleResource();
	void Initialize_SpecialComboAttackResource();
	void Initialize_MimosaResource();
	void Initialize_MimosaCutinResource();


public:
	void Set_OwnActive(_bool Active) { m_bOwnActive = Active; }


private:
	_bool m_bOwnActive = { true };


public:
	static shared_ptr<CUi_Default> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
