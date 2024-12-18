#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPhysXCollider;
END

BEGIN(Client)

class CBrake_Part : public CGameObject
{
public:
	static wstring ObjID;

	struct ANIMOBJ_DESC : public CTransform::TRANSFORM_DESC
	{
		_float4x4 worldMatrix = {};
	};

	enum BRAKE
	{
		FRONT_LEFT,
		FRONT_RIGHT,
		BACK_LEFT,
		BACK_RIGHT,
		BRAKE_END,
	};

private:
	CBrake_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBrake_Part(const CBrake_Part& rhs);
public:
	virtual ~CBrake_Part();

public:
	HRESULT Set_ModelComponent(const string& strPrototypeTag);
	void Set_Parent(_float4x4* pParent)
	{
		m_pParent = pParent;
	}
	void Break() { m_bBroken = true; }
	_bool Is_Broken() const { return m_bBroken; }
	_bool Is_On() const { return m_bOn; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	
	virtual void Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<CCollider> pOther, shared_ptr<CCollider> pCaller) override;
	void Brake_On();
	void Create_Ui(BRAKE Pos, const _float4x4* BonePtr);

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CShader> m_pNonAnimShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CModel> m_pNonAnimModelCom = { nullptr };

	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_uint m_iPrevBeat = { 0 };
	vector<_uint> m_MeshIdx;

	_bool m_bBroken = false;
	_bool m_bOn = false;

	_int m_iHp = 7;

	_float4x4* m_pParent = nullptr;
	_float4x4 m_CombMat = Identity;
	_bool m_bRight = false;
	_bool m_bSoundPlayed = false;

private:
	shared_ptr<class CUi_TramGauge> m_TramUi;
	shared_ptr<class CUi_TramMacaronSign> m_MacaronUi;

private:
	HRESULT Ready_Components(const ANIMOBJ_DESC* pDesc);


public:
	static shared_ptr<CBrake_Part> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	//brakeframe_affter00
};

END