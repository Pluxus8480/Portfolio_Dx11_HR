#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CFree_SM_Object : public CGameObject
{
public:
	static wstring ObjID;

public:
	struct DESC
	{
		string strModelPath;
	};

private:
	CFree_SM_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFree_SM_Object(const CFree_SM_Object& rhs);
public:
	virtual ~CFree_SM_Object();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();

private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };

private:
	const _float4x4* m_pBoneMatrix = { nullptr };

public:
	void AttachBone(const _float4x4* pBoneMatrix);

private:
	wstring m_strModelTag;


public:
	static shared_ptr<CFree_SM_Object> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END