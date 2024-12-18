#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysXCollider;
class CTexture;
END

BEGIN(Client)

class CGiantRobot2 : public CGimmickBase
{
public:
	static wstring ObjID;


private:
	CGiantRobot2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGiantRobot2(const CGiantRobot2& rhs);
public:
	~CGiantRobot2();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void TakeDamage(const DamageInfo& damageInfo) override;

	virtual void Edit_Attribute() override;
	virtual void Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;
	virtual void Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;

	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider> m_pBodyColliderCom = { nullptr };

	const _float4x4* m_pBoneMat = nullptr;// "r_hand_attach_00"
	_float4x4 m_HandMat = Identity;

	//em8100_event-beam_500 첫 둥장
	//EV1125_90_000_em8100_00 첫 등장 이후 컨테이너 쏘기까지
	// 
	//EV1150_70_000_em8100_00 두번째 등장
	//EV1150_70_001_em8100_00 발싸
	//EV1150_70_002_em8100_00 컨태이너 공격
	//EV1150_70_003_em8100_00 줌인
	//EV1150_70_005_em8100_00 총맞기
	//EV1150_70_006_em8100_00 쫓아가기
	_float4 m_vSecondPos = { 0.f,0.f,0.f,1.f };
	_float4 m_vLook = { 0.f,0.f,1.f,0.f };
	_float4 m_vCollPos{ 0.f,0.f,0.f,1.f };
	_float4 m_vCollScale{ 1.f,1.f,1.f,0.f };
	_bool m_bOn = { false };
	_bool m_bSecPos = false;
	_bool m_bLeft = false;
	_bool m_bChanged = false;
	_float m_fMove = 0.f;
	_float m_fTimer = 30.f;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();





public:
	static shared_ptr<CGiantRobot2> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END