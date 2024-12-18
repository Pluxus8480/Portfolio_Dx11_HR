#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_KaleBoss : public CLevel
{
private:
	CLevel_KaleBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_KaleBoss();

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


public:
	void Level_Chanege_Sequenece(_float fTimeDelta);

public:
	static shared_ptr<CLevel_KaleBoss> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
