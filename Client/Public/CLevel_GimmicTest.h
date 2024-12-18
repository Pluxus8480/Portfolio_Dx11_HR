#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GimmicTest : public CLevel
{
private:
	CLevel_GimmicTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_GimmicTest();

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Ready_Layer_PlayerAndCamera(_bool CollisionTest);
	HRESULT Ready_Layer_Map(const wstring& strLayerTag);
	HRESULT Ready_Light();
	void Ready_Test();
	void Ready_Setting();
	HRESULT Ready_Layer_Assist();

public:
	void Level_Chanege_Sequenece(_float fTimeDelta);

public:
	static shared_ptr<CLevel_GimmicTest> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
