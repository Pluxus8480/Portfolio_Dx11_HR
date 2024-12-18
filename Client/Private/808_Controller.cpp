#include "808_Controller.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "GameManager.h"
#include "Model.h"
wstring C808_Controller::ObjID = TEXT("C808_Controller");

C808_Controller::C808_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

C808_Controller::C808_Controller(const C808_Controller& rhs)
	: CGameObject(rhs)
{
}

C808_Controller::~C808_Controller()
{
	Free();
}

HRESULT C808_Controller::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT C808_Controller::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_p808_Cat = static_pointer_cast<C808_Cat>(GAMEINSTANCE->Add_Clone_Return(GAMEMANAGER->Get_Current_Level(), TEXT("Layer_808_Cat"), C808_Cat::ObjID, &TransformDesc));
	m_p808_Companion = static_pointer_cast<C808_Companion>(GAMEINSTANCE->Add_Clone_Return(GAMEMANAGER->Get_Current_Level(), TEXT("Layer_808_Companion"), C808_Companion::ObjID, &TransformDesc));

	return S_OK;
}

void C808_Controller::PriorityTick(_float fTimeDelta)
{
}

void C808_Controller::Tick(_float fTimeDelta)
{


}

void C808_Controller::LateTick(_float fTimeDelta)
{
	
}



HRESULT C808_Controller::Ready_Components()
{


	return S_OK;
}

shared_ptr<C808_Controller> C808_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public C808_Controller
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : C808_Controller(pDevice, pContext)
		{}
	};


	shared_ptr<C808_Controller> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : C808_Controller"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> C808_Controller::Clone(void* pArg)
{
	struct MakeSharedEnabler :public C808_Controller
	{
		MakeSharedEnabler(const C808_Controller& rhs) : C808_Controller(rhs)
		{}
	};


	shared_ptr<C808_Controller> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : C808_Controller"));
		assert(false);
	}

	return pInstance;
}

void C808_Controller::Free()
{
}

void C808_Controller::Active_808_Sphere_Mode()
{
	m_p808_Companion->Set_CatMode(false);
	m_p808_Cat->Set_CatMode(false);

}

void C808_Controller::Active_808_Cat_Mode()
{
	m_p808_Companion->Set_CatMode(true);
	m_p808_Cat->Set_CatMode(true);
}

void C808_Controller::Active_808_Shop_Animation()
{
	shared_ptr<CModel> pModel = static_pointer_cast<CModel>(m_p808_Companion->Find_Component(TEXT("Com_Model")));

	pModel->Set_AnimIndex("am0020_action-cp_040",false);
}

void C808_Controller::Active_808_Shop_Animation_End()
{
	shared_ptr<CModel> pModel = static_pointer_cast<CModel>(m_p808_Cat->Find_Component(TEXT("Com_Model")));

	pModel->Set_AnimIndex("am0000_action-cp_042", false);
}
