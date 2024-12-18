#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
END

BEGIN(Client)

class CSignBoard : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;
private:
	CSignBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSignBoard(const CSignBoard& rhs);
public:
	~CSignBoard();

public:
	void Set_TextureIndex(_uint iTextureIndex);

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
	vector<shared_ptr<CTexture>> m_vecTextureCom;

	vector<wstring> m_vecTextureTags;
	_int m_iTextureIndex = {};

	_bool m_bSoundWave = { false };
	_int m_iFrameIndex = { 0 };
	const _int m_iMaxFrame = { 16 };
	_float m_fSecPerFrame = {};
	_float m_fTime = {};
	_uint m_iPrevBeat = {};

	_bool m_bInit = { false };

private:
	HRESULT Ready_Components(const MISCOBJ_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CSignBoard> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END