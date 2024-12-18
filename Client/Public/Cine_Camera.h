#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Cine_Sequence.h"

BEGIN(Client)

class CCine_Camera : public CCamera
{
public:
	static wstring ObjID;

public:
	struct DESC : public CCamera::CAMERA_DESC {
		_float		fMouseSensor = {};
	};

private:
	CCine_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCine_Camera(const CCine_Camera& rhs);

public:
	~CCine_Camera();

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

private:
	_float m_fMouseSensor = {};

public:
	static shared_ptr<CCine_Camera> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END