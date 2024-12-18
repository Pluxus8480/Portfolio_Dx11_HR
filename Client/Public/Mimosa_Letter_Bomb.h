#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)
class CEffect_Union;

class CMimosa_Letter_Bomb : public CGameObject
{
public:
	static wstring ObjID;
public:
	struct MiMOSA_LETTER
	{
		_int iLetterModel = -1;
		_float4 fCenterPos = {};
		_float4 fMimosaLookDir = {};
		_float fShootDelay = {};
	};
	enum class LETTER_STATE
	{
		UP, SPIN, SHOOT, EXPLODE ,LETTER_STATE_END
	};
private:
	CMimosa_Letter_Bomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMimosa_Letter_Bomb(const CMimosa_Letter_Bomb& rhs);
public:
	virtual ~CMimosa_Letter_Bomb();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Push_From_Pool(void* pArg) override;
	virtual void Dying() override;

private:
	HRESULT Ready_Components();

private:
	_int	m_iLetterModel = -1;
	_float4 m_fCenterPos = {};
	_float4 m_fMimosaLookDir = {};
	_float4 m_fShootDir = {};
	_float m_fShootDistance = {};
	_float m_fExplode_Delay = 0.f;
	_bool m_bDown = false;
	_bool m_bShooting = false;

	_float m_fShoot_Delay = 0.f;
	LETTER_STATE m_eLetter_State = LETTER_STATE::LETTER_STATE_END;

	shared_ptr<CEffect_Union> m_pEffect = {};
public:
	static shared_ptr<CMimosa_Letter_Bomb> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END