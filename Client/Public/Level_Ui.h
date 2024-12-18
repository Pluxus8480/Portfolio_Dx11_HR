#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Ui : public CLevel
{
private:
	CLevel_Ui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_Ui();

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_Map(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Light();
	void Ready_Ui();

private:
	_bool m_isTalkOver = true;

	_float m_fTime = { 0 };
public:
	void Level_Chanege_Sequenece(_float fTimeDelta);

public:
	static shared_ptr<CLevel_Ui> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
