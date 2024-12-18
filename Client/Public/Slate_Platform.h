#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPhysXCollider;
END

BEGIN(Client)

class CSlate_Platform : public CGimmickBase
{
public:
	static wstring ObjID;

	enum STATE
	{
		CLOSE,
		OPEN,
		OPEN_TO_CLOSE_1,
		OPEN_TO_CLOSE_2,
		OPEN_TO_CLOSE_ONCE,
		CLOSE_TO_OPEN_1,
		CLOSE_TO_OPEN_2,
		STATE_END,
	};

	struct ANIMOBJ_DESC : public CTransform::TRANSFORM_DESC
	{
		_float4x4 worldMatrix = {};
	};

private:
	CSlate_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlate_Platform(const CSlate_Platform& rhs);
public:
	virtual ~CSlate_Platform();

public:
	HRESULT Set_ModelComponent(const string& strPrototypeTag);

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

	void Set_OpenDuration(_int iDuration) { m_iOpenDuration = iDuration; }
	void Set_CloseDuration(_int iDuration) { m_iCloseDuration = iDuration; }

	void Open();
	void Close();

private:
	void Anim_Finished();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };

	STATE m_iCurrAnimIndex = { CLOSE };
	_float m_fAnimSpeed[STATE_END] = {};

	_uint m_iPrevBeat = { 0 };

	_bool m_bPlay = { false };
	_float m_fSize = 0.f;
	_bool m_bInit = false;

	_int m_iStartAccBeat = 0;
	_int m_iAccBeat = 0;
	_int m_iOpenDuration = 7;
	_int m_iCloseDuration = 7;
	_bool m_bCulled = false;

private:
	HRESULT Ready_Components(const COMMONOBJ_DESC* pDesc);

public:
	static shared_ptr<CSlate_Platform> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END