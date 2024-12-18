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

class CMimosa_RingText : public CGameObject, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CMimosa_RingText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMimosa_RingText(const CMimosa_RingText& rhs);
public:
	~CMimosa_RingText();

public:
	virtual void Change_Phase(_uint iPhase) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pBackModelCom = { nullptr };
	shared_ptr<CModel> m_pRoofModelCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom[4] = { nullptr, };

	_float m_fUvFlow = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CMimosa_RingText> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END