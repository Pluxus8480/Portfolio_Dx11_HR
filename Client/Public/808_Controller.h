#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "808_Cat.h"
#include "808_Companion.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
END


BEGIN(Client)

class C808_Controller : public CGameObject
{
public:
	static wstring ObjID;
private:
	C808_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C808_Controller(const C808_Controller& rhs);
public:
	virtual ~C808_Controller();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

private:
	HRESULT Ready_Components();

public:
	static shared_ptr<C808_Controller> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();

public:
	void Active_808_Sphere_Mode();
	void Active_808_Cat_Mode();
	void Active_808_Shop_Animation();
	void Active_808_Shop_Animation_End();
public:
	shared_ptr<C808_Companion> Get_808_Companion() { return m_p808_Companion; }
	shared_ptr<C808_Cat> Get_808_Cat() { return m_p808_Cat; }

private:
	shared_ptr<C808_Companion> m_p808_Companion = nullptr;
	shared_ptr<C808_Cat> m_p808_Cat = nullptr;
};

END