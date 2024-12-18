#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)
class CEffect_Union;

class CKale_BigSword : public CGameObject
{
public:
	static wstring ObjID;

private:
	CKale_BigSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKale_BigSword(const CKale_BigSword& rhs);
public:
	virtual ~CKale_BigSword();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Push_From_Pool(void* pArg) override;
	virtual void Dying() override;

public:
	void Set_Extend(_bool bExtend) { m_bExtended = bExtend; }

private:
	HRESULT Ready_Components();
	_int m_iPrevBeat = 0;
	_bool m_bShoot = false;
	//_int	m_iShootTimer = 0;
	_float	m_fShootTimerLeft = 0.f;

	_bool m_bExtended = false;
public:
	static shared_ptr<CKale_BigSword> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END