#pragma once

#include "Client_Defines.h"
#include "CUi_Canvas.h"

BEGIN(Client)

class CUi_LoadingSystem : public CUi_Canvas
{
public:
	static wstring ObjID;

protected:
	CUi_LoadingSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_LoadingSystem(const CUi_LoadingSystem& rhs);

public:
	virtual ~CUi_LoadingSystem();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void    Tick(_float fTimeDelta);
	virtual void    Dying() override;

public:
	void Create_LoadingCat();
	void Create_LoadingNote();
	void ShowCurrentNote();
	void HideAllNotes();


protected:
	_float m_fElapsedTime = 0.0f; 
	_float m_fNoteIntervals[4] = { 0.4f, 1.0f, 1.4f, 1.6f };  
	_float m_fDisappearDelay = 0.8f;  
	_int m_iCurrentNote = 0;
	_bool m_bAllNotesVisible = { false };

protected:
	shared_ptr<class CUi_Sprite> m_pLoadingCat;
	shared_ptr<class CUi_Sprite> m_pLoadingNoteA;
	shared_ptr<class CUi_Sprite> m_pLoadingNoteB;
	shared_ptr<class CUi_Sprite> m_pLoadingNoteC;
	shared_ptr<class CUi_Sprite> m_pLoadingNoteD;

public:
	static shared_ptr<CUi_LoadingSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
