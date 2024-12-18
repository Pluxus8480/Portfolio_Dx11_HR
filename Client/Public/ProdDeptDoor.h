#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CTexture;
END

BEGIN(Client)

class CProdDeptDoor : public CGimmickBase
{
public:
	static _wstring ObjID;

private:
	enum ELEMENTS { DOOR_L, DOOR_R, ELEM_END };
	enum STATE { OPEN, CLOSE, ST_END };

private:
	CProdDeptDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProdDeptDoor(const CProdDeptDoor& rhs);
public:
	~CProdDeptDoor();

public:
	void Execute_Door();
	virtual void Set_Dead(bool bDead = true) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

private:
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom[ST_END] = { nullptr, };

	vector<shared_ptr<class CPartObject>> m_vecParts;
	//shared_ptr<class CDoorTrigger> m_pDoorTrigger = { nullptr };

	_bool m_bOpen = { true };
	_bool m_isTalkOver = { true };
	_bool m_bTalk = { false };

private:
	HRESULT Ready_Parts(const COMMONOBJ_DESC* pDesc);
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr< CProdDeptDoor> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END