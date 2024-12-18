#include "Font_Effect_Wvuong.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Effect_Union.h"

wstring CFont_Effect_Wvuong::ObjID = TEXT("CFont_Effect_Wvuong");

CFont_Effect_Wvuong::CFont_Effect_Wvuong(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base{ pDevice, pContext }
{
	s_PrototypeTags.insert(CFont_Effect_Wvuong::ObjID);
}

CFont_Effect_Wvuong::CFont_Effect_Wvuong(const CFont_Effect_Wvuong& rhs)
	: CEffect_Base(rhs)
{
}

CFont_Effect_Wvuong::~CFont_Effect_Wvuong()
{
	Free();
}

HRESULT CFont_Effect_Wvuong::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_T_VFX_tk_word_tc_partner"), TEXT("../Bin/Resources/Effect/Font/Partner/T_VFX_tk_word_tc_partner_%d.png"), 3));
	//m_List_ResourceInfo.push_back(make_shared<RES_TEXTURE>(TEXT("Prototype_Component_Effect_Smack_Scratch"), TEXT("../Bin/Resources/Effect/Smack/T_VFX_tk_scratch_impact_tc_04.png"), 1));

	return S_OK;
}

HRESULT CFont_Effect_Wvuong::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(24.f, 48.f, 1.f));

	_float4 vPos = _float4(0.f, 7.f, 0.f, 1.f);
	m_fEffectUvFlowYSpeed = 0.f;
	m_fEffectUvFlowXSpeed = 0.f;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPos));

	m_eEffectType = EFFECT_TYPE::EFFECT_TEXTURE;

	m_vEffectColorPresets.push_back(_float4{ 1.f,0.0f,0.0f,1.f });
	m_vEffectColorPresets.push_back(_float4{ 0.0f, 0.0f,1.0f, 1.f });
	m_vEffectColorPresets.push_back(_float4{ 1.0f,1.0f,1.0f,1.f });
	m_vEffectBaseColor = _float4{ 1.0f,1.0f,1.0f,1.f };
	SetRootMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

void CFont_Effect_Wvuong::PriorityTick(_float fTimeDelta)
{


}

void CFont_Effect_Wvuong::Tick(_float fTimeDelta)
{

	EffectTick(fTimeDelta);
}

void CFont_Effect_Wvuong::LateTick(_float fTimeDelta)
{
	if (GetEffectIsView()) {
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());
		//m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_GLOW, shared_from_this());
	}
}

HRESULT CFont_Effect_Wvuong::Render()
{
	//_float fTimeRatio = m_fEffectTimeAcc / m_fEffectTimeMax;
	_float4x4 ViewMat;

	XMStoreFloat4x4(&ViewMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));

	_float4x4 ProjMat;

	XMStoreFloat4x4(&ProjMat, m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	_float4x4 Temp;
	_float fScale = min(pow(m_fEffectTimePercent * 4.f, 3.f) + 0.5f, 1.f);
	XMStoreFloat4x4(&Temp, XMMatrixScaling(fScale, fScale, fScale) * m_pTransformCom->Get_WorldMatrix_XMMat());

	m_pTransformCom->Go_Up(GAMEINSTANCE->Get_TimeDelta(TEXT("Timer_60")) * 0.6f);

	if (m_pParentWorld != nullptr) {
		_float4x4 ParentMultipleWorld;
		XMStoreFloat4x4(&ParentMultipleWorld, XMMatrixMultiply(XMLoadFloat4x4(&Temp), XMLoadFloat4x4(m_pParentWorld)));

		if (m_pParentUnion.lock()->GetBillboard()) {
			ParentMultipleWorld = GAMEINSTANCE->Matrix_BillBoard(ParentMultipleWorld);
		}

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &ParentMultipleWorld)))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Temp)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	EffectParamBind(m_pShaderCom);

	m_pShaderCom->Bind_Float("g_fDissolveThreshold", max((m_fEffectTimePercent - 0.5f) * 2.f, 0.f));
	//m_pShaderCom->Bind_Float("g_fDissolveThreshold", 0.f);

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Noise_Effect_Texture_01", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin((_uint)EFFECT_TEX_PASS::UVFlow)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CFont_Effect_Wvuong::EffectStartCallBack()
{
}

void CFont_Effect_Wvuong::EffectEndCallBack()
{

}

void CFont_Effect_Wvuong::EffectSoftEnd()
{
}

void CFont_Effect_Wvuong::EffectHardEnd()
{
}

HRESULT CFont_Effect_Wvuong::Ready_Components()
{
	/* For.Com_Shader */
	m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffectPosTex"), TEXT("Com_Shader")));

	m_pVIBufferCom = static_pointer_cast<CVIBuffer_Rect>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer")));

	m_pNoiseTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Smack_Scratch"), TEXT("Com_Noise_Texture")));

	m_pTextureCom = static_pointer_cast<CTexture>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_T_VFX_tk_word_tc_partner"), TEXT("Com_Texture")));

	if (!m_pShaderCom || !m_pTextureCom || !m_pNoiseTextureCom)
		assert(false);

	return S_OK;
}

shared_ptr<CFont_Effect_Wvuong> CFont_Effect_Wvuong::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler :public CFont_Effect_Wvuong
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CFont_Effect_Wvuong(pDevice, pContext)
		{}
	};


	shared_ptr<CFont_Effect_Wvuong> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFont_Effect_Wvuong"));
		assert(false);
	}

	return pInstance;
}


shared_ptr<CGameObject> CFont_Effect_Wvuong::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CFont_Effect_Wvuong
	{
		MakeSharedEnabler(const CFont_Effect_Wvuong& rhs) : CFont_Effect_Wvuong(rhs) {}
	};

	shared_ptr<CFont_Effect_Wvuong> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFont_Effect_Wvuong"));
		assert(false);
	}

	return pInstance;
}

void CFont_Effect_Wvuong::Free()
{
}