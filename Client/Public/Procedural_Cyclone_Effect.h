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

class CProcedural_Cyclone_Effect : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CProcedural_Cyclone_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProcedural_Cyclone_Effect(const CProcedural_Cyclone_Effect& rhs);

public:
	virtual ~CProcedural_Cyclone_Effect();

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

	_int m_iNumCycle = 8;
	vector<_float4x4> m_vecTransforms;


private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CProcedural_Cyclone_Effect> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END