#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Cine_Sequence.h"

BEGIN(Client)

class CTool_Cine_Camera : public CCamera
{
public:
	static wstring ObjID;

public:
	struct DESC : public CCamera::CAMERA_DESC {
		_float		fMouseSensor = {};
	};

private:
	CTool_Cine_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Cine_Camera(const CTool_Cine_Camera& rhs);

public:
	~CTool_Cine_Camera();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void SetCamInfo(CCine_Sequence::CineCamInfo& camInfo);
	void GetCamInfo(CCine_Sequence::CineCamInfo& camInfo);

	_float GetSpeed() const;

private:
	_float m_fMouseSensor = {};
public:
	_float m_fMoveSpeed = 10.f;

public:
	static shared_ptr<CTool_Cine_Camera> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END