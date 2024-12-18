#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CSwordMachine_Sword : public CPartObject
{
public:
	static wstring ObjID;

public:
	struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_uint* pState;
		_uint  iLevelIndex;
	};

private:
	CSwordMachine_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwordMachine_Sword(const CSwordMachine_Sword& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void Animation(_float fTimeDelta);

private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };

private:
	const _float4x4* m_pBoneMatrix = { nullptr };


public:
	static shared_ptr<CSwordMachine_Sword> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual ~CSwordMachine_Sword();
};

END