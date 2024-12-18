#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CBasic_Guitar : public Engine::CPartObject
{
public:
	static wstring ObjID;
public:
	struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		const _float4x4* pEventBoneMatrix;
		const _float4x4* pLeftHandBoneMatrix;
		_uint* pState;
	};
	enum class GUITAR_STATE
	{
		NORMAL,EVENT, LEFTHAND , MIMOSA_DANCE,GUITAR_STATE_END
	};
private:
	CBasic_Guitar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBasic_Guitar(const CBasic_Guitar& rhs);


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Change_Animation(string strAnimName);
	void Change_Event_Animation(string strAnimName);

	void Change_Guitar_State(GUITAR_STATE eGuitarState) { m_eGuitar_State = eGuitarState; }
	void Is_Electric_Guitar(_bool bElectir) { m_bElectric_Guitar = bElectir; }
	void Is_Render(_bool bRender) { m_bRender = bRender; }
	void Animation_End();
private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };
	shared_ptr <CModel> m_pElecGuitar_ModelCom = { nullptr };
	shared_ptr <CCollider> m_pColliderCom = { nullptr };

private:
	const _float4x4* m_pBoneMatrix = { nullptr };
	const _float4x4* m_pEvent_BoneMatrix = { nullptr };
	const _float4x4* m_pLeftHand_BoneMatrix = { nullptr };
	vector<string> m_vecAnimList;

	_bool m_bElectric_Guitar = false;
	GUITAR_STATE m_eGuitar_State = GUITAR_STATE::NORMAL;
	_bool m_bRender = true;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CBasic_Guitar> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual ~CBasic_Guitar();
};


END