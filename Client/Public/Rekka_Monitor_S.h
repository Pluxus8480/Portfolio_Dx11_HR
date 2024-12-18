#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CRekka_Monitor_S : public CGameObject, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	struct DMGINFO
	{
		XMINT2 vDivide;
		_uint iMaxFrame;
		_float fSecPerFrame;
	};

private:
	CRekka_Monitor_S(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRekka_Monitor_S(const CRekka_Monitor_S& rhs);
public:
	~CRekka_Monitor_S();

public:
	virtual void Change_Pattern(_uint iPattern) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom[(_uint)REKKA_PATTERN::PATTERN_END] = { nullptr, };

	DMGINFO m_DMGInfos[(_uint)REKKA_PATTERN::PATTERN_END] = {};
	_int m_iCurFrame = {};
	_float m_fTime = {};

	_bool m_bInit = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CRekka_Monitor_S> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END