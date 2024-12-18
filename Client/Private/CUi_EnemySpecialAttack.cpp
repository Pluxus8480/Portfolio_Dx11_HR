#include "CUi_EnemySpecialAttack.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_Default.h"
#include "Beat_Manager.h"
#include "iostream"
#include "CUi_EnemySpecialAttackPart.h"

wstring CUi_EnemySpecialAttack::ObjID = L"CUi_EnemySpecialAttack";

CUi_EnemySpecialAttack::CUi_EnemySpecialAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_EnemySpecialAttack::CUi_EnemySpecialAttack(const CUi_EnemySpecialAttack& rhs)
	:CUi_2D(rhs)
{
}

CUi_EnemySpecialAttack::~CUi_EnemySpecialAttack()
{
}

HRESULT CUi_EnemySpecialAttack::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_Default::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_EnemySpecialAttackPart::ObjID);
	
	return S_OK;
}

HRESULT CUi_EnemySpecialAttack::Initialize(void* pArg)
{
	Initialize_Transform();
	Desc* Arg = reinterpret_cast<Desc*>(pArg);


	m_iParryCount = Arg->iParryCount;
	m_pTargetTransform = Arg->TargetTransform;
	m_fDistance = Arg->fDistance;

	Initialize_Board();
	Initialize_Parry(Arg->bParry);
	Initialize_CreateBeatTime(Arg->fBeatGap);
	Initialize_String();
	Create_Part(Arg->RealAttackGapBeat);

	return S_OK;
}

void CUi_EnemySpecialAttack::PriorityTick(_float fTimeDelta)
{
}

void CUi_EnemySpecialAttack::Tick(_float fTimeDelta)
{
	Adjust_ParryActive();
	Increase_ParrySize(fTimeDelta);
	Decrease_ParrySize(fTimeDelta);
	Adjust_StringSize(fTimeDelta);
	Chase_Target();
}

void CUi_EnemySpecialAttack::LateTick(_float fTimeDelta)
{
	Adjust_Pos();
}

void CUi_EnemySpecialAttack::Initialize_Board()
{
	CUi_Default::Desc Desc;
	Desc.eRenderType = RenderType::BackGround;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_ParryBoard";
	Desc.TextureLevel = LEVEL_STATIC;

	if (m_iParryCount == 2)
	{
		Desc.fSizeX = 200;
		Desc.fSizeY = 100;
		Desc.fX = 0;
		Desc.fY = 0;
		Desc.fZ = 0.6f;
		m_pBoard = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	}
	else if (m_iParryCount <= 5)
	{
		Desc.fSizeX = 400;
		Desc.fSizeY = 100;
		Desc.fX = 0;
		Desc.fY = 0;
		Desc.fZ = 0.6f;
		m_pBoard = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	}
	else if (m_iParryCount <= 9)
	{
		Desc.fSizeX = 400 + (m_iParryCount - 5) * 65;
		Desc.fSizeY = 100;
		Desc.fX = 0;
		Desc.fY = 0;
		Desc.fZ = 0.6f;
		m_pBoard = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
	}
	else
	{
		assert(false);
	}

	if (m_iParryCount - 2 > 3)
	{
		m_pBoard->Set_TextureNum(3);
	}
	else
	{
		m_pBoard->Set_TextureNum(m_iParryCount - 2);
	}
}

void CUi_EnemySpecialAttack::Initialize_Parry(vector<_bool>& ParryVec)
{
	CUi_Default::Desc Desc;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 0;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_Parry";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.fSizeX = 10;
	Desc.fSizeY = 10;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	for (_uint i = 0; i < m_iParryCount; i++)
	{
		m_pParryVec.emplace_back(static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
			->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

		if(!ParryVec[i])
			m_pParryVec[i]->Set_TextureNum(1);
	}

	for (auto& iter : m_pParryVec)
		iter->Set_Active(false);

}

void CUi_EnemySpecialAttack::Initialize_CreateBeatTime(vector<_float>& ParryGap)
{
	_float Sum_BeatCount = CBeat_Manager::Get_Instance()->Get_BeatCount();
	m_iParryCreateTimeVec.reserve(m_iParryCount);
	for (_uint i = 0; i < m_iParryCount; ++i)
	{
		Sum_BeatCount += ParryGap[i];
		m_iParryCreateTimeVec.emplace_back(Sum_BeatCount);
	}
}

void CUi_EnemySpecialAttack::Initialize_String()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 250;
	Desc.fSizeY = 100;
	Desc.fX = 0;
	Desc.fY = 0;
	Desc.fZ = 0.6f;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 5;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Default;
	Desc.TextureFileName = L"Prototype_Component_Texture_ParryString";
	Desc.TextureLevel = LEVEL_STATIC;
	m_pString = static_pointer_cast<CUi_Default>(CUi_Mgr::Get_Instance()
		->Add_Clone_Return(CUi_Default::ObjID, m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc));
}

void CUi_EnemySpecialAttack::Chase_Target()
{
	if (m_pTargetTransform)
	{
		_float2 BoardPos = Adjust_RealWorld(m_pTargetTransform->Get_Position(), m_fDistance.x, m_fDistance.y);
		m_pBoard->Set_Pos(BoardPos.x, BoardPos.y);
	}
}

void CUi_EnemySpecialAttack::Adjust_Pos()
{
	_float PosY = m_pBoard->Get_Pos().y + m_pBoard->Get_Scale().y * 0.2f;
	_float PosX = m_pBoard->Get_Pos().x;
	m_pString->Set_Pos(PosX, m_pBoard->Get_Pos().y + m_pBoard->Get_Scale().y);
	switch (m_iParryCount)
	{
	case 2:
		PosX = m_pBoard->Get_Pos().x - 15;
		break;
	case 3:
		PosX = m_pBoard->Get_Pos().x - 40;
		break;
	case 4:
		PosX = m_pBoard->Get_Pos().x - 65;
		break;
	case 5:
		PosX = m_pBoard->Get_Pos().x - 95;
		break;
	case 6:
		PosX = m_pBoard->Get_Pos().x - 120;
		break;
	case 7:
		PosX = m_pBoard->Get_Pos().x - 150;
		break;
	case 8:
		PosX = m_pBoard->Get_Pos().x - 180;
		break;
	case 9:
		PosX = m_pBoard->Get_Pos().x - 210;
		break;
	default:
		assert(false);
		break;
	}

	for (size_t i = 0; i < m_pParryVec.size(); ++i)
	{
		m_pParryVec[i]->Set_Pos(PosX + (60 * i), PosY);
	}
}

void CUi_EnemySpecialAttack::Adjust_ParryActive()
{
	_float BeatCount = CBeat_Manager::Get_Instance()->Get_BeatCount();
	_float Beat = CBeat_Manager::Get_Instance()->Get_Beat();
	_float Timer = CBeat_Manager::Get_Instance()->Get_Timer();
	_float Sum = BeatCount + Timer / Beat;
	for (size_t i = 0; i < m_iParryCreateTimeVec.size(); ++i)
	{
		if (m_iParryCreateTimeVec[i] <= Sum)
		{
			if (!m_pParryVec[i]->Get_Active())
			{
				m_pParryVec[i]->Set_Active(true);
				m_bStringSize = true;
			}
			
		}
	}
}

void CUi_EnemySpecialAttack::Adjust_StringSize(_float fTimeDelta)
{
	if (m_bStringSize)
	{
		m_pString->Add_Scale(fTimeDelta * 600, fTimeDelta * 250);
	}
	else
	{
		m_pString->Add_Scale(-fTimeDelta * 600, -fTimeDelta * 250);
		if (m_pString->Get_Scale().x < m_StringOriginSize.x)
		{
			m_pString->Set_Scale(m_StringOriginSize.x, m_StringOriginSize.y);
		}
	}
}

void CUi_EnemySpecialAttack::Increase_ParrySize(_float fTimeDelta)
{
	_uint Count = { 0 };
	for (auto& iter : m_pParryVec)
	{
		if (m_bIncrese[Count])
		{
			if (iter->Get_Active())
			{
				iter->Add_Scale(fTimeDelta * 800, fTimeDelta * 800);

				if (iter->Get_TextureNum() == 0)
				{
					if (m_iOriginParrySize.x + 50 < iter->Get_Scale().x)
					{
						m_bIncrese[Count] = false;
						m_bStringSize = false;
					}
				}
				else
				{
					if (m_iOriginNonParrySize.x + 30 < iter->Get_Scale().x)
					{
						m_bIncrese[Count] = false;
						m_bStringSize = false;
					}
				}
			}
		}
		++Count;
	}
}

void CUi_EnemySpecialAttack::Decrease_ParrySize(_float fTimeDelta)
{
	for (size_t i = 0; i < m_pParryVec.size(); ++i)
	{
		if (!m_bIncrese[i])
		{
			m_pParryVec[i]->Add_Scale(-fTimeDelta * 800, -fTimeDelta * 800);
			if (m_pParryVec[i]->Get_TextureNum() == 0)
			{
				if (m_pParryVec[i]->Get_Scale().x < m_iOriginParrySize.x)
				{
					m_pParryVec[i]->Set_Scale(m_iOriginParrySize.x, m_iOriginParrySize.y);
				}
			}
			else
			{
				if (m_pParryVec[i]->Get_Scale().x < m_iOriginNonParrySize.x)
				{
					m_pParryVec[i]->Set_Scale(m_iOriginNonParrySize.x, m_iOriginNonParrySize.y);
				}
			}
		}
	}
}

void CUi_EnemySpecialAttack::Dying()
{
	m_pBoard->Dead();

	for (auto& iter : m_pParryVec)
		iter->Dead();

	if(m_pString)
		m_pString->Dead();

	m_pTargetTransform = nullptr;
}

void CUi_EnemySpecialAttack::Create_Part(_float RealAttackBeatGap)
{
	CUi_EnemySpecialAttackPart::Desc Desc;
	Desc.ActiveBeat = CBeat_Manager::Get_Instance()->Get_BeatCount() + BeatPercent() + RealAttackBeatGap - 2;
	Desc.eRenderType = RenderType::Priority;
	Desc.iPriority = 3;
	CUi_Mgr::Get_Instance()->Add_Clone(CUi_EnemySpecialAttackPart::ObjID, 
		m_iLevelIndex, L"CUi", CUi_EnemySpecialAttackPart::ObjID, &Desc);
}

shared_ptr<CUi_EnemySpecialAttack> CUi_EnemySpecialAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_EnemySpecialAttack
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_EnemySpecialAttack(pDevice, pContext) { }
	};

	shared_ptr<CUi_EnemySpecialAttack> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_EnemySpecialAttack::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_EnemySpecialAttack
	{
		MakeSharedEnabler(const CUi_EnemySpecialAttack& rhs) : CUi_EnemySpecialAttack(rhs) { }
	};

	shared_ptr<CUi_EnemySpecialAttack> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
