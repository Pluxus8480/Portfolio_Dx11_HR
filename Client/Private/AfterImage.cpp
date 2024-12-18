#include "AfterImage.h"
#include "GameInstance.h"
#include "Beat_Manager.h"
#include "Shader.h"

CAfterImage::CAfterImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{
}

CAfterImage::CAfterImage(const CAfterImage& rhs) :
	CGameObject(rhs), m_tDesc(rhs.m_tDesc), m_iRenderPass(rhs.m_iRenderPass)
{
}

CAfterImage::~CAfterImage()
{
	Free();
}

void CAfterImage::PriorityTick(_float fTimeDelta)
{
	m_bActive = false;
}

void CAfterImage::Tick(_float fTimeDelta)
{
	for (auto& fLifeTime : m_fLifeTime)
	{
		if (fLifeTime > 0.f)
		{
			fLifeTime -= fTimeDelta;
			m_bActive = true;
		}
	}
}

void CAfterImage::LateTick(_float fTimeDelta)
{
	if (m_bActive)
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_BLEND_NON_LIT, shared_from_this());
}

HRESULT CAfterImage::Render()
{
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance.lock()->Get_Transform_Float4x4(D3DTS_VIEW));
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance.lock()->Get_Transform_Float4x4(D3DTS_PROJ));


	m_pShaderCom->Bind_RawValue("g_iObjectID", &m_iObjectID, sizeof(_uint));

	for (_int iMeshIdx = 0; iMeshIdx < (_int)(*m_pModelCom)->Get_NumMeshes(); ++iMeshIdx)
	{
		(*m_pModelCom)->Bind_Buffers(iMeshIdx);
		for (_uint iImageIdx = 0; iImageIdx < m_tDesc.iMaxImage; ++iImageIdx)
		{
			if (m_fLifeTime[iImageIdx] < 0.001f)
				continue;
			_float4 vColor = m_tDesc.vColor;
			vColor.w *= m_fLifeTime[iImageIdx] / m_tDesc.fLifeTime;


			m_pShaderCom->Bind_Vector4("g_vColor", &vColor);
			m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_Transforms[iImageIdx]);
			if (m_tDesc.bAnimated)
				m_pShaderCom->Bind_Matrices("g_BoneMatrix", m_BoneMatrices[iImageIdx][iMeshIdx].data(), (_uint)m_BoneMatrices[iImageIdx][iMeshIdx].size());
			m_pShaderCom->Begin(m_iRenderPass);
			(*m_pModelCom)->Draw(iMeshIdx);
		}
	}
	return S_OK;
}

void CAfterImage::SetUp_Matrices(const _float4x4& Transform)
{
	m_Transforms[m_iNumImages] = Transform;
	if (m_tDesc.bAnimated && m_pModelCom)
	{
		for (_int i = 0; i < (_int)(*m_pModelCom)->Get_NumMeshes(); ++i)
		{
			m_BoneMatrices[m_iNumImages][i].clear();
			(*m_pModelCom)->Get_BoneMatrices(m_BoneMatrices[m_iNumImages][i], i);
		}
	}
	m_fLifeTime[m_iNumImages] = m_tDesc.fLifeTime;

	++m_iNumImages;
	m_iNumImages %= m_tDesc.iMaxImage;
}

void CAfterImage::Push_From_Pool(void* pArg)
{
}

void CAfterImage::Dying()
{
}

HRESULT CAfterImage::Initialize_Prototype(const AFTERIMAGE_DESC& desc)
{
	m_tDesc = desc;
	m_iRenderPass = m_tDesc.iRenderPass;
	return S_OK;
}

HRESULT CAfterImage::Initialize(void* Arg)
{
	__super::Initialize(nullptr);

	m_pModelCom = (shared_ptr<CModel>*)Arg;
	//Add_Component(LEVEL_STATIC, m_tDesc.strModelTag, L"Com_Model", (CComponent**)&m_pModel);
	if (m_tDesc.bAnimated)
	{
		m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader")));
		m_BoneMatrices.resize(m_tDesc.iMaxImage);
		_uint iNumMesh = (*m_pModelCom)->Get_NumMeshes();
		for (auto& vecImages : m_BoneMatrices)
		{
			vecImages.resize(iNumMesh);
		}
	}
	else
	{
		m_pShaderCom = static_pointer_cast<CShader>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader")));
	}
	m_Transforms.resize(m_tDesc.iMaxImage);
	m_fLifeTime.resize(m_tDesc.iMaxImage);

	return S_OK;
}

shared_ptr<CAfterImage> CAfterImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const AFTERIMAGE_DESC& desc)
{
	struct MakeSharedEnabler :public CAfterImage
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CAfterImage(pDevice, pContext)
		{}
	};

	shared_ptr<CAfterImage> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(desc)))
	{
		assert(false);
	}


	return pInstance;
}

shared_ptr<CGameObject>  CAfterImage::Clone(void* pArg)
{
	struct MakeSharedEnabler :public CAfterImage
	{
		MakeSharedEnabler(const CAfterImage& rhs) : CAfterImage(rhs)
		{}
	};
	shared_ptr<CAfterImage> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		assert(false);
	}
	return pInstance;
}

void CAfterImage::Free()
{
}
