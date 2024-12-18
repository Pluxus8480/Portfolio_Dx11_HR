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

class CEffect_Mimosa_Barrier : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CEffect_Mimosa_Barrier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Mimosa_Barrier(const CEffect_Mimosa_Barrier& rhs);

public:
	virtual ~CEffect_Mimosa_Barrier();

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
	shared_ptr<CTexture> m_pNoiseTextureCom = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CEffect_Mimosa_Barrier> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END