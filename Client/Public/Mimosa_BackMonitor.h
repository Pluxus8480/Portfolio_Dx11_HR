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

class CMimosa_BackMonitor : public CGameObject, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CMimosa_BackMonitor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMimosa_BackMonitor(const CMimosa_BackMonitor& rhs);
public:
	~CMimosa_BackMonitor();

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
	shared_ptr<CModel> m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_float m_fTimeAcc = 0.f;
	_float m_fTimeAccMax = 5.f;

public:
	static shared_ptr<CMimosa_BackMonitor> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END