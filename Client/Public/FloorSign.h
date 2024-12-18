#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CModel;
class CTexture;
END

BEGIN(Client)

class CFloorSign : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

private:
	enum FLOOR { FIRST, SECOND, THIRD, FLOOR_END };

private:
	CFloorSign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFloorSign(const CFloorSign& rhs);
public:
	~CFloorSign();

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
	shared_ptr<CVIBuffer_Rect> m_pVIBufferCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom[FLOOR_END] = { nullptr, };

	_int m_iNumFloor = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CFloorSign> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END