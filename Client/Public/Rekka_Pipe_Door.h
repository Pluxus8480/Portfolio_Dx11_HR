#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"


BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
END

BEGIN(Client)

class CRekka_Pipe_Door : public CGameObject
{
public:
	static wstring ObjID;

private:
	CRekka_Pipe_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_Pipe_Door(const CRekka_Pipe_Door& rhs);
public:
	virtual ~CRekka_Pipe_Door();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components(const MISCOBJ_DESC* pDesc);
private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };

public:
	void OpenDoor();

private:
	_bool m_doingOpening = false;

private:
	const _float m_fTargetTimer_C = 2.f;
	_float m_fTimer = m_fTargetTimer_C;


public:
	static shared_ptr<CRekka_Pipe_Door> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END