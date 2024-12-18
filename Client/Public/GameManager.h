#pragma once
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CShader;
END

BEGIN(Client)

class CGame_Manager
{
	DECLARE_SINGLETON(CGame_Manager)

private:
	CGame_Manager();

public:
	~CGame_Manager();

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Tick_Engine(_float fTimeDelta);

	void Render();
	void Dissolve(_float fTime, _float fRange = 0.01f);

	void Set_Current_Level(_uint iLevel) { m_iLevel = iLevel; }
	_uint Get_Current_Level()const { return m_iLevel; }

	void Bind_DefaultNormal(shared_ptr<CShader> pShader, const char* pTag);

public:
	void DebugMode();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_bool m_bDebugMode = false;
	_uint m_iLevel = LEVEL_END;
	_float m_fDissolveMax = 0.f;
	_float m_fDissolveTimer = 0.f;
	_float m_fDissolveRange = 0.f;
	shared_ptr<CTexture> m_pDissolveNoise;

	shared_ptr<CTexture> m_pDefaultNormal;

public:
	void Free();
};

END