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

class CDMGConsole : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

private:
	enum CONSOLEINDEX { CONSOLE1, CONSOLE2, CONSOLE3, CONSOLE4, CONSOLE5, CONSOLE6, CONSOLE7, CONSOLE_END };

	struct CONSOLEINFO {
		_uint iConsoleIndex;
		_uint iNumDMG;
		_uint iTextureIndex[2] = {};
		_uint iMaterialIndex[2] = {};
	};

	struct DMGINFO {
		_uint iTextureIndex;
		_int iX;
		_int iY;
		_int iMaxFrame;
		_float fSecPerFrame;
	};

private:
	CDMGConsole(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDMGConsole(const CDMGConsole& rhs);
public:
	~CDMGConsole();

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
	vector<shared_ptr<CModel>> m_vecModelCom = { nullptr };
	vector<shared_ptr<CTexture>> m_vecTextureCom;

	vector<CONSOLEINFO> m_vecConsoleInfo;
	vector<DMGINFO> m_vecDMGInfo;

	CONSOLEINFO m_tCurConsoleInfo;

	_int m_iModelIndex = {};

	_int m_iFrameIndex[2] = { 0, };
	_int m_iMaxFrame[2] = { 1,1 };
	_float m_fSecPerFrame[2] = {0.f, };

	_float m_fTime[2] = {0.f, };

	_bool m_bInit = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CDMGConsole> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END