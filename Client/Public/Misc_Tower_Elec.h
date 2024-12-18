#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMisc_Tower_Elec : public CGameObject, public CMisc_Interface, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CMisc_Tower_Elec(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMisc_Tower_Elec(const CMisc_Tower_Elec& rhs);
public:
	~CMisc_Tower_Elec();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Change_Phase(_uint iPhase);
private:
	_uint m_iPrevBeat = 0;


public:
	static shared_ptr<CMisc_Tower_Elec> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END