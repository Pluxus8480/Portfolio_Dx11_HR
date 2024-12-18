#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"

BEGIN(Engine)
class CShader;
class CModel;
class CPhysXCollider;
class CCollider;
END

BEGIN(Client)

class BossKaleObjSystem : public CJeongGimmicBase
{
public:
	static wstring ObjID;

private:
	BossKaleObjSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	BossKaleObjSystem(const BossKaleObjSystem& rhs);
public:
	~BossKaleObjSystem();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;


private:
	void Initialize_Obj();
	void Adjust_ObjPos();


private:
	


public:
	static shared_ptr<BossKaleObjSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

END