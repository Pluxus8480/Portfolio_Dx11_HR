#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public enable_shared_from_this<CLevel>
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CLevel();

public:
	_uint Get_LevelID() const {
		return m_iLevelID;
	}

public:
	virtual HRESULT Initialize();
	virtual void	Terminate();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Change_Level_Delayed(_uint iNextLevel);

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	weak_ptr<class CGameInstance> m_pGameInstance;
	_uint						m_iLevelID = { 0 };
	
	_bool m_bLevelChanged = false;
	_uint m_iNextLevel = 0;

public:
	void Free();
};

END