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

class CWire : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CWire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWire(const CWire& rhs);

public:
	virtual ~CWire();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	void Set_Start_Pos(const _float4& vStartPos) { m_vStartPos = vStartPos; }
	void Set_End_Pos(const _float4& vEndPos) { m_vEndPos = vEndPos; }

public:
	virtual void EffectStartCallBack();
	virtual void EffectEndCallBack();

	virtual void EffectSoftEnd();
	virtual void EffectHardEnd();

private:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CTexture>		m_pTextureCom = { nullptr };
	shared_ptr<CVIBuffer_Rect>	m_pVIBufferCom = { nullptr };

	_float4 m_vStartPos;
	_float4 m_vEndPos;

	_float4x4 m_matRotation;
	_float m_fAngle = 0.f;
	_float3 m_vRotationAxis = { 0.f,0.f,-1.f };
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CWire> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END