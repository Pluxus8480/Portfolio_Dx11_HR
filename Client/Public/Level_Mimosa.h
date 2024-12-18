#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Mimosa : public CLevel
{
private:
	CLevel_Mimosa(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_Mimosa();

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Terminate() override;

private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_Map(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Light();
	HRESULT Ready_Layer_Assist();

private:
	void FireCrack_Controler(_float fTimeDelta);
	void LandFireCrack_Controler(_float fTimeDelta);
	void StageShine_Controler(_float fTimeDelta);
	void ShowFireCrackTest(_float fTimeDelta);
	void SideFireCrack();
	void FrontFireCrack();

public:
	void Level_Chanege_Sequenece(_float fTimeDelta);

private:
	_float  m_fFireCrack_1_Time = 0.f;
	_float  m_fFireCrack_2_Time = 0.f;
	_float  m_fFireCrack_3_Time = 0.f;

	_float  m_fFireCrack_1_TimeMax = 5.f;
	_float  m_fFireCrack_2_TimeMax = 2.f;
	_float  m_fFireCrack_3_TimeMax = 3.f;

	_float m_fLandFireCrack_Time = 0.f;

	_float m_fLandFireCrack_TimeMax = 2.f;

	_bool m_bFireCrackPattern = false;

	_float m_fStageShine_Time = 0.f;

	_float m_fStageShine_TimeMax = 1.f;

public:
	static shared_ptr<CLevel_Mimosa> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
