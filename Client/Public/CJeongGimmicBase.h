#pragma once
#include "Client_Defines.h"
#include "Effect_Manager.h"
#include "GimmickBase.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"
#include "Bounding_OBB.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "Mesh.h"
#include "MapManager.h"


BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
class CMesh;
END

BEGIN(Client)

class CJeongGimmicBase abstract : public CGimmickBase
{
public:
	using Desc = struct CJeongGimmicBaseInfo : public COMMONOBJ_DESC
	{
		_float3 Pos = {};
		_float3 Scale = {};
		wstring ModelName = L"";
		wstring ShaderName = L"";
		_uint ShaderPass = 0;
		CRenderer::RENDERGROUP eRenderGroup = { CRenderer::RENDER_NONBLEND };
	};
protected:
	CJeongGimmicBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJeongGimmicBase(const CJeongGimmicBase& rhs);

public:
	virtual ~CJeongGimmicBase();


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Bind_ShaderResources();


public:
	_float3 Get_Pos();
	void Set_Pos(_float3 Pos);
	void Set_Pos(_float PosX, _float PosY, _float PosZ);


protected:
	void InitializeBasicComponent(wstring ModelName,
		wstring ShaderName = L"Prototype_Component_Shader_VtxAnimMesh",
		wstring PhysicsName = L"Prototype_Component_PhysXCollider");
	void Use_PhysX();
	void Follow_BonePhysX(const _float4x4 Bone);
	void Initialize_Transform();
	void Argument_Setting(Desc* pDesc);
	void wstringtostring(wstring ObjID, string& GimmicId);
	void Initialize_CookPhysX(wstring ModelName, _float3 Center = { 0, 0, 0 });



protected:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };
	vector<PxRigidStatic*> m_vecPxRigid;
	_uint m_iShaderPassIndex = { 0 };
	_float3 m_vModelCenter = {};
	_float m_fModelRange = {};
	_bool m_bAnim = { true };
	CRenderer::RENDERGROUP m_eRenderGroup = { CRenderer::RENDER_NONBLEND };

public:
	void Free();
};

END


//string GimmicId;
//GimmicId.assign(CKaleBoss_ObjSystem::ObjID.begin(), CKaleBoss_ObjSystem::ObjID.end());
//printf(GimmicId.c_str());
//shared_ptr<CKaleBoss_ObjSystem> Instance = dynamic_pointer_cast<CKaleBoss_ObjSystem>(CGimmickManager::Get_Instance()->Get_GimmckObj(GimmicId));


//CCine_Manager::Get_Instance()->AddLastCallback
//("CS1125", [&]()
//	{
//	}
//);


//void CContainerBreakPannel::Change_Phase(_uint iPhase)
//{
//	switch ((KALEMAP_PHASE)m_iCurPhase)
//	{
//	case Client::CBoss_InteractObj::KALEMAP_PHASE::START:
//		break;
//	case Client::CBoss_InteractObj::KALEMAP_PHASE::FIRSTBATTLECUTSCENE:
//		break;
//	case Client::CBoss_InteractObj::KALEMAP_PHASE::CONTAINERCUTSCENE:
//		break;
//	default:
//		break;
//	}
//}