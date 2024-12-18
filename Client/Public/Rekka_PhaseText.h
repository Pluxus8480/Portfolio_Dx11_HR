#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CRekka_PhaseText : public CGameObject, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CRekka_PhaseText (ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_PhaseText (const CRekka_PhaseText& rhs);
public:
	~CRekka_PhaseText ();

public:
	virtual void Change_Phase(_uint iPhase) override { m_iCurPhase = iPhase; }

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
	shared_ptr<CTexture> m_pTextureCom[(_uint)REKKA_PHASE::PHASE_END] = {nullptr,};

	_float m_fUvFlow = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CRekka_PhaseText > Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END