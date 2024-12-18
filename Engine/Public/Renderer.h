#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CRenderer
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOW, RENDER_TERRAIN, RENDER_DECAL, RENDER_NONBLEND, RENDER_DECAL_ALL ,
		RENDER_BLEND,RENDER_NONBLEND_NON_LIT, RENDER_BLEND_NON_LIT, RENDER_GLOW, RENDER_REFLECTION, RENDER_DISTORTION, RENDER_UI, RENDER_LENS, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CRenderer();

public:
	HRESULT Initialize();
	HRESULT Add_RenderObjects(RENDERGROUP eRenderGroup, shared_ptr<class CGameObject> pRenderObject);
	HRESULT Draw();
	void Clear();
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_Terrain();
	HRESULT Render_Decal();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_Decal_All();
	HRESULT Render_Blend_Non_Lit();
	HRESULT Render_NonBlend_Non_Lit();
	HRESULT Render_Glowing_Object();
	HRESULT Render_Reflection();
	HRESULT Render_Distortion();

	HRESULT Render_UI();
	HRESULT Render_Lens();

	HRESULT Render_To_BackBuffer();

	void	Render_Debug();

	_float4x4* Get_ScreenQuadWorld() { return &m_FullScreenWorld; }
	_float4x4* Get_ScreenQuadProjection() { return &m_ProjMatrix; }

	void Blur_On(const _float2& vCenter,  _float fStrength);
	void Dissolve_On(shared_ptr<class CTexture> pNoiseTexture, const _float& fDissolve, const _float& fRange);

	void Set_Fog(const _float4& vColor, _float fRange = 0.f) { m_fFogRange = fRange, m_vFogColor = vColor; }

#ifdef _DEBUG
	HRESULT Render_RTDebug();

#endif
	HRESULT Render_Deferred();

	HRESULT Render_Glow();
	HRESULT Render_WeightedBlended();
	HRESULT Render_Cross();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	list<shared_ptr<class CGameObject>>			m_RenderObjects[RENDER_END];
	weak_ptr<class CGameInstance> m_pGameInstance;

	shared_ptr<class CVIBuffer_Rect> m_pVIBuffer = nullptr;

	shared_ptr<class CShader> m_pShader = nullptr;
private:

	_float4x4 m_FullScreenWorld{};
	_float4x4 m_HalfScreenWorld{};
	_float4x4 m_QuatScreenWorld{};
	_float4x4 m_ProjMatrix{};
	_float4x4 m_ProjMatrixHalf{};
	_float4x4 m_ProjMatrixQuat{};
	_float2 m_vResolution{};
	enum GLOWPASS
	{
		GP_POWER = 5,
		GP_X = 6,
		GP_Y = 7,
		GP_GLOW = 8,
	};
	enum CROSSPASS
	{
		CP_POWER = 9,
		CROSSPASS = 10
	};
	enum BLUR
	{
		RADIAL_BLUR = 11,
	};

	_float2 m_vRadialBlurCenter{};
	_float  m_fBlurStrength = 0.f;
	_bool	m_bBlur = false;

	shared_ptr<class CTexture> m_pNoise_Texture;
	_float m_fDissolve = 0.f;
	_float m_fDissolveRange = 0.1f;
	_bool m_bDissolve = false;

	ID3D11DepthStencilView* m_pDepthStencilViewQuarter = { nullptr };

	_float4 m_vFogColor = { 0.2f,0.2f,0.2f,1.f };
	_float  m_fFogRange = 0.f;

	_bool m_bDebugRTDraw = false;

#ifndef _DEBUG
#ifdef PERFECTION_LOAD
	vector<ID3D11DepthStencilView*>		m_DSVs;
	vector<ID3D11ShaderResourceView*>	m_SRVs;
	vector<D3D11_VIEWPORT>				m_vecViewPorts;
#endif // !_DEBUG
#endif

	_uint m_iCascade = 1;
	D3D11_VIEWPORT m_ViewPort;
#ifdef PERFECTION_LOAD
	_bool m_bShadow = true;
#else
	_bool m_bShadow = false;
#endif
	_bool m_bStencil = false;


public:
	static shared_ptr<CRenderer> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
};

END