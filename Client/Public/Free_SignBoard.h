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

class CFree_SignBoard : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;
private:
	CFree_SignBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFree_SignBoard(const CFree_SignBoard& rhs);
public:
	~CFree_SignBoard();

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
	shared_ptr<CTexture> m_pSoundWaveTexture = { nullptr };

	string m_strModelPath;
	string m_strTexturePath;

	_int m_iSignIndex = {};
	_bool m_bEmissive = { false };
	_bool m_bBattleBoard = { false };

	_float4 m_vModelCenter = {};
	_float m_fCullingRange = {};

	/* SoundWave */
	_bool m_bSoundWave = { false };
	_bool m_bChanged = { false };
	_int m_iFrameIndex = { 0 };
	const _int m_iMaxFrame = { 16 };
	_float m_fSecPerFrame = {};
	_float m_fTime = {};
	_uint m_iPrevBeat = {};

	_bool m_bInit = { false };

	/* Tool */
	ImGui::FileBrowser fileDialog;
	_bool m_bModelSelect = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Create_Model();
	void Create_Texture();

public:
	static shared_ptr<CFree_SignBoard> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END