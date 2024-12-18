#pragma once

#include "Client_Defines.h"
#include "CUi_Default.h"
#include "PlayerManager.h"


BEGIN(Client)

class CUi_EquipDefault : public CUi_Default
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_EquipDefaultInfo : public CUi_Default::Desc
	{
		_bool Big = { true };

	};

protected:
	CUi_EquipDefault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_EquipDefault(const CUi_EquipDefault& rhs);


public:
	virtual ~CUi_EquipDefault();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


public:
	virtual void Add_Color(ColorType Type, _float Color) override;
	virtual void Set_Color(ColorType Type, _float Color) override;
	virtual void Set_Color(_float4 Color) override;
	virtual void Add_Pos(_float PosX, _float PosY) override;
	virtual void Set_Active(_bool Active) override;
	void Set_Skill(CChai::SkillDesc Skill);
	wstring Get_SkillName() const;

	
private:
	void Initialize_GaugeVec(_bool Big);
	void Initialize_Font(_bool Big);


private:
	vector<shared_ptr<CUi_Default>> m_GaugeVec;
	shared_ptr<CUi_Default> m_pTwinkle;
	_bool m_bBig = { true };
	FontDescInfo m_FontString;
	_float2 m_OriginPos = {};
	_float m_fMoveTime = {};
	_int m_iGaugeCount = { 0 };


public:
	static shared_ptr<CUi_EquipDefault> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
