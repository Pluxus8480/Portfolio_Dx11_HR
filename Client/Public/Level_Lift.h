
#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CEnemy_Lift;
class CLift;

class CLevel_Lift : public CLevel
{
private:
	CLevel_Lift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_Lift();

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Terminate() override;

private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_Map(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Layer_Bgm();
	HRESULT Ready_Light();
	void Ready_Ui();


public:
	_uint m_iPrevBeat = 0;
	_bool m_bStart = false;
	_uint m_iWaveCount = 0;
	_uint m_iPanelCount = 0;
	_uint m_iEndingCount = 0;

	_bool m_bTalk = { false };
	_bool m_isTalkOver = { true };
	_bool m_bEnd = false;
	shared_ptr<CEnemy_Lift> m_pEnemyLift;
	shared_ptr<CLift>		m_pLift;
	
	void Level_Chanege_Sequenece(_float fTimeDelta);
	

public:
	static shared_ptr<CLevel_Lift> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
