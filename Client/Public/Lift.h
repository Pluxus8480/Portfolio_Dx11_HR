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

class CBrake_Part;

class CLift : public CGameObject
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
	CLift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLift(const CLift& rhs);
public:
	virtual ~CLift();

public:
	HRESULT Set_ModelComponent(const string& strPrototypeTag);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	_bool Is_Broken(BRAKE eBrake);

	void Brake_On(BRAKE eBrake);
	void Brake_Crush(BRAKE eBrake);
	void Brake_Broken();
	void Down(_bool bToggle);


private:
	void Initialize_Ui();
	void Check_Ui();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CModel> m_pModelColliderCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = nullptr;
	shared_ptr<class CUi_TramTimer> m_pTimerUi = { nullptr };

#ifdef _DEBUG
	shared_ptr<CShader> m_pDebugShaderCom = { nullptr };
	_bool m_bColliderDraw = false;
#endif
	vector<shared_ptr<CBrake_Part>> m_vecBrakePart;
	_float4x4 m_BrakePartMat[BRAKE_END] = {};
	_float m_fColliderOffset = 0.f;
	_uint m_iPrevBeat = { 0 };

private:
	HRESULT Ready_Components(const ANIMOBJ_DESC* pDesc);

	map<string, _uint> m_mapPartState;
	_bool m_bDown = false;
	_bool m_bBroken[BRAKE_END]{false, false, false, false};

private:
	string strPartBone[6];


public:
	static shared_ptr<CLift> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	//brakeframe_affter00
};

END