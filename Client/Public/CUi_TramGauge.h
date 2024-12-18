#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "Brake_Part.h"

BEGIN(Client)

class CUi_TramGauge : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_TramGaugeInfo
	{
		const _float4x4* pBonePtr = { nullptr };
		CBrake_Part::BRAKE PartPos = {};
		const _int* pHp = { nullptr };
	};

protected:
	CUi_TramGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_TramGauge(const CUi_2D& rhs);


public:
	virtual ~CUi_TramGauge();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	void Render_Back();
	void Render_String();
	void Initialize_Gauge();
	void Initialize_Rotation();
	void Initialize_Back();
	

private:
	void Sync_Pos();
	void Adjust_Back();
	void Cal_Percent();


private:
	shared_ptr<CTexture> m_pBackTextureCom;
	shared_ptr<class CTransform> m_pBackTransformCom;
	const _float4x4* m_pBoneptr = { nullptr };
	_float m_fPercent = { 1 };
	CBrake_Part::BRAKE m_ePartPos = {};
	const _int* m_pPartHp = { nullptr };
	_int m_PartMaxHp = { 0 };


public:
	static shared_ptr<CUi_TramGauge> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
