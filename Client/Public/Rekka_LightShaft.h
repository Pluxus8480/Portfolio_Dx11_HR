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

class CRekka_LightShaft : public CGameObject, public CMisc_Interface, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CRekka_LightShaft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_LightShaft(const CRekka_LightShaft& rhs);
public:
	~CRekka_LightShaft();

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

	_float4 m_vPhaseColor[4] = {
		{0.294f, 0.561f, 0.902f, 0.8f},
		{0.478f, 0.094f, 0.337f, 0.8f},
		{1.f, 1.f, 0.494f, 0.8f},
		{0.988f, 0.176f, 0.184f, 0.8f}
	};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Create_Model();

public:
	static shared_ptr<CRekka_LightShaft> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END