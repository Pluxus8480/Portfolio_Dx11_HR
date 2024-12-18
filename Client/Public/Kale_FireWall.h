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

class CKale_FireWall : public CGameObject
{
public:
	static wstring ObjID;

private:
	CKale_FireWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKale_FireWall(const CKale_FireWall& rhs);
public:
	virtual ~CKale_FireWall();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Push_From_Pool(void* pArg) override;
	virtual void Dying() override;

private:
	HRESULT Ready_Components();
	_int m_iPrevBeat = 0;

	_int	m_iShootTimer = 0;
	_float	m_fShootTimerLeft = 0.f;

public:
	static shared_ptr<CKale_FireWall> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END