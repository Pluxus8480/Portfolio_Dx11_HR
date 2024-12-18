#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Union.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)

class CMimosa_SpotLight : public CGameObject
{
public:
	static wstring ObjID;

private:
	CMimosa_SpotLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMimosa_SpotLight(const CMimosa_SpotLight& rhs);
public:
	virtual ~CMimosa_SpotLight();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Change_Color(_bool bChange) { m_bColor_Change = bChange; }

	virtual void Dying()override;
private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CEffect_Union> m_SpotLight = {};

	_float4 m_fColor = {};
	_bool m_bColor_Change = false;
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CMimosa_SpotLight> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END