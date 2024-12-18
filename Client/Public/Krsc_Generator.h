#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CKrsc_Generator : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CKrsc_Generator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKrsc_Generator(const CKrsc_Generator & rhs);
public:
	~CKrsc_Generator();

public:
	virtual void Execute(_bool bExecute = true);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;
	virtual void Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;

	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CTexture> m_pEmiTexture[2] = { nullptr, };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_uint m_iAnimIndex = {};

	_bool m_bInit = { false };
	_uint m_iFirstBeat = {};
	_uint m_iPrevBeat = {};
	_uint m_iCurIndex = {};

	_float m_fPlaySpeed[2] = {};

	_float4x4 m_ModelMatrix = Identity;
	_float4x4 m_ModelOffset = Identity;

	_float4x4 m_ColliderOffset = Identity;

	_float m_fTime = {};

	_bool m_bTalk = { false };
	_bool m_isTalkOver = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CKrsc_Generator> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END