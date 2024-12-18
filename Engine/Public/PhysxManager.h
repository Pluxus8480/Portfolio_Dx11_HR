#pragma once
#include "Engine_Defines.h"

class CharacterControllerHitReport : public PxUserControllerHitReport {
public:
	PxVec3 lookDirection = { 0.f,0.f,0.f }; // 캐릭터의 바라보는 방향
	bool rightHit = false;
	bool leftHit = false;
	bool sideHit = false;

	CharacterControllerHitReport() {}

	void SetLookDirection(const PxVec3& direction) {
		lookDirection = direction;
	}

	void onShapeHit(const PxControllerShapeHit& hit) override;

	bool GetRightHit() const {
		return rightHit;
	}

	bool GetLeftHit() const {
		return leftHit;
	}

	bool GetSideHit() const {
		return sideHit;
	}

	void onControllerHit(const PxControllersHit& hit) override {}
	void onObstacleHit(const PxControllerObstacleHit& hit) override {}
};




BEGIN(Engine)

class CPhysxManager 
{
private:
	CPhysxManager();
public:
	virtual ~CPhysxManager();

public:
	HRESULT Initialize();

	HRESULT Simulate(_float fTimeDelta);

	PxScene* Get_Scene() const { return m_pScene; }
	PxControllerManager* Get_CCT_Manager() const { return m_pCCTManager; }
	PxPhysics* Get_Physics() const { return m_pPhysics; }
	PxFoundation* Get_Foundation() const { return m_pFoundation; }
	HRESULT Add_Actor(PxActor* pActor, _fmatrix Transform = XMMatrixIdentity());
	HRESULT Remove_Actor(PxActor* pActor);
	// Dynamic, Static 정하기, 시작 위치 정하기...등등
	//HRESULT Add_CCT();
	HRESULT Add_Shape(const wstring& strTag, const vector<_float3>& vecPosition, const vector<_uint>& vecIndicies);
	//HRESULT Add_Shape(const wstring& strTag, const vector<PxVec3>& vecPosition, const vector<_uint>& vecIndicies);
	PxShape* Get_Shape(const wstring& strTag);

	bool Sweep(PxTransform& StartTransform, PxVec3& vDirection, _float fDistance, _float fRadius, PxSweepBuffer& HitOut);
	void Simulate_Physx(_bool bSim = true) { m_bSimulate = bSim; }
	//PxCudaContextManager* Get_CudaContextManager() { return m_pCudaContextManager; }

private:


private:
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;
	PxFoundation* m_pFoundation = NULL;

	PxPhysics* m_pPhysics = NULL;


	PxDefaultCpuDispatcher* m_pDispatcher = NULL;
	
	PxScene* m_pScene = NULL;

	PxMaterial* m_pMaterial = NULL;

	PxControllerManager* m_pCCTManager = nullptr;
	//PxCudaContextManager* m_pCudaContextManager = nullptr;

	map<wstring, PxShape*> m_Shapes;

	_bool m_bSimulate = true;
	_float m_fTimer = 0.f;

#ifdef _DEBUG
	physx::PxPvdTransport* mTransport = nullptr;
	physx::PxPvd* mPvd = nullptr;
#endif

public:
	static shared_ptr<CPhysxManager> Create();
	void Free();
};

END