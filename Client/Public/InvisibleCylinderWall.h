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

class CInvisibleCylinderWall : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	CInvisibleCylinderWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvisibleCylinderWall(const CInvisibleCylinderWall& rhs);
public:
	~CInvisibleCylinderWall();

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
	shared_ptr<CModel>			m_pModelCom = { nullptr };
	shared_ptr<CTexture>		m_pTextureCom = { nullptr };
	shared_ptr<CTexture>		m_pNoiseTextureCom = { nullptr };

	_bool m_bDone = { false };
	_float3 m_vModelSize;
	_float3	m_vModelCenter;
	_float4 m_vR = { 1.f,0.5f,0.1f,1.f };
	_float4 m_vG = { 1.f,0.f,0.1f,1.f };
	_uint m_iPrevBeatCnt = 0;
	_float2 m_vUVFlow = { 0.f,0.f };
	_float4 m_vThreshold{ 0.f,0.f,0.f,0.f };
	_bool m_bEnd = false;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static shared_ptr<CInvisibleCylinderWall> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END