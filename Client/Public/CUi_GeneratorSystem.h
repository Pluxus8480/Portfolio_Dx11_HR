#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_GeneratorSystem : public CUi_2D
{
public:
	static wstring ObjID;


protected:
	CUi_GeneratorSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_GeneratorSystem(const CUi_GeneratorSystem& rhs);


public:
	virtual ~CUi_GeneratorSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual void Dying() override;

	_int Get_Success_Count() const { return  m_iSuccess_Count; }
	_bool Get_Is_Fail() const { return  m_bFail; }

	_bool Generator_Judge();
private:
	void Create_CommandIcon();
	void Create_BackGround();
	void Cal_Dead();
	void Judge_CreateTime(_float fTimeDelta);
	void Adjust_BackSize();
	void Create_Success();


private:
	_uint m_iCreateBeat = { 0 };
	_uint m_iCreateCount = { 0 };
	_float m_fCreateSuccessTime = { 0 };
	vector<_float2> m_fCenterPosVec;
	vector<shared_ptr<class CUi_CommandIcon>> m_pCommandIconVec;
	vector<shared_ptr<class CUi_Default>> m_pBackCircleVec;
	vector<shared_ptr<class CUi_Default>> m_pBackLightVec;
	vector<shared_ptr<class CUi_Default>> m_pSuccessVec;

	_uint m_iSuccess_Count = { 0 };
	_bool m_bFail = false;
public:
	static shared_ptr<CUi_GeneratorSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
