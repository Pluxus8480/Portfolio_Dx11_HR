#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

END


BEGIN(Client)

class CChai_Random_Sound : public CGameObject
{
public:
	static wstring ObjID;

private:
	CChai_Random_Sound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChai_Random_Sound(const CChai_Random_Sound& rhs);
public:
	virtual ~CChai_Random_Sound();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Play_Sound(string strSoundName);
	void Play_Random_Attack_Sound_L();
	void Play_Random_Attack_Sound_R();
private:
	HRESULT Ready_Components();

public:
	static shared_ptr<CChai_Random_Sound> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END