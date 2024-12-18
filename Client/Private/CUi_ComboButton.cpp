#include "CUi_ComboButton.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "iostream"


wstring CUi_ComboButton::ObjID = L"CUi_ComboButton";

CUi_ComboButton::CUi_ComboButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_ComboButton::CUi_ComboButton(const CUi_ComboButton& rhs)
	:CUi_2D(rhs)
{
}

CUi_ComboButton::~CUi_ComboButton()
{
}

HRESULT CUi_ComboButton::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);

	return S_OK;
}

HRESULT CUi_ComboButton::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	m_pTargetTransform = Arg->OwnerTransform;
	m_RelativeDistance = Arg->RelativeDistance;
	Initialize_BackGround((_uint)Arg->LButtonVec.size());
	Initialize_Button(Arg->LButtonVec);
	Initialize_SuccessButton((_uint)Arg->LButtonVec.size());

	m_SuccessVec.resize((_uint)Arg->LButtonVec.size());
	for (size_t i = 0; i < m_SuccessVec.size(); ++i)
	{
		m_SuccessVec[i] = false;
	}


	return S_OK;
}

void CUi_ComboButton::PriorityTick(_float fTimeDelta)
{
}

void CUi_ComboButton::Tick(_float fTimeDelta)
{

}

void CUi_ComboButton::LateTick(_float fTimeDelta)
{
	_float2 Pos = Adjust_RealWorld(m_pTargetTransform->Get_Position(), m_RelativeDistance.x, m_RelativeDistance.y);
	m_pBack->Set_Pos(Pos.x, Pos.y);	
	Adjust_ComboButtonPos();
	Adjust_SuccessButton();
}

void CUi_ComboButton::Set_Success(_bool Success)
{
	if (Success)
	{
		for (size_t i = 0; i < m_SuccessVec.size(); ++i)
		{
			if (m_SuccessVec[i] == false)
			{
				m_SuccessVec[i] = true;
				return;
			}
		}
		Dead();
	}
	else
	{
		for (size_t i = 0; i < m_SuccessVec.size(); ++i)
		{
			m_SuccessVec[i] = false;
		}
	}
}

void CUi_ComboButton::Initialize_BackGround(_uint iCount)
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 170 + iCount * 45;
	Desc.fSizeY = 80;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.fSpeed = 50;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_ComboButtonBg";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pBack = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));

}

void CUi_ComboButton::Initialize_Button(vector<_bool> Vec)
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 40;
	Desc.fSizeY = 40;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_ComboButtonMouse";
	Desc.TextureLevel = LEVEL_STATIC;

	for (_uint i = 0; i < Vec.size(); ++i)
	{
		m_pButton.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	}

	for (size_t i = 0; i < m_pButton.size(); ++i)
	{
		switch (Vec[i])
		{
		case true:
			m_pButton[i]->Set_TextureNum(0);
			break;
		case false:
			m_pButton[i]->Set_TextureNum(1);
			break;
		}	
	}

	Desc.TextureFileName = L"Prototype_Component_Texture_ComboButtonCircle";
	m_pButton.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
}

void CUi_ComboButton::Initialize_SuccessButton(_uint iCount)
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 50;
	Desc.fSizeY = 50;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 3;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_ComboSuccess";
	Desc.TextureLevel = LEVEL_STATIC;

	for (_uint i = 0; i < iCount; ++i)
	{
		m_pSuccessButton.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));
	}
	
	for (auto& iter : m_pSuccessButton)
		iter->Set_Active(false);

}

void CUi_ComboButton::Adjust_ComboButtonPos()
{
	_float3 BackScale = m_pBack->Get_Scale();
	_float3 BackPos = m_pBack->Get_Pos();
	_float3 ButtonScale = m_pButton[0]->Get_Scale();

	if (m_pButton.size() - 1 == 3)
	{
		for (size_t i = 0; i < m_pButton.size(); ++i)
		{
			_float PosX = BackPos.x - (BackScale.x * 0.23f) + i * (ButtonScale.x + 6);
			m_pButton[i]->Set_Pos(PosX, BackPos.y);
		}
	}
	else if (m_pButton.size() - 1 == 4)
	{
		for (size_t i = 0; i < m_pButton.size(); ++i)
		{
			_float PosX = BackPos.x - (BackScale.x * 0.25f) + i * (ButtonScale.x + 6);
			m_pButton[i]->Set_Pos(PosX, BackPos.y);
		}
	}
}

void CUi_ComboButton::Adjust_SuccessButton()
{
	for (size_t i = 0; i < m_pSuccessButton.size(); ++i)
	{
		m_pSuccessButton[i]->Set_Active(m_SuccessVec[i]);
		_float3 Pos = m_pButton[i]->Get_Pos();
		m_pSuccessButton[i]->Set_Pos(Pos.x, Pos.y);
	}
}

void CUi_ComboButton::Set_Active(_bool Active)
{
	m_pBack->Set_Active(Active);

	for(auto& iter : m_pButton)
		iter->Set_Active(Active);

	for (auto& iter : m_pSuccessButton)
		iter->Set_Active(Active);
}

void CUi_ComboButton::Dying()
{
	m_pBack->Dead();
	for (auto& iter : m_pButton)
		iter->Dead();
	for (auto& iter : m_pSuccessButton)
		iter->Dead();
}

shared_ptr<CUi_ComboButton> CUi_ComboButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_ComboButton
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_ComboButton(pDevice, pContext) { }
	};

	shared_ptr<CUi_ComboButton> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_ComboButton::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_ComboButton
	{
		MakeSharedEnabler(const CUi_ComboButton& rhs) : CUi_ComboButton(rhs) { }
	};

	shared_ptr<CUi_ComboButton> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
