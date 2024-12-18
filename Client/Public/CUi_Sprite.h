#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_Sprite : public CUi_2D
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_SpriteInfo : public CUi_2DInfo
	{
		_int iTextureNum;
		_int iMaxFrame;
	};

protected:
	CUi_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_Sprite(const CUi_Sprite& rhs);

public:
	virtual ~CUi_Sprite();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta)override;
	virtual void    LateTick(_float fTimeDelta) override;
	virtual void    PlayAnimation(shared_ptr<class CUI_Animation> InAnimation);

	void StopAnimation();

public:
	void SetVisible(_bool bVisible) { m_bIsVisible = bVisible; }
	_bool IsVisible() const { return m_bIsVisible; }
	_bool IsAnimationPlaying() const { return !m_AnimationList.empty(); }
	_int GetMaxFrame() const { return m_iMaxFrame; }

protected:
	_bool  m_bIsVisible = { true };
	_uint  m_iMaxFrame = { 0 };
	list<shared_ptr<class CUI_Animation>> m_AnimationList; 
	virtual void AnimationTick(_float fTimeDelta); 

public:
	static shared_ptr<CUi_Sprite> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
