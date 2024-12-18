#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CSpectra_FanBlade : public CPartObject
{
public:
	static wstring ObjID;

private:
	CSpectra_FanBlade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpectra_FanBlade(const CSpectra_FanBlade& rhs);
public:
	~CSpectra_FanBlade();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CShader> m_pShaderCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CSpectra_FanBlade> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END