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

class CEffect_Filter_Focus_Trigger : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CEffect_Filter_Focus_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Filter_Focus_Trigger(const CEffect_Filter_Focus_Trigger& rhs);

public:
	virtual ~CEffect_Filter_Focus_Trigger();

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
	shared_ptr<CVIBuffer_Rect>	m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CEffect_Filter_Focus_Trigger> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END