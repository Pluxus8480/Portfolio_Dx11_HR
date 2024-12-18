#pragma once

#include "Client_Defines.h"
#include "CUi_Store.h"

BEGIN(Client)

class CUi_StoreSkillEquip : public CUi_Store
{
public:
	static wstring ObjID;


protected:
	CUi_StoreSkillEquip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_StoreSkillEquip(const CUi_StoreSkillEquip& rhs);


public:
	virtual ~CUi_StoreSkillEquip();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual void Progress_Store(_float fTimeDelta) override;
	virtual void Select_Bar() override;


public:
	virtual void Move(MoveDir Dir, _float fTimeDelta) override;
	virtual void Add_Color(ColorType Type, _float Color) override;
	virtual void Set_Color(ColorType Type, _float Color) override;
	virtual void Add_Pos(_float PosX, _float PosY) override;
	virtual void Set_Active(_bool Active) override;
	virtual void Dying() override;
	void Set_Equip(_bool Equip) { m_bEquip = Equip; }

	void Initialize_SkillBar();

private:
	void Initialize_EquipBack();
	void Initialize_EquipDefault();
	void Change_Equip(_float fTimeDelta);
	void Check_Skill();
	void Set_ReadyChange(_bool Change);


private:
	_float3 m_fEquipPos = { 650, 0, 0 };
	_float3 m_fNonEquipPos = { 0, 0, 0 };
	vector<shared_ptr<class CUi_EquipDefault>> m_EquipWindowVec;
	_bool m_bEquip = { false };
	_float m_fBarOriginPos = { -200 };
	_bool m_bReadyChange = { false };
	_uint m_iChangeFocusIndex = { 0 };


public:
	static shared_ptr<CUi_StoreSkillEquip> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
