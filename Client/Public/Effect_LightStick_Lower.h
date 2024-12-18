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

class CEffect_LightStick_Lower : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CEffect_LightStick_Lower(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_LightStick_Lower(const CEffect_LightStick_Lower& rhs);

public:
	virtual ~CEffect_LightStick_Lower();

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

private:
	HRESULT Ready_Components();

private:
	_float m_fRandRotate = 0.f;
	_float m_fRandScale = 0.f;

private:
	_float m_fUvRandX = 0.f;
	_float m_fUvRandY = 0.f;

private:
	_bool  m_fState = false;

public:
	static shared_ptr<CEffect_LightStick_Lower> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END