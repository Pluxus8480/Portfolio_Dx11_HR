#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)

class CJeongGimmicBaseDefault : public CJeongGimmicBase
{
public:
	static wstring ObjID;


private:
	CJeongGimmicBaseDefault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJeongGimmicBaseDefault(const CJeongGimmicBaseDefault& rhs);
public:
	~CJeongGimmicBaseDefault();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller) override;
	virtual HRESULT Render() override;
	void Render_Reflection();

private:
	wstring m_strGimmickNameID = L"";

private:
	_bool m_bDMGConsole = { false };
	_bool m_bInit = { false };
	_float m_fTime[3] = {};
	_float m_fSecPerFrame[3] = {};
	_uint m_iFrameIndex[3] = {};
	const _uint m_iMaxFrame[3] = { 8, 8, 4 };

public:
	static shared_ptr<CJeongGimmicBaseDefault> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();

private:
	_int m_iSkipMeshNum = 0;
};

END