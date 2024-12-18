#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CPhysXCollider;
END

BEGIN(Client)

class CPartDoor : public CPartObject
{
public:
	static _wstring ObjID;

	enum DOOR_DIR { LEFT, RIGHT, UP, DOWN, DIR_END };

	struct DOOR_PARTDESC : public CPartObject::PARTOBJ_DESC
	{
		_bool bOpen;
		_bool bNumbered = { false };
		DOOR_DIR eDir;
		_uint iNumDecal = { 0 };
		_float fMoveDist;
		_float4x4 worldMatrix;
		_wstring strModelTag;
	};

private:
	CPartDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartDoor(const CPartDoor& rhs);
public:
	~CPartDoor();

public:
	void Set_DecalIndex(_int iDecalIndex, _int iTextureIndex);

public:
	void Execute_Door(_bool bExecute);
	void Set_Open(_bool bOpen) { m_bExecute = true; m_bDone = false; m_bOpen = bOpen; }
	void Init_DoorState(_bool bOpen);

	_bool Get_Open() const { return m_bOpen; }
	_bool Get_Execute() const { return m_bExecute; }
	_bool Get_Done() const { return m_bDone; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CPhysXCollider> m_pPhysXCollider = { nullptr };

	_bool m_bExecute = { false };
	_bool m_bDone = { false };
	_bool m_bOpen = { false };
	DOOR_DIR m_eDoorDir = { DIR_END };
	_float m_fMoveDist = {};

	_float3 m_vModelCenter = {};
	_float4 m_vWorldCenter = {};
	_float3 m_vModelSize = {};
	_float m_fModelRange = {};
	vector<PxRigidStatic*> m_vecPxRigid;

	_bool m_bNumbered = { false };
	vector<_int> m_vecDecalIndex;

private:
	HRESULT Ready_Components(DOOR_PARTDESC* pDesc);
	HRESULT Bind_ShaderResources();

	void Open_Door(_float fTimeDelta);
	void Close_Door(_float fTimeDelta);

public:
	static shared_ptr<CPartDoor> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END