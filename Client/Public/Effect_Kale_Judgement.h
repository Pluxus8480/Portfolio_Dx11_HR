#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CEffect_Kale_Judgement : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CEffect_Kale_Judgement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Kale_Judgement(const CEffect_Kale_Judgement& rhs);

public:
	virtual ~CEffect_Kale_Judgement();

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
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Components();

private:
	_float m_fRandRotate = 0.f;
	_float m_fRandScale = 0.f;

public:
	static shared_ptr<CEffect_Kale_Judgement> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END