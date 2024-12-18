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

class CRekka_Spark_Lightning : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CRekka_Spark_Lightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_Spark_Lightning(const CRekka_Spark_Lightning& rhs);

public:
	virtual ~CRekka_Spark_Lightning();

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

	_float3 m_vDir;
	_float  m_fSize = 1.f;
	_float4x4 m_Local;

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CRekka_Spark_Lightning> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END