#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CKale_Sword : public Engine::CPartObject
{
public:
	static wstring ObjID;
public:
	struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_uint* pState;
	};

private:
	CKale_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKale_Sword(const CKale_Sword& rhs);


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Setup_Matrix();

	void Set_AnimSpeed(_float fAnimSpeed) { m_fAnimSpeed = fAnimSpeed; }
	_bool Change_Animation(string strAnimName);
	void Yellow(_bool bToggle) { m_bYellow = bToggle; }

	void ReAttach(const _float4x4* pBoneMatrix) {
		m_pBoneMatrix = pBoneMatrix;
	}
	void Off_Render()
	{
		m_bRender = false;
	}
private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };
	shared_ptr <CCollider> m_pColliderCom = { nullptr };

	vector<string> m_vecAnimList;

private:
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float m_fAnimSpeed = 1.f;
	_bool m_bYellow = false;
	_bool m_bAnimStop = false;
	_int m_iRenderCount = 0;
	_bool m_bRender = true;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CKale_Sword> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual ~CKale_Sword();
};


END