#pragma once
#include "Client_Defines.h"
#include "CMagnet.h"
#include "ZipLine_Obstacle.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CZipLine_Magnet : public CMagnet
{
public:
	static wstring ObjID;

	enum class ANIMSTATE
	{
		GO_FORWARD = 0,
		DODGE_LEFT = 1,
		DODGE_RIGHT = 2,
		DASH = 3, // ???
		HIT = 4,
		DODGE_TURNOVER = 5,
		EVENT_EXIT = 7,
		EVENT_ENTER = 8,
		IDLE = 13, 
		ANIM_END
	};

private:
	CZipLine_Magnet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CZipLine_Magnet(const CZipLine_Magnet& rhs);
public:
	~CZipLine_Magnet();

public:
	const _float4x4* Get_PlayerAttachMatrix();
	_bool Get_EventOnGoing() { return (m_bEnterEvent && !m_bExitEvent); }

	void Set_MagnetDirection(_float3 vDirection) { m_vDirection = vDirection; }
	void Set_AnimState(ANIMSTATE eState);
	void Set_CurObstacleInfo(const CZipLine_Obstacle::ZIPOBS_INFO& tCurObstacleInfo) { m_tCurObstacleInfo = tCurObstacleInfo; }

	void Enter_Event();
	void Exit_Event();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Animation(_float fTimeDelta);
	void Animation_End(_float fTimeDelta);

private:
	void Spark_Control(_float fTimeDelta);
	void Zipline_Spark(_float fTimeDelta);

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CTexture> m_pEmissionTexture[2] = { nullptr, };

	_float3 m_vDirection = {};

	_bool m_bEnterEvent = { false };
	_bool m_bExitEvent = { false };
	_bool m_bPlayer_Attach_End = false;
	_bool m_bSet_Wire_list = false;
	ANIMSTATE m_eAnimState = { ANIMSTATE::IDLE };

	CZipLine_Obstacle::ZIPOBS_INFO m_tCurObstacleInfo = {};

	//Spark Variables
private:
	_float m_fSpark_Delay = 0.f;
	_float m_fSpark_DelayMax = 0.1f;

public:
	void Set_RenderState(_bool Render) { m_bRender = Render; }
private:
	void Ready_Components();
	HRESULT Bind_ShaderResources();
	_bool m_bRender = { true };

public:
	static shared_ptr<CZipLine_Magnet> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END