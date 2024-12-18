#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Imgui_Manager.h"
#include "Cine_Scenario.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CCine_Sequence;
class CCine_Actor;
class CTool_Cine_Camera;
class CLevel_CineTool : public CLevel
{
private:
	CLevel_CineTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_CineTool();

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Light();

	void	Imgui_GizmoRender();
	void	Imgui_Trash_GizmoRender();
	tagGizmoDesc m_tGizmoDesc;

private:
	void	OptionTool();
	void	CineTool();
	void	CameraTool();

private:
	CCine_Scenario	m_CurScenario;
	wstring			m_curSoundTag = L"";

	
private:
	// For Sequence
	string			m_strSelectedActor = "";
	string			m_strSelectedAnim = "";

	_int			m_iActionSelect = -1;
	_int			m_iCamSelect = -1;
	_int			m_iEffectSelect = -1;
	_int			m_iCallbackSelect = -1;
	_int			m_iConversationSelect = -1;

private:
	shared_ptr<CTransform>		m_pCurSeq_Trans = nullptr;
	shared_ptr<CTool_Cine_Camera>	m_pToolCamera = nullptr;

	shared_ptr<CTransform>			m_pSelectAction_Trans = nullptr;

private:
	_bool						m_isTrashMode = false;
	shared_ptr<CTransform>		m_pTrash_Trans = nullptr;

private:
	ImGui::FileBrowser m_fileDialog_Sound;
	ImGui::FileBrowser m_fileDialog_Json;
	ImGui::FileBrowser m_fileDialog_Save_Scenario;
	ImGui::FileBrowser m_fileDialog;


public:
	static shared_ptr<CLevel_CineTool> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
