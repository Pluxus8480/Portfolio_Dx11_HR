#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CRekka_GenCover : public CGameObject, public CBoss_InteractObj
{
public:
	static wstring ObjID;
private:
	CRekka_GenCover(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_GenCover(const CRekka_GenCover& rhs);
public:
	~CRekka_GenCover();

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
	shared_ptr<CModel> m_pCrushedModel = { nullptr };
	
	_bool m_bCrushed = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CRekka_GenCover> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END