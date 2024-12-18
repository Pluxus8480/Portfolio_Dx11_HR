#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_KorsicaSystem : public CUi_2D
{
public:
	static wstring ObjID;


public:
	using Desc = struct CUi_KorsicaSystemInfo : public CUi_2DInfo
	{
		_float* CorsicaHp = { nullptr };
	};


protected:
	CUi_KorsicaSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_KorsicaSystem(const CUi_KorsicaSystem& rhs);


public:
	virtual ~CUi_KorsicaSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);



private:
	void Initialize_FaceVec();
	void Initialize_HpBack();
	void Initialize_Hp();
	void Initialize_Shadow();


private:
	void Renew_Hp();
	void Shadow_Move(_float fTimeDelta);
	virtual void Dying() override;
	virtual void Set_Active(_bool Active) override;


private:
	vector<shared_ptr<class CUi_KorsicaFace>> m_pFaceVec;
	shared_ptr<class CUi_Default> m_pChaiHpBack;
	shared_ptr<class CUi_Default> m_pKorsicaHpBack;
	shared_ptr<class CUi_KorsicaHpBar> m_pChaiHp;
	shared_ptr<class CUi_KorsicaHpBar> m_pKorsicaHp;
	shared_ptr<class CUi_Default> m_pKorsicaShadow;
	shared_ptr<class CUi_Default> m_pChaiShadow;
	//shared_ptr<>
	_float m_fKorsicaMaxHp = { 100 };
	_float* m_fKorsicaHp = { nullptr };
	_float m_fShadowOriginY = { 240 };


public:
	static shared_ptr<CUi_KorsicaSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
