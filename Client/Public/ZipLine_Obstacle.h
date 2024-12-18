#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CZipLine_Obstacle : public CPartObject
{
public:
	static wstring ObjID;

public:
	enum class OBS_DIR { LEFT, RIGHT, FRONT, OBS_END };

	struct ZIPOBS_DESC : public PARTOBJ_DESC
	{
		_int iRailIndex;
		OBS_DIR eDir;
		_float4x4 localMat;
	};

	struct ZIPOBS_INFO
	{
		OBS_DIR eDir;
		_float3 vWorldPosition;
	};

private:
	enum MODELINDEX { MAGNET, CONTAINER, MODEL_END };

private:
	CZipLine_Obstacle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CZipLine_Obstacle(const CZipLine_Obstacle& rhs);
public:
	~CZipLine_Obstacle();

public:
	_bool IsChecked() const { return m_bChecked; }
	_int Get_RailIndex() const { return m_iRailIndex; }
	ZIPOBS_INFO Get_ObstacleInfo();
	void Set_Checked(_bool bChecked) { m_bChecked = bChecked; }
	
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual void Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;

	void Set_RenderState(_bool State = false) { m_bRender = State; }

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom[MODEL_END] = { nullptr, };
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

	_float4x4 m_LocalWorld = Identity;
	const _float m_fRotRadian = { XMConvertToRadians(30.f) };

	_int m_iRailIndex = {};

	OBS_DIR m_eDirection = { OBS_DIR::OBS_END };
	_float4x4 m_ContainerMatrix = Identity;
	_float4x4 m_ContainerWorld = Identity;

	_bool m_bChecked = { false };
	_bool m_bEvade = false;
	_bool m_bRender = { true };

private:
	void Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CZipLine_Obstacle> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END