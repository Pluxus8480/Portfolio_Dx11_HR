#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_KaleStage : public CLevel
{
private:
	CLevel_KaleStage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	~CLevel_KaleStage();

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
	HRESULT Ready_Layer_Assist();
	HRESULT Ready_Layer_Bgm();
	HRESULT Ready_Light();
	void Ready_Ui();


public:
	void Level_Chanege_Sequenece(_float fTimeDelta);

private:
	_bool   m_bTalkCol = false;
	_bool   m_isTalkOver = true;

public:
	static shared_ptr<CLevel_KaleStage> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END
