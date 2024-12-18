#pragma once
#include "GameManager.h"
#include "Gameinstance.h"
#include "Imgui_Manager.h"
#include "Texture.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CGame_Manager)

CGame_Manager::CGame_Manager()
{

}

CGame_Manager::~CGame_Manager()
{
	Free();
}

void CGame_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
	GAMEINSTANCE->Add_Prototype(LEVEL_STATIC, L"TransitionDissolve", CTexture::Create(pDevice, pContext, L"../Bin/Resources/Textures/T_VFX_tk_scratch_AReye.png", 1));
	GAMEINSTANCE->Add_Prototype(LEVEL_STATIC, L"DefaultNormal", CTexture::Create(pDevice, pContext, L"../Bin/Resources/Textures/T_FlatNormal_MM.dds", 1));
	m_pDissolveNoise = static_pointer_cast<CTexture>(GAMEINSTANCE->Clone_Component(LEVEL_STATIC, L"TransitionDissolve"));
	m_pDefaultNormal = static_pointer_cast<CTexture>(GAMEINSTANCE->Clone_Component(LEVEL_STATIC, L"DefaultNormal"));

}

void CGame_Manager::PriorityTick(_float fTimeDelta)
{
	if(m_fDissolveTimer > 0.f)
	{
		GAMEINSTANCE->Dissolve_On(m_pDissolveNoise, m_fDissolveTimer / m_fDissolveMax, m_fDissolveRange);
		m_fDissolveTimer -= fTimeDelta;
		if (m_fDissolveTimer < 0.f)
			m_fDissolveTimer = 0.f;
	}
	else if (m_fDissolveTimer < 0.f)
	{
		GAMEINSTANCE->Dissolve_On(m_pDissolveNoise, (m_fDissolveMax + m_fDissolveTimer) / m_fDissolveMax, m_fDissolveRange);
		m_fDissolveTimer += fTimeDelta;
		if (m_fDissolveTimer > 0.f)
			m_fDissolveTimer = 0.f;
	}

}

void CGame_Manager::Tick(_float fTimeDelta)
{
	DebugMode();
}

void CGame_Manager::LateTick(_float fTimeDelta)
{

}

void CGame_Manager::Tick_Engine(_float fTimeDelta)
{
	PriorityTick(fTimeDelta);
	Tick(fTimeDelta);
	LateTick(fTimeDelta);
}

void CGame_Manager::Render()
{

}

void CGame_Manager::Dissolve(_float fTime, _float fRange)
{
	m_fDissolveMax = abs(fTime);  m_fDissolveTimer = fTime; m_fDissolveRange = fRange;
}

void CGame_Manager::Bind_DefaultNormal(shared_ptr<CShader> pShader, const char* pTag)
{
	m_pDefaultNormal->Bind_ShaderResource(pShader, pTag);
}

void CGame_Manager::DebugMode()
{
	if (GAMEINSTANCE->Get_KeyDown(DIK_F8)) m_bDebugMode = !m_bDebugMode;

	if(m_bDebugMode == true)
		IMGUIMANAGER->Imgui_SetState(eImgui_State::IMGUI_STATE_RENDER);
	else
		IMGUIMANAGER->Imgui_SetState(eImgui_State::IMGUI_STATE_WAIT);

}

void CGame_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
