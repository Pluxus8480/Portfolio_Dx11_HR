#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CPepperMint_Effect_MF_1 : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CPepperMint_Effect_MF_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPepperMint_Effect_MF_1(const CPepperMint_Effect_MF_1& rhs);

public:
	virtual ~CPepperMint_Effect_MF_1();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void EffectStartCallBack();
	virtual void EffectEndCallBack();

	virtual void EffectSoftEnd();
	virtual void EffectHardEnd();

private:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CTexture>		m_pTextureCom = { nullptr };
	shared_ptr<CVIBuffer_Rect>	m_pVIBufferCom = { nullptr };
	shared_ptr<CTexture>		m_pNoiseTextureCom = { nullptr };
	_float4x4 m_matRotation;
	_float m_fAngle = 0.f;
	_float3 m_vRotationAxis = { 0.f,0.f,-1.f };
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CPepperMint_Effect_MF_1> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END