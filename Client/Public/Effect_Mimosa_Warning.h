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

class CEffect_Mimosa_Warning : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CEffect_Mimosa_Warning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Mimosa_Warning(const CEffect_Mimosa_Warning& rhs);

public:
	virtual ~CEffect_Mimosa_Warning();

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
	static shared_ptr<CEffect_Mimosa_Warning> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END