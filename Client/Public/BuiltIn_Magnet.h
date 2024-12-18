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

class CBuiltIn_Magnet : public CMagnet, public CBoss_InteractObj
{
public:
	static wstring ObjID;
private:
	CBuiltIn_Magnet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBuiltIn_Magnet(const CBuiltIn_Magnet& rhs);
public:
	~CBuiltIn_Magnet();

private:
	enum ANIMSTATE { EXCT_BARRIER, EXCT_FULL, EXCT_SHORT, IDLE_BARRIER, IDLE_ON, IDLE_OFF, ANIM_END };
	enum TEXTURE_INDEX { ON_SQ, OFF_SQ, ON_WARN, OFF_WARN, TEX_END };

public:
	virtual void Execute(_bool bExecute = true) override;
	void Set_MagnetAttribute(_bool bWait, _int iJumpHeight);

	_bool Is_ComboMagnet() const { return m_bComboMagnet; }
	_int Get_JumpHeight() const { return m_iJumpHeight; }
	shared_ptr<CGameObject> Get_NextMagnet();

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

	virtual void Store_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;
	virtual void Remove_Relatives(shared_ptr<CGimmickBase> pRelativeObject) override;

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

	_int m_iJumpHeight = { 0 };
	_bool m_bSet_Wire_list = false;

	_bool m_bComboMagnet = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Update_AnimState();


public:
	static shared_ptr<CBuiltIn_Magnet> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END