#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)
class CLaserPod;

class CLaserCircuit : public CGimmickBase
{
public:
	static wstring ObjID;
	enum COLOR
	{
		RED,
		BLUE,
		PURPLE,
		GREEN,
		WHITE,
		YELLOW,
		COLOR_END,

	};

private:
	CLaserCircuit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLaserCircuit(const CLaserCircuit& rhs);
public:
	~CLaserCircuit();

public:
	virtual void Execute(_bool bExecute = true) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Collision(shared_ptr< CCollider> pOther, shared_ptr< CCollider> pCaller) override;
	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;
	void Turn_Off();
	void Shuffle_Route(_uint iRedNum);
	void Turn_On(_bool bA);
	void Turn_On(_bool bA, _uint iIdx);
	void Turn_On(_bool bA, _uint iRouteIdx, _uint iIdx);
	void Set_PodYPos(_float fPos) { m_fPodYPos = fPos; }
	_bool Is_Executed() const { return m_bExecuted; }
private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CTexture> m_pEmissionTexture = nullptr;

	_int m_iPrevBeatCnt = { 0 };
	vector<COLOR> m_vecEmitColor;
	
	vector<vector<_uint>> m_vecRouteA;
	vector<vector<_float4>> m_vecRouteAPosition;

	vector<vector<_uint>> m_vecRouteB;
	vector<vector<_float4>> m_vecRouteBPosition;

	vector<COLOR> m_vecRouteColor;
	vector<shared_ptr<CLaserPod>> m_LaserPods;
	vector<_float4> m_vecPodPos;
	vector<_float>  m_vecPodInterpolTime;
	vector<_uint>  m_vecPodCurrRoutePos;
	vector<_int>  m_vecPodCurrRouteIdx;

	_int m_iWarnCount = 4;
	_int m_iMoveCount = 4;
	_int m_iWarnFrequency = 4;
	_float m_fPodYPos = 10.f;
	//0 ~ 82;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static shared_ptr<CLaserCircuit> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END