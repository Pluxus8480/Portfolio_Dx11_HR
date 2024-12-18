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

class C808_Companion : public CGameObject
{
public:
	static wstring ObjID;

private:
	C808_Companion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C808_Companion(const C808_Companion& rhs);
public:
	virtual ~C808_Companion();

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
	static shared_ptr<C808_Companion> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	void Play_JustMark_Effect(_uint iStreak);

	void Shop_Animation();
private:

	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);

public:
	_bool Get_Is_CatMode() const { return m_bCatMode; }
	void Set_CatMode(_bool bCatMode) { m_bCatMode = bCatMode; }

	_bool Get_Is_Zero_Pos() const { return m_bZero_Pos; }
	void Set_Zero_Pos(_bool bZeroPos) { m_bZero_Pos = bZeroPos; }

private:
	vector<string> m_vecAnimList;
	_bool m_bRender = true;
	_bool m_bCatMode = false;
	_bool m_bZero_Pos = false;
	_bool m_bAttacking = false;

	_float m_fDistance = 0.f;

	_uint m_iAssist = 0;
	_float3 m_vColor[3]{ {0.f,1.f,1.f}, {0.f,1.f,0.f}, {1.f,0.f,0.f} };
	_float  m_fEmit = 0.f;
	shared_ptr<CGameObject> m_pCat_Mode = nullptr;

	shared_ptr<C808_Flare_Effect> m_pFlareOuter;
	shared_ptr<CFlare_Effect>	  m_pFlareInner;
	shared_ptr<CJustMark_Effect>  m_pJustMark;
	// emission material = 1
private:	// For Effect
	_float4x4 m_matCurrentPos = {};
};

END