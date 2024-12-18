#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPhysXCollider;
class COctComp;
END

BEGIN(Client)

class CRekka_Generator : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

private:
	CRekka_Generator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_Generator(const CRekka_Generator& rhs);
public:
	virtual ~CRekka_Generator();

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
	shared_ptr<CModel> m_pModel_After = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	shared_ptr<COctComp> m_pOctCom = { nullptr };

	_float3 m_vMeshCenter = {};
	_float m_fCullingRange = { 0 };

private:
	HRESULT Ready_Components(const MISCOBJ_DESC* pDesc);

public:
	static shared_ptr<CRekka_Generator> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END