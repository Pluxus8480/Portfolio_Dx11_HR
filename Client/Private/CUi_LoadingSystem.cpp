#include "CUi_LoadingSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_LoadingSystem::ObjID = L"CUi_LoadingSystem";

CUi_LoadingSystem::CUi_LoadingSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Canvas(pDevice, pContext)
{
}

CUi_LoadingSystem::CUi_LoadingSystem(const CUi_LoadingSystem& rhs)
	:CUi_Canvas(rhs)
{
}

CUi_LoadingSystem::~CUi_LoadingSystem()
{

}

HRESULT CUi_LoadingSystem::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_LoadingCat", L"../Bin/Resources/Textures/Ui/Loading/T_loading_screen_808.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_LoadingNote1", L"../Bin/Resources/Textures/Ui/Loading/T_loading_screen_note_1.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_LoadingNote2", L"../Bin/Resources/Textures/Ui/Loading/T_loading_screen_note_2.png")
	);

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_LoadingNote3", L"../Bin/Resources/Textures/Ui/Loading/T_loading_screen_note_3.png")
	);

	return S_OK;
}

HRESULT CUi_LoadingSystem::Initialize(void* pArg)
{
	Initialize_Transform();
	Create_LoadingCat();
	Create_LoadingNote();

	m_pLoadingNoteA->SetVisible(false);
	m_pLoadingNoteB->SetVisible(false);
	m_pLoadingNoteC->SetVisible(false);
	m_pLoadingNoteD->SetVisible(false);

	return S_OK;
}

void CUi_LoadingSystem::Tick(_float fTimeDelta)
{   
	m_fElapsedTime += fTimeDelta;

	if (!m_bAllNotesVisible)
	{
		if (m_fElapsedTime >= m_fNoteIntervals[m_iCurrentNote])
		{
			ShowCurrentNote();  
			m_iCurrentNote++;   

			if (m_iCurrentNote >= 4)
			{
				m_bAllNotesVisible = true;  
				m_fElapsedTime = 0.0f;      
			}
		}
	}
	else
	{
		if (m_fElapsedTime >= m_fDisappearDelay)
		{
			HideAllNotes();  
			m_bAllNotesVisible = false; 
			m_iCurrentNote = 0;         
			m_fElapsedTime = 0.0f;      
		}
	}
}

void CUi_LoadingSystem::Create_LoadingCat()
{
	CUi_Sprite::Desc    CatDesc{};
	CatDesc.fX = -420.f;
	CatDesc.fY = -160.f;
	CatDesc.fZ = 0.7f;
	CatDesc.eRenderType = RenderType::Normal;
	CatDesc.iPriority = 0;
	CatDesc.fSizeX = 342;
	CatDesc.fSizeY = 346;
	CatDesc.fSizeZ = 1.f;
	CatDesc.fSpeed = 0.f;
	CatDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	CatDesc.TextureFileName = L"Prototype_Component_Texture_LoadingCat";
	CatDesc.TextureLevel = LEVEL_STATIC;
	CatDesc.iTextureNum = 0;
	CatDesc.ShaderPass = ShaderPass::Default;
	m_pLoadingCat = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, LEVEL_LOADING, L"CUi", CUi_Sprite::ObjID, &CatDesc));
}

void CUi_LoadingSystem::Create_LoadingNote()
{
	CUi_Sprite::Desc NoteADesc{};
	NoteADesc.fX = -300.f;
	NoteADesc.fY = -125.f;
	NoteADesc.fZ = 0.7f;
	NoteADesc.eRenderType = RenderType::Normal;
	NoteADesc.iPriority = 0;
	NoteADesc.fSizeX = 80.f;
	NoteADesc.fSizeY = 110.f;
	NoteADesc.fSizeZ = 1.f;
	NoteADesc.fSpeed = 0.f;
	NoteADesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	NoteADesc.TextureFileName = L"Prototype_Component_Texture_LoadingNote1";
	NoteADesc.TextureLevel = LEVEL_STATIC;
	NoteADesc.iTextureNum = 0;
	NoteADesc.ShaderPass = ShaderPass::Default;
	m_pLoadingNoteA = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, LEVEL_LOADING, L"CUi", CUi_Sprite::ObjID, &NoteADesc));

	CUi_Sprite::Desc NoteBDesc = NoteADesc;
	NoteBDesc.fX = -210.f;
	NoteBDesc.fY = -125.f;
	NoteBDesc.TextureFileName = L"Prototype_Component_Texture_LoadingNote2";
	m_pLoadingNoteB = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, LEVEL_LOADING, L"CUi", CUi_Sprite::ObjID, &NoteBDesc));

	CUi_Sprite::Desc NoteCDesc = NoteBDesc;
	NoteCDesc.fX = -255.f;
	NoteCDesc.fY = -105.f;
	NoteCDesc.TextureFileName = L"Prototype_Component_Texture_LoadingNote3";
	m_pLoadingNoteC = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, LEVEL_LOADING, L"CUi", CUi_Sprite::ObjID, &NoteCDesc));

	CUi_Sprite::Desc NoteDDesc = NoteCDesc;
	NoteDDesc.fX = -155.f;
	NoteDDesc.fY = -95.f;
	NoteDDesc.TextureFileName = L"Prototype_Component_Texture_LoadingNote1";
	m_pLoadingNoteD = static_pointer_cast<CUi_Sprite>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_Sprite::ObjID, LEVEL_LOADING, L"CUi", CUi_Sprite::ObjID, &NoteDDesc));
}

void CUi_LoadingSystem::ShowCurrentNote()
{
	switch (m_iCurrentNote)
	{
	case 0: m_pLoadingNoteA->SetVisible(true); break;
	case 1: m_pLoadingNoteC->SetVisible(true); break;
	case 2: m_pLoadingNoteB->SetVisible(true); break;
	case 3: m_pLoadingNoteD->SetVisible(true); break;
	}
}

void CUi_LoadingSystem::HideAllNotes()
{
	m_pLoadingNoteA->SetVisible(false);
	m_pLoadingNoteC->SetVisible(false);
	m_pLoadingNoteB->SetVisible(false);
	m_pLoadingNoteD->SetVisible(false);
}

void CUi_LoadingSystem::Dying()
{
	m_pLoadingCat->Dead();
	m_pLoadingNoteA->Dead();
	m_pLoadingNoteC->Dead();
	m_pLoadingNoteB->Dead();
	m_pLoadingNoteD->Dead();
}

shared_ptr<CUi_LoadingSystem> CUi_LoadingSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_LoadingSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_LoadingSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_LoadingSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_LoadingSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_LoadingSystem
	{
		MakeSharedEnabler(const CUi_LoadingSystem& rhs) : CUi_LoadingSystem(rhs) { }
	};

	shared_ptr<CUi_LoadingSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
