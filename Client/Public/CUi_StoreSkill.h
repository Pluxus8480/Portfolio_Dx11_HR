#pragma once

#include "Client_Defines.h"
#include "CUi_Store.h"

BEGIN(Client)

class CUi_StoreSkill : public CUi_Store
{
public:
	static wstring ObjID;


protected:
	CUi_StoreSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreSkill(const CUi_StoreSkill& rhs);


public:
	virtual ~CUi_StoreSkill();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


public:
	virtual void Check_Has() override;
	virtual void Progress_Store(_float fTimeDelta) override;
	virtual void Set_Active(_bool Active) override;


private:
	virtual void Select_Bar() override;
	void Initialize_Skill();
	void Initialize_SkillMap();
	void Initialize_Back();
	void Change_Equip(_float fTimeDelta);


private:
	virtual void Move(MoveDir Dir, _float fTimeDelta) override;
	

private:
	map<wstring, CUi_StoreDesc::SkillDesc> m_SkillMap;
	_float m_fOriginSkillY = { 300 };
	_float m_fEquipMove = { 0 };
	_float3 m_fNonEquipPos = { 0, 0, 0 };
	_float3 m_fEquipPos = { 650, 0, 0 };
	_bool m_bEquip = { false };


public:
	static shared_ptr<CUi_StoreSkill> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
