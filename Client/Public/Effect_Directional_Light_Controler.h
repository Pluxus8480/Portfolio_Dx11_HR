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

class CEffect_Directional_Light_Controler : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CEffect_Directional_Light_Controler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Directional_Light_Controler(const CEffect_Directional_Light_Controler& rhs);

public:
	virtual ~CEffect_Directional_Light_Controler();

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
	static shared_ptr<CEffect_Directional_Light_Controler> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END