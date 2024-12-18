#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "CUi_JudgeWord.h"

BEGIN(Client)

class CUi_CommandIcon : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_CommandIconInfo : public CUi_2DInfo
	{
		_bool SpecialIcon = { false };
		_uint CorrectBeat = { 2 };
		_float SafeCircleSize = { 200 };
		_float DangerCircleSize = { 400 };
		_bool bCreateSuccess = { true };
		_bool bCreateWord = { true };
	};
protected:
	CUi_CommandIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_CommandIcon(const CUi_2D& rhs);


public:
	virtual ~CUi_CommandIcon();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	CUi_JudgeWord::WordType Cal_Judge();


	_float Get_DangerIconSize() { return m_pDangerIcon->Get_Scale().x; }


private:
	void Initialize_Icon(_float fX, _float fY, _float SafeCircleSize, _float DangerCircleSize, _bool SpecialIcon);
	void Initialize_Speed(Desc Desc);
	virtual void Dying() override;


private:
	shared_ptr<class CUi_Default> m_pDangerIcon;
	shared_ptr<class CUi_Default> m_pSafeIcon;
	_float m_fScaleDownSpeed = { 0 };
	_float m_fWordTime = { 0 };
	_bool m_bCreateSuccess = { true };
	_bool m_bCreateWord = { true };

public:
	static shared_ptr<CUi_CommandIcon> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
