#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)
class CMonsterBase;

class CEnemy_Lift : public CGameObject
{
public:
	static wstring ObjID;

	enum POSITION
	{
		LF, LR, RF, RR, POSITION_END
	};


	struct ANIMOBJ_DESC : public CTransform::TRANSFORM_DESC
	{
		_float4x4 worldMatrix = {};
	};

private:
	CEnemy_Lift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Lift(const CEnemy_Lift& rhs);
public:
	virtual ~CEnemy_Lift();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Move(POSITION eType, _int iDirection, const vector<_uint>& vecMonType);

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) {}

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	vector<shared_ptr<CCollider>> m_vecColliders;//발판 높이 == 실제 transform 높이

	vector<vector<shared_ptr<CMonsterBase>>> m_vecMonsterVector;

	vector<_float>  m_vZPos;
	_float m_fZDiff = 0.f;

	vector<_float3> m_vecPositions;

	vector<_int> m_vecDirection;
	vector<_int> m_vecLeft;

	_uint m_iPrevBeat = { 0 };

	_bool m_bBeatChanged = false;


private:
	HRESULT Ready_Components(const ANIMOBJ_DESC* pDesc);

public:
	static shared_ptr<CEnemy_Lift> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END