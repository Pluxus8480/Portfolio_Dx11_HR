#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)

class CSteelFrame_Platform : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	enum COL_INDEX { COL_A, COL_B, COL_C, COL_D, COL_END };
	enum ANIM_STATE { IDLE_READY, ANIM_FIRST, IDLE_FIRST, ANIM_LEFTOVER, IDLE_DONE, ANIM_END };

private:
	CSteelFrame_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSteelFrame_Platform(const CSteelFrame_Platform& rhs);
public:
	~CSteelFrame_Platform();

public:
	virtual void Execute(_bool bExecute = true) override;
	virtual void Set_Dead(bool bDead = true) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider[COL_END] = {nullptr,};
	//PxRigidStatic* m_pRigidActors[COL_END] = { nullptr, };

	const _float4x4* m_pBoneMatrix[COL_END] = { nullptr, };

	ANIM_STATE m_eState = { ANIM_END };

	_float4x4 m_ColliderOffset = { Identity };

	/* Magnet */
	shared_ptr<class CBuiltIn_Magnet> m_pAttachedMagnet = { nullptr };
	_float4x4 m_MagnetLocalMat = { Identity };
	_float4x4 m_AttachBone = {};

	_bool m_bBegin = { false };

private:
	HRESULT Ready_Components();
	void Ready_Parts();
	HRESULT Bind_ShaderResources();

	void Update_Animation();
	void Update_MagnetMatrix();

public:
	static shared_ptr<CSteelFrame_Platform> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END