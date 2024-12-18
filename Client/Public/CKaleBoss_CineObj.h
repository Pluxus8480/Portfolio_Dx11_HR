#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
class CCharacterController;
END

BEGIN(Client)

class CKaleBoss_CineObj : public CJeongGimmicBase, public CBoss_InteractObj
{
public:
	static wstring ObjID;


private:
	CKaleBoss_CineObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKaleBoss_CineObj(const CKaleBoss_CineObj& rhs);

public:
	~CKaleBoss_CineObj();


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;


private:
	void Initialize_KaleBossResource();
	void Initialize_Obj();
	void Adjust_ObjPos();


private:
	vector<shared_ptr<class CJeongGimmicBaseDefault>> m_Objs;
	shared_ptr<CTransform> m_pKaleTransform;
	shared_ptr<CCharacterController> m_pKaleCCt;
	shared_ptr<CTransform> m_pPlayerTransform;
	shared_ptr<CCharacterController> m_pPlayerCCt;
	_bool m_bUp = { false };
	_bool m_bMove = { false };
	_float m_fSpeed = { 40.f };
	_float m_fDownLimitPos = { -150 };
	_float m_fUpLimitPos = { 0 };


public:
	static shared_ptr<CKaleBoss_CineObj> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END