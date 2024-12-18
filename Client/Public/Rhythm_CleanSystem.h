#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CTexture;
END

BEGIN(Client)

class CRhythm_CleanSystem : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

private:
	CRhythm_CleanSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRhythm_CleanSystem(const CRhythm_CleanSystem& rhs);
public:
	~CRhythm_CleanSystem();

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
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<CTexture> m_pTextureCom[2] = { nullptr, };

	_int m_iAnimIndex = {};
	vector<_float> m_vecAnimSpeed;

	_bool m_bRunning = { false };

	_bool m_bInit = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void SetUp_PhysXCollider();

public:
	static shared_ptr<CRhythm_CleanSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END