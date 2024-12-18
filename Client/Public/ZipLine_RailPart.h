#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CZipLine_RailPart : public CPartObject
{
public:
	static wstring ObjID;

	struct RAILPART_DESC : public PARTOBJ_DESC
	{
		_bool bRapidCurve = { false };
		_float4x4 vLocalMatrix = Identity;
	};

private:
	enum POINT { LEFT, RIGHT, POINT_END };

private:
	CZipLine_RailPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CZipLine_RailPart(const CZipLine_RailPart& rhs);
public:
	~CZipLine_RailPart();

public:
	_float3 Get_LocalRightPoint() const { return m_vLocalPoint[RIGHT]; }
	_vector Get_WorldRightPoint() const { return XMVectorSetW(XMLoadFloat3(&m_vWorldPoint[RIGHT]), 1.f); }

	_bool Is_RapidCurve() const { return m_bRapidCurve; }
	void Set_RapidCurve(_bool bRapidCurve) { m_bRapidCurve = bRapidCurve; }

	_bool IsIn_RailPart(_fvector vPosition);
	_vector Adjust_Position(_fvector vPosition);
	_vector Calc_WorldDirection();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Set_RenderState(_bool State = false) { m_bRender = State; }

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

	_float4x4 m_LocalWorld = Identity;
	_float3 m_vLocalPoint[POINT_END] = {};
	_float3 m_vWorldPoint[POINT_END] = {};

	_bool m_bRapidCurve = { false };

	_float4 m_vYellowColor = { 0.922f, 0.808f, 0.f, 1.f};

	_bool m_bRender = { true };
#ifdef _DEBUG
	shared_ptr<CCollider> m_pColliderCom[POINT_END] = { nullptr, };
#endif

private:
	void Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CZipLine_RailPart> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END