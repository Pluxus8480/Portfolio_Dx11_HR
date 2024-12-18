#pragma once

#include "Client_Defines.h"
#include "CUi_Sprite.h"

BEGIN(Client)

class CUi_Font : public CUi_Sprite
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_FontInfo : public CUi_SpriteInfo
	{
		FontDescInfo Font;

	};

protected:
	CUi_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_Font(const CUi_Font& rhs);

public:
	virtual ~CUi_Font();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Font(FontDescInfo Info);
	void Set_Text(_wstring InText);

public:
	static shared_ptr<CUi_Font> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
