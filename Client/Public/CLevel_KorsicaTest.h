#pragma once

#include "Client_Defines.h"
#include "Level.h"


BEGIN(Client)

class CLevel_KorsicaTest : public CLevel
{
private:
	CLevel_KorsicaTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_KorsicaTest();

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


public:
	void Level_Chanege_Sequenece(_float fTimeDelta);

public:
	static shared_ptr<CLevel_KorsicaTest> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
