#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CRekka_Long_Pipe : public CPartObject
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
	CRekka_Long_Pipe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_Long_Pipe(const CRekka_Long_Pipe& rhs);
public:
	virtual ~CRekka_Long_Pipe();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();

public:
	void AttachOn();
	void AttachOff();

private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };

private:
	_bool m_isAttached = false;

private:
	const _float4 m_vDefaultPos = { 1181.41f, 487.38f, 132.353f, 1.f };
	const _float4x4* m_pBoneMatrix = { nullptr };


public:
	static shared_ptr<CRekka_Long_Pipe> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END