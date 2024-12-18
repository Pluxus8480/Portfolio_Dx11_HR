#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"
#include "Effect_Union.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPartObject;
END


BEGIN(Client)

class CMusician_Robot : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;


private:
	CMusician_Robot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMusician_Robot(const CMusician_Robot& rhs);
public:
	virtual ~CMusician_Robot();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

	void Create_Instrument();
	void Play_Instrument();
public:
	void Render_On(_bool bToggle = true)
	{
		m_bRender = bToggle;
	}

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CMusician_Robot> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual void Dying()override;
private:

	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);
	void Attack(_float fTimeDelta);

private:
	vector<string> m_vecAnimList;
	_bool m_bRender = true;
	_bool m_bAttacking = false;

	_int m_iType = 6;

	vector <shared_ptr< CPartObject>> m_pWeapon;

private:	// For Effect
	_float4x4 m_matCurrentPos = {};

};

END