#pragma once
#include "Client_Defines.h"
#include "CMagnet.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CWireMagnet : public CMagnet, public CBoss_InteractObj
{
public:
	static wstring ObjID;
private:
	CWireMagnet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWireMagnet(const CWireMagnet& rhs);
public:
	~CWireMagnet();

private:
	enum ANIMSTATE { EXCT_BARRIER, EXCT_FULL, EXCT_SHORT, IDLE_BARRIER, IDLE_ON, IDLE_OFF, ANIM_END };
	enum TEXTURE_INDEX { ON_SQ, OFF_SQ, ON_WARN, OFF_WARN, TEX_END };

public:
	virtual void Execute(_bool bExecute = true) override;
	virtual void Change_Pattern(_uint iPattern);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Collision_Enter(shared_ptr<CCollider> pOther, shared_ptr< CCollider> pCaller) override;

	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;


	virtual void Change_Phase(_uint iPhase);

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom[TEX_END] = {nullptr,};

	_bool m_bInit = { false };
	vector<_float> m_vecAnimSpeeds;

	ANIMSTATE m_eAnimState = {};
	_uint m_iSquareEmiIndex = {};

	_bool m_bWait = { false };
	_bool m_bSet_Wire_list = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Update_AnimState();

public:
	static shared_ptr<CWireMagnet> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END