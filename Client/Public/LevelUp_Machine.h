#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
class CTexture;
END

BEGIN(Client)

class CLevelUp_Machine : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	enum ANIM_STATE { IDLE, READY_SHOP, SHOPPING, SHOP_END, ANIM_END };

private:
	CLevelUp_Machine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLevelUp_Machine(const CLevelUp_Machine& rhs);
public:
	~CLevelUp_Machine();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom = { nullptr };

	ANIM_STATE m_eState = { IDLE };

	_int m_iTextureIndex = { 0 };
	const _int m_iMaxIndex = { 32 };

	_float m_fTime = {};
	_float m_fSecPerFrame = {};

	_bool m_bInit = { false };

	const _float4x4* m_pRootMat = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Update_Animation();
	void Animation_End();

public:
	static shared_ptr<CLevelUp_Machine> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END