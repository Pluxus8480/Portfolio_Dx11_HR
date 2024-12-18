#include "Light_Controller.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "PhysxCollider.h"
#include "Collider.h"
#include "Bounding_OBB.h"

#include "Fan_Blade.h"
#include "MapManager.h"

wstring CLight_Controller::ObjID = TEXT("CLight_Controller");
CLight_Controller::CLight_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strLightTag = string(ObjID.begin(), ObjID.end()) + to_string(m_iObjectID);
}

CLight_Controller::CLight_Controller(const CLight_Controller& rhs)
	: CGameObject{ rhs }
{
	m_strLightTag = string(ObjID.begin(), ObjID.end()) + to_string(m_iObjectID);
}

CLight_Controller::~CLight_Controller()
{
	Free();
}



HRESULT CLight_Controller::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CLight_Controller::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
	m_tDesc.eType = LIGHT_DESC::TYPE_POINT;
	m_tDesc.fRange = 3.f;
	m_tDesc.g_fIntensity = 1.f;
	m_tDesc.g_fLightFalloff = 1.f;
	m_tDesc.g_fSpotInnerConeAngle = cos(XM_PIDIV2);
	m_tDesc.g_fSpotOuterConeAngle = cos(XM_PI);
	strcpy(m_tDesc.szName, m_strLightTag.c_str());
	m_tDesc.vAmbient = { 0.f,0.f,0.f,0.f };
	m_tDesc.vDiffuse = { 1.f,1.f,1.f,1.f };
	m_tDesc.vSpecular = { 0.f,0.f,0.f,0.f };

	XMStoreFloat4(&m_tDesc.vPosition, m_pTransformCom->Get_Position());
	XMStoreFloat4(&m_tDesc.vDirection, m_pTransformCom->Get_NormLook());

	auto pGI = m_pGameInstance.lock();
	pGI->Add_Light(&m_tDesc);


	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	return S_OK;
}

void CLight_Controller::PriorityTick(_float fTimeDelta)
{
}

void CLight_Controller::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
#endif
	m_tDesc.g_fSpotInnerConeAngle = XMConvertToRadians(m_fInnerConAngle);
	m_tDesc.g_fSpotOuterConeAngle = XMConvertToRadians(m_fOuterConAngle);
	XMStoreFloat4(&m_tDesc.vPosition, m_pTransformCom->Get_Position());
	XMStoreFloat4(&m_tDesc.vDirection, m_pTransformCom->Get_NormLook());
}

void CLight_Controller::LateTick(_float fTimeDelta)
{
	auto pGI = m_pGameInstance.lock();
	pGI->Rewrite_Description(m_strLightTag, m_tDesc);

#ifdef _DEBUG
	GAMEINSTANCE->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
}

HRESULT CLight_Controller::Render()
{

#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

void CLight_Controller::Edit_Attribute()
{
	auto pLight = GAMEINSTANCE->Return_Light(m_strLightTag);
	_bool bIsOn = pLight->Get_LightState();
	if (ImGui::Checkbox("Light On", &bIsOn))
		pLight->Set_LightState(bIsOn);

	_int iType = m_tDesc.eType;
	ImGui::SliderInt("0:Dir,1:Point,2:Spot", &iType, 0, 2);
	if (iType < 0)
	{
		iType = 0;
	}
	if (iType > 3)
	{
		iType = 2;
	}
	m_tDesc.eType = (LIGHT_DESC::TYPE)iType;
	ImGui::InputFloat("Range", &m_tDesc.fRange);
	ImGui::InputFloat("Intensity", &m_tDesc.g_fIntensity);
	ImGui::InputFloat("FallOff", &m_tDesc.g_fLightFalloff);
	ImGui::InputFloat("InnerCone", &m_fInnerConAngle);
	ImGui::InputFloat("OuterCone", &m_fOuterConAngle);
	ImGui::InputFloat4("Ambient", &m_tDesc.vAmbient.x);
	ImGui::InputFloat4("Diffuse", &m_tDesc.vDiffuse.x);
	ImGui::InputFloat4("Specular", &m_tDesc.vSpecular.x);	
}

void CLight_Controller::Save_Data(ofstream& os)
{
	__super::Save_Data(os);
	write_typed_data(os, m_tDesc.eType);
	write_typed_data(os, m_tDesc.fRange);
	write_typed_data(os, m_tDesc.g_fIntensity);
	write_typed_data(os, m_tDesc.g_fLightFalloff);
	write_typed_data(os, m_tDesc.g_fSpotInnerConeAngle);
	write_typed_data(os, m_tDesc.g_fSpotOuterConeAngle);
	write_typed_data(os, m_tDesc.vAmbient);
	write_typed_data(os, m_tDesc.vDiffuse);
	write_typed_data(os, m_tDesc.vDirection);
	write_typed_data(os, m_tDesc.vPosition);
	write_typed_data(os, m_tDesc.vSpecular);
	//write_typed_data(os, strlen(m_tDesc.szName));
	//os.write(m_tDesc.szName, strlen(m_tDesc.szName));

}

void CLight_Controller::Load_Data(ifstream& is)
{
	__super::Load_Data(is);

	read_typed_data(is, m_tDesc.eType);
	read_typed_data(is, m_tDesc.fRange);
	read_typed_data(is, m_tDesc.g_fIntensity);
	read_typed_data(is, m_tDesc.g_fLightFalloff);
	read_typed_data(is, m_tDesc.g_fSpotInnerConeAngle);
	read_typed_data(is, m_tDesc.g_fSpotOuterConeAngle);
	read_typed_data(is, m_tDesc.vAmbient);
	read_typed_data(is, m_tDesc.vDiffuse);
	read_typed_data(is, m_tDesc.vDirection);
	read_typed_data(is, m_tDesc.vPosition);
	read_typed_data(is, m_tDesc.vSpecular);
	//size_t size;
	//read_typed_data(is, size);
	//is.read(m_tDesc.szName, size);
	//m_strLightTag = m_tDesc.szName;

	m_pTransformCom->Set_Position(m_tDesc.vPosition);
	m_pTransformCom->LookAt(m_pTransformCom->Get_Position() + XMLoadFloat4(&m_tDesc.vDirection));
	m_fInnerConAngle = XMConvertToDegrees((m_tDesc.g_fSpotInnerConeAngle));
	m_fOuterConAngle = XMConvertToDegrees((m_tDesc.g_fSpotOuterConeAngle));
}

HRESULT CLight_Controller::Ready_Components()
{
#ifdef _DEBUG
	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vExtents = { 1.f, 1.f, 2.f };
	obbDesc.vCenter = { 0.f, 0.f, 0.f };
	obbDesc.vRotation = { 0.f, 0.f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Collider_Com"), &obbDesc));

	assert(m_pColliderCom);
#endif
	return S_OK;
}

HRESULT CLight_Controller::Ready_Parts()
{
	return S_OK;
}

HRESULT CLight_Controller::Bind_ShaderResources()
{
	return S_OK;
}

shared_ptr<CLight_Controller> CLight_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CLight_Controller
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLight_Controller(pDevice, pContext)
		{}
	};

	shared_ptr<CLight_Controller> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLight_Controller"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CLight_Controller::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CLight_Controller
	{
		MakeSharedEnabler(const CLight_Controller& rhs) : CLight_Controller(rhs)
		{}
	};

	shared_ptr<CLight_Controller> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLight_Controller"));
		assert(false);
	}

	return pInstance;
}

void CLight_Controller::Free()
{
}
