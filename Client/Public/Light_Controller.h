#pragma once
#include "Client_Defines.h"
#include "Light.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)

class CLight_Controller : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

private:
	CLight_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLight_Controller(const CLight_Controller& rhs);
public:
	~CLight_Controller();

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

private:

#ifdef _DEBUG
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
#endif

private:
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();
	_float  m_fInnerConAngle = 45.f;
	_float  m_fOuterConAngle = 90.f;

	LIGHT_DESC m_tDesc{};
	string m_strLightTag;

public:
	static shared_ptr<CLight_Controller> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END