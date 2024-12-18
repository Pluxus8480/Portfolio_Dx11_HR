#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"
#include "Brake_Part.h"


BEGIN(Client)

class CUi_TramMacaronSign : public CUi_2D
{
public:
	static wstring ObjID;

	using Desc = struct CUi_TramMacaronSignInfo
	{
		const _float4x4* pBonePtr = { nullptr };
		CBrake_Part::BRAKE PartPos = {};
		_float AwareDistance = {};
	};

protected:
	CUi_TramMacaronSign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_TramMacaronSign(const CUi_TramMacaronSign& rhs);


public:
	virtual ~CUi_TramMacaronSign();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

private:
	void Initialize_Sign();
	void Adjust_Pos();
	_bool Judge_InFrustum_Distance();


private:
	const _float4x4* m_pBoneptr = { nullptr };
	CBrake_Part::BRAKE m_ePartPos = {};
	_float m_fAwareDistance = {};


public:
	static shared_ptr<CUi_TramMacaronSign> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
