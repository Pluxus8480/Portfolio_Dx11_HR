//PeppermintContainer
#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CPeppermintContainer : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CPeppermintContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPeppermintContainer(const CPeppermintContainer& rhs);
public:
	~CPeppermintContainer();

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
	//

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

	virtual void Edit_Attribute() override;

	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_float4x4 m_ColliderOffset = Identity;

	_uint m_iAnimIndex = {};

	_bool m_bInit = { false };
	_uint m_iFirstBeat = {};
	_uint m_iPrevBeat = {};
	_uint m_iCurIndex = {};

	_float m_fPlaySpeed = 1.f;

	_bool m_bTalk = { false };
	_bool m_isTalkOver = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CPeppermintContainer> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END