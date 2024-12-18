#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "PlayerManager.h"


BEGIN(Client)

class CUi_SkillChangeSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_SkillChangeSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_SkillChangeSystem(const CUi_2D& rhs);


public:
	virtual ~CUi_SkillChangeSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

public:
	void Set_Change(CChai::SkillDesc EquipSkill, CChai::SkillDesc NonEquipSkill);
	

private:
	void Initialize_SkillBar();
	void Initialize_SkillBack();
	void Adjust_Pos(_float fTimeDelta);
	void Adjust_Color(_float fTimeDelta);


private:
	shared_ptr<class CUi_Default> m_pEquipSkillBar;
	shared_ptr<class CUi_DefaultFont> m_pEquipSkillBack;
	shared_ptr<class CUi_Default> m_pNonEquipSkillBar;
	shared_ptr<class CUi_DefaultFont> m_pNonEquipSkillBack;
	const _float2 m_EquipPos = { -450, -180 };
	const _float2 m_NoneEquipPos = { -400, -250 };
	_float m_fTime = { 0 };
	_bool m_bAdjustPos = { false };
	_bool m_bAdjustColor = { false };
	_float4 m_EquipColor = { 1, 0.8f, 0, 1 };
	_float4 m_NonEquipColor = { 0.3f, 0.3f, 0.3f, 1 };

public:
	static shared_ptr<CUi_SkillChangeSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
