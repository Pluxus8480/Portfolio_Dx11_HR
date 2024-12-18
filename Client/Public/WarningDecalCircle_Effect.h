#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
END


BEGIN(Client)

class CWarningDecalCircle_Effect : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CWarningDecalCircle_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWarningDecalCircle_Effect(const CWarningDecalCircle_Effect& rhs);

public:
	virtual ~CWarningDecalCircle_Effect();

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
	shared_ptr<CVIBuffer_Cube>	m_pVIBufferCom = { nullptr };
	shared_ptr<CTexture>		m_pNoiseTextureCom = { nullptr };
	_float m_fThreshold = 0.f;
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CWarningDecalCircle_Effect> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END