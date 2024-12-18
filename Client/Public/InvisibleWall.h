
#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Rect;
class CPhysXCollider;
END

BEGIN(Client)

class CInvisibleWall : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CInvisibleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvisibleWall(const CInvisibleWall& rhs);
public:
	~CInvisibleWall();

public:
	virtual void Execute(_bool bExecute = true);


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Edit_Attribute() override;

private:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CVIBuffer_Rect>	m_pVIBufferCom = { nullptr };
	shared_ptr<CTexture>		m_pTextureCom = { nullptr };
	shared_ptr<CTexture>		m_pNoiseTextureCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider[4] = {nullptr};

	_float4x4 m_Matrices[4];
	_float4x4 m_WorldMatrices[4];

	_bool m_bDone = { false };
	_float3 m_vModelSize;
	_float3	m_vModelCenter;
	_float4 m_vR = { 1.f,0.5f,0.f,1.f };
	_float4 m_vG = { 1.f,1.f,0.f,1.f };
	_uint m_iPrevBeatCnt = 0;
	_float2 m_vUVFlow = { 0.f,0.f };
	_float4 m_vThreshold{0.f,0.f,0.f,0.f};
	_bool m_bEnd = false;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Move(_float fTimeDelta);

public:
	static shared_ptr<CInvisibleWall> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END