#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CRekka_Gauntlet : public CPartObject
{
public:
	static wstring ObjID;

public:
	struct DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_uint iLevelIndex;
	};

private:
	CRekka_Gauntlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_Gauntlet(const CRekka_Gauntlet& rhs);
public:
	virtual ~CRekka_Gauntlet();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	void PrepareCallbacks();

private:
	void SizeUp();

public:
	void ReadyForAttack();
	void StayForAttack();
	void StartAttack();
	void FinishAttack();

private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };

	const _float4x4* m_pBoneMatrix = { nullptr };
	const _float4x4* m_pMat_Spine = { nullptr };

private:
	_float			m_fTimer_Scale = 0.f;
	_float			m_fCurScale = 1.f;

	_float			m_fTargetScale = 1.f;

	_float4x4		m_matScale;

private:
	void SelectBeat();
private:
	_float m_fBeatSpeed = 1.f;

private:
	enum class EStep {
		Default, Ready, Stay, Attack
	};
	EStep m_eStep = EStep::Default;
private:
	_bool	m_isRendering = false;


public:
	static shared_ptr<CRekka_Gauntlet> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END