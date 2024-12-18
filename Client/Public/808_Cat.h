#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)
class C808_Flare_Effect;
class CFlare_Effect;
class CJustMark_Effect;

class C808_Cat : public CGameObject
{
public:
	static wstring ObjID;

private:
	C808_Cat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C808_Cat(const C808_Cat& rhs);
public:
	virtual ~C808_Cat();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<C808_Cat> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();

private:

	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);
public:
	_bool Get_Is_CatMode() const { return m_bCatMode; }
	void Set_CatMode(_bool bCatMode) { m_bCatMode = bCatMode; }
private:
	vector<string> m_vecAnimList;
	_bool m_bRender = true;

	_bool m_bCatMode = false;

	_float m_fDistance = 0.f;



private:	// For Effect
	_float4x4 m_matCurrentPos = {};
};

END