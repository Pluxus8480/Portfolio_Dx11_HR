#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CMimosa_RoofLEDText : public CGameObject, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CMimosa_RoofLEDText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMimosa_RoofLEDText(const CMimosa_RoofLEDText& rhs);
public:
	~CMimosa_RoofLEDText();

public:
	virtual void Change_Phase(_uint iPhase) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

	_float4 m_vDiffuseColor[4] = { {}, };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CMimosa_RoofLEDText> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END