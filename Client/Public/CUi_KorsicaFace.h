#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_KorsicaFace : public CUi_2D
{
public:
	static wstring ObjID;

public:
	using Desc = struct CUi_KorsicaFaceInfo : public CUi_2DInfo
	{
		_uint BackTextureNum = { 0 };
	};

protected:
	CUi_KorsicaFace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_KorsicaFace(const CUi_KorsicaFace& rhs);


public:
	virtual ~CUi_KorsicaFace();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual void Set_Active(_bool Active) override;


private:
	void Initialize_BackTexture();
	void Initialize_FaceBack();


private:
	virtual void Dying() override;


private:
	shared_ptr<CTexture> m_pBackTextureCom = { nullptr };
	shared_ptr<class CUi_Default> m_pBack;
	_uint m_pBackTextureNum = { 0 };


public:
	static shared_ptr<CUi_KorsicaFace> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
