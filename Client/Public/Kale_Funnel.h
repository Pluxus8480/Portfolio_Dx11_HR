#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CKale_Funnel : public Engine::CPartObject
{
public:
	static wstring ObjID;
public:
	struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_uint	iNumber;
	};
	enum STATE
	{
		TO_ATK,//30.f
		CHARGE,//60.f
		TO_IDLE,//20.f
		SHOOT,//30.f
		STATE_END
	};
private:
	CKale_Funnel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKale_Funnel(const CKale_Funnel& rhs);


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	//void Set_AnimSpeed(_float fAnimSpeed) { m_fAnimSpeed = fAnimSpeed; }

	void Shoot(_int iTimer);
	void ReAttach(const _float4x4* pBoneMatrix) {
		m_pBoneMatrix = pBoneMatrix;
	}
private:
	void Anim_End();
	void Select_Beat();
	void Look(_fvector vAt, _float fTimeDelta);

private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };
	shared_ptr <CCollider> m_pColliderCom = { nullptr };

	//vector<string> m_vecAnimList;

private:
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float m_fAnimSpeed = 1.f;
	_int m_iRenderCount = 0;
	STATE m_eState = TO_IDLE;
	_wstring m_strModelTag;
	_int	m_iShootTimer = 0;
	_float	m_fShootTimerLeft = 0.f;
	_uint m_iPrevBeat = 0;
	_float m_fFloatingPoint = 0.f;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CKale_Funnel> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual ~CKale_Funnel();
};


END