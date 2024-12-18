#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CMimosa_LightShaft : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

private:
	CMimosa_LightShaft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMimosa_LightShaft(const CMimosa_LightShaft& rhs);
public:
	~CMimosa_LightShaft();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom = { nullptr };

	_int m_iModelIndex = { 0 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Create_Model();

public:
	static shared_ptr<CMimosa_LightShaft> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END