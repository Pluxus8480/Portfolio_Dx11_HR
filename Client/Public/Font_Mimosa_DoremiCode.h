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

class CFont_Mimosa_DoremiCode : public CEffect_Base
{
public:
	static wstring ObjID;

private:
	CFont_Mimosa_DoremiCode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFont_Mimosa_DoremiCode(const CFont_Mimosa_DoremiCode& rhs);

public:
	virtual ~CFont_Mimosa_DoremiCode();

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

	vector<shared_ptr<CModel>> m_vecModelCom;
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CFont_Mimosa_DoremiCode> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END