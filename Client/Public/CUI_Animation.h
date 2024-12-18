#pragma once

#include "Client_Defines.h"
#include "CUi_Sprite.h"

BEGIN(Client)

class CUI_Animation
{
public:
	explicit CUI_Animation();
	virtual ~CUI_Animation();

public:
	virtual _bool Tick(_float fTimeDelta);
	virtual void OnPlayAnimationEvent(shared_ptr<CGameObject> pOwner);

protected:
	shared_ptr<CUi_Sprite> m_pOwner = { nullptr };
};

END