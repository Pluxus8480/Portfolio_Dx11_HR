#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)

class CKale_Antenna : public CJeongGimmicBase, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CKale_Antenna(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKale_Antenna(const CKale_Antenna& rhs);
public:
	~CKale_Antenna();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual HRESULT Render() override;


protected:
	virtual void Change_Phase(_uint iPhase);


private:
	void Ready_Component();


private:
	_int m_iCurAnimIndex = { 0 };
	_float m_fAngle = { 0 };
	

public:
	static shared_ptr<CKale_Antenna> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END