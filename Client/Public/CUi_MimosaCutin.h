#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUI_MimosaCutin : public CUi_2D
{
public:
	static wstring ObjID;

public:
	enum class State { MimosaAppear, ChaiAppear, Slow, Disappear, End };
	//using Desc = struct CUI_MimosaCutinInfo
	//{
	//	_float fLifeTime = { 0 };
	//};

protected:
	CUI_MimosaCutin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MimosaCutin(const CUi_2D& rhs);


public:
	virtual ~CUI_MimosaCutin();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

private:
	void Progress_State(_float fTimeDelta);
	void Progress_MimosaAppear(_float fTimeDelta);
	void Progress_ChaiAppear(_float fTimeDelta);
	void Progress_Slow(_float fTimeDelta);
	void Progress_Disappear(_float fTimeDelta);
	void Initialize_CUtinObj();
	virtual void Dying() override;
	

private:
	shared_ptr<class CUi_Default> m_pChaiCutin;
	shared_ptr<class CUi_Default> m_pMimosaCutin;
	_float m_fSlowTime = { 1.f };
	_float m_fDelayTime = { 0.5f };
	State m_eState = { State::MimosaAppear };


public:
	static shared_ptr<CUI_MimosaCutin> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
