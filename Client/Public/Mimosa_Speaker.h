#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMimosa_Speaker : public CGameObject
{
public:
	static wstring ObjID;

private:
	CMimosa_Speaker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMimosa_Speaker(const CMimosa_Speaker& rhs);
public:
	~CMimosa_Speaker();

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

	_bool m_bLightOn = { false };
	_float m_fLightTime = {};

	_uint m_iPrevBeat = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CMimosa_Speaker> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END