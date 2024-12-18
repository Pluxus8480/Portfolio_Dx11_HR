#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;

END

BEGIN(Client)

class CMimosa_Floor : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

private:
	CMimosa_Floor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMimosa_Floor(const CMimosa_Floor& rhs);
public:
	~CMimosa_Floor();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Change_Phase(_uint iPhase);

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel>  m_pModelCom = { nullptr };
	shared_ptr<CTexture>  m_pTextureCom = { nullptr };

	_bool m_bRefl = false;

public:
	static shared_ptr<CMimosa_Floor> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END