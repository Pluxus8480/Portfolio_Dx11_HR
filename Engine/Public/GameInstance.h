#pragma once
#include "Engine_Defines.h"
#include "Renderer.h"
#include "PipeLine.h"
#include "Collision_Manager.h"
#include "Octree.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance : public enable_shared_from_this<CGameInstance>
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
public:
	virtual ~CGameInstance();

public:
	HRESULT Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear(_uint iLevelIndex);
	_float2 Get_Resolution() const { return _float2{ (_float)m_iWinSizeX, (_float)m_iWinSizeY }; }
	HWND	Get_hWnd() const { return m_hWnd; }
	shared_ptr<class CPhysxManager> Get_PhysxManager() { return m_pPhysxManager; }

	_uint Get_Loading_Level_ID()const { return m_iLoadingLevelIdx; }

#pragma region GRAPHIC_DEVICE
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Clear_RTVs(_float4 vClearColor);
	HRESULT Bind_BackBuffer();
	HRESULT Bind_RTVs();
	HRESULT Present();

	HRESULT Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag);

	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool bClear = true);
	HRESULT End_MRT();
	shared_ptr<class CRenderTarget> Find_RenderTarget(const _wstring& strRenderTargetTag);
	list<shared_ptr<class CRenderTarget>>* Find_MRT(const _wstring& strMRTTag);
	HRESULT Bind_RT_SRV(const _wstring& strRTTag, shared_ptr<class CShader> pShader, const _char* pConstantName);
	void Map_Picking_Screen();

	_uint Pick_Screen(_uint iX, _uint iY);

	void Unmap_Picking_Screen();

	shared_ptr<CGameObject> Pick_Object(_uint iX, _uint iY);

	_float4 Pick_Position(_uint iX, _uint iY);

	HRESULT Bind_Default_ViewPort();
#ifdef _DEBUG
	HRESULT Ready_RTDebug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RTDebug(const _wstring& strMRTTag, shared_ptr<class CShader> pShader, const _char* pConstantName, shared_ptr<class CVIBuffer_Rect> pVIBuffer);
#endif

#pragma region LEVEL_MANAGER
public:
	HRESULT Change_Level(shared_ptr<class CLevel> pNewLevel);
	void Change_Level_Delayed(_uint iNextLevel);
	_uint	Get_Current_LevelID();
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	HRESULT Add_Prototype(const wstring& strPrototypeTag, shared_ptr<class CGameObject> pPrototype);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	shared_ptr<CGameObject> Clone_Object(const wstring& strPrototypeTag, void* pArg);
	shared_ptr<class CComponent> Find_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);
	shared_ptr<CGameObject> Add_Clone_Return(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Push_Object(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<CGameObject> pObject);
	shared_ptr<class CLayer> Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);

	shared_ptr<CGameObject> Get_Object_Prototype(const wstring& strPrototypeTag);
	shared_ptr<CComponent> Get_Component_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

#pragma endregion

#pragma region OBJECT_POOL
public: //Object Pool
	HRESULT Pool_Object(const wstring& strPrototypeTag, void* pArg = nullptr);
	shared_ptr<class CGameObject> Pool_Object_Return(const wstring& strPrototypeTag, void* pArg = nullptr);
	shared_ptr<CGameObject> Push_Object_From_Pool(const wstring& strPrototypeTag, _uint iLevelIndex, const wstring& strLayerTag, void* pArg = nullptr, _float4x4* TransformMat = nullptr);
	shared_ptr<CGameObject> Get_Object_From_Pool(const wstring& strPrototypeTag, void* pArg = nullptr, _float4x4* TransformMat = nullptr);
	void	Push_ObjectsPool(const wstring& strPrototypeTag, _uint iLevelIndex, const wstring& strLayerTag, void* pArg = nullptr, _float4x4* TransformMat = nullptr);
	void	Clear_Pool();
	void	Recall_Object_To_Pool();
	void	Recall_Object_To_Pool(const wstring& strPrototypeTag);
	void	Clear_Pool(_wstring strTag);
#pragma endregion

#pragma region RENDERER
public:
	HRESULT Add_RenderObjects(CRenderer::RENDERGROUP eRenderGroup, shared_ptr<class CGameObject> pRenderObject);
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();
	_float4x4* Get_ScreenQuadWorld();
	_float4x4* Get_ScreenQuadProjection();
	void Blur_On(const _float2& vCenter, _float fStrength);
	void Dissolve_On(shared_ptr<class CTexture> pNoiseTexture, const _float& fDissolve, const _float& fRange);
	void Set_Fog(const _float4& vColor, _float fRange = 0.f);
#pragma endregion

#pragma region COMPONENT_MANAGER
public:
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<class CComponent> pPrototype);
	shared_ptr<class CComponent> Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region RESOURCE_CONTAINER
public:
	shared_ptr<class CTexture> Auto_Search_Texture(const wstring& strTextureTag);
#pragma endregion

#pragma region DELAYED_PROCESS
public:
	void	Add_Delayed(DELAYED tEvent);

	//Do not use External Logic
	void	Push_Dead_Obj(shared_ptr<class CGameObject> _pDeadObj);
#pragma endregion

#pragma region PIPELINE
public: //PipeLine

	void    Set_Transform(CPipeLine::PIPELINE_STATE eState, _fmatrix Matrix);
	void    Set_Transform(CPipeLine::PIPELINE_STATE eState, const _float4x4& Matrix);

	_fmatrix  Get_Transform_Matrix(CPipeLine::PIPELINE_STATE eState) const;
	const _float4x4& Get_Transform_Float4x4(CPipeLine::PIPELINE_STATE eState) const;

	void    Set_ShadowDir(_fvector vDir);
	void    Set_ShadowDir(const _float4& vDir);

	_fmatrix  Get_LightView_Matrix() const;
	const _float4x4& Get_LightView_Float4x4() const;
	_fmatrix  Get_LightProj_Matrix() const;
	const _float4x4& Get_LightProj_Float4x4() const;

	const _float4& Get_CamPosition_Float4() const;
	_fvector Get_CamPosition_Vector() const;

	const _float4& Get_CamLook_Float4() const;
	_fvector Get_CamLook_Vector() const;

	void     Add_Camera(const _wstring& strTag, shared_ptr<class CCamera> pCamera);
	shared_ptr<class CCamera> Get_Camera(const _wstring& strTag);
	shared_ptr<class CCamera> Get_MainCamera();
	void     Set_MainCamera(const _wstring& strTag);
	void     Remove_Camera(const _wstring& strTag);
	void     Clear_Camera();

	void Update_Pipeline();
#pragma endregion

#pragma region INPUT_DEVICE
public: // Input Device
	_byte	Get_DIKeyState(_ubyte byKeyID)	const;

	bool	Get_KeyDown(_ubyte byKeyID) const;
	bool	Get_KeyPressing(_ubyte byKeyID) const;
	bool	Get_KeyUp(_ubyte byKeyID) const;
	bool	Get_KeyNone(_ubyte byKeyID) const;
	bool	Get_MouseDown(MOUSEKEYSTATE byKeyID) const;
	bool	Get_MousePressing(MOUSEKEYSTATE byKeyID) const;
	bool	Get_MouseUp(MOUSEKEYSTATE byKeyID) const;
	bool	Get_MouseNone(MOUSEKEYSTATE byKeyID) const;
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) const;
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
#pragma endregion

#pragma region LIGHT_MANAGER
	HRESULT Add_Light(void* LightDesc);
	shared_ptr<class CLight> Add_Light_Return(void* LightDesc);
	HRESULT Bind_Lights(shared_ptr<class CShader> pShader);
	void LightControl(string strLightTag, _bool bOn);
	void LightControl(string strLightTag);
	shared_ptr<class CLight> Return_Light(string strLightTag);
	void Clear_Light();
	void Remove_Light(string strLightTag);
	void Rewrite_Description(string strLightTag, const LIGHT_DESC& Desc);
	_float* Get_LightCullingPtr();
#pragma endregion

#pragma region COLLISION
	void CollisionClear();

	void Add_Collider(COLLIDER_GROUP _colliderType, shared_ptr<class CCollider> pCollider);
	void CollisionAccessBoth(COLLIDER_GROUP eDst, COLLIDER_GROUP eSrc);
	void CollisionAccess(COLLIDER_GROUP eDst, COLLIDER_GROUP eSrc);
	void CollisionDenyBoth(COLLIDER_GROUP eDst, COLLIDER_GROUP eSrc);
	void CollisionDeny(COLLIDER_GROUP eDst, COLLIDER_GROUP eSrc);
	void CollisionFlagClear();
#pragma endregion

#pragma region PHYSX
	public: //Physx
		PxScene* Get_Scene() const;
		PxControllerManager* Get_CCT_Manager() const;
		PxPhysics* Get_Physics() const;
		PxFoundation* Get_Foundation() const;
		HRESULT Add_Shape(const wstring& strTag, const vector<_float3>& vecPosition, const vector<_uint>& vecIndicies);
		PxShape* Get_Shape(const wstring& strTag);
		void Simulate_Physx(_bool bSim = true);
		bool PxSweep(PxTransform& StartTransform, PxVec3& vDirection, _float fDistance, _float fRadius, PxSweepBuffer& HitOut);
		//PxCudaContextManager* Get_CudaContextManager();
#pragma endregion
#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const wstring& strTimerTag);
	HRESULT Add_Timer(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);

	void Set_TimeMultiplier(_float _fTimeMultiplier);
	_float Get_TimeMultiplier();
#pragma endregion

#pragma region SOUND_MANAGER
public: // SoundManager
	void RegisterToBGM(shared_ptr<class CSound> _pSound);
	void Set_BGM_Tag(const wstring& strTag);
	wstring Get_BGM_Tag() const;
	void Stop_BGM();
	void Start_BGM();
	bool IsBGMExist();
	bool IsBGMPlaying();


	bool Is_Sound_Playing(SOUND_CHANNEL eChannel, const wstring& strSoundTag);
	bool Is_Sound_3D_Playing(SOUND_CHANNEL eChannel, const wstring& strSoundTag);

	HRESULT Add_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, const wstring& strSoundPath);
	HRESULT Add_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, const wstring& strSoundPath);

	void Set_Master_Volume(float fVolume);
	void Set_Channel_Volume(SOUND_CHANNEL eChannel, float fVolume);

	void Play_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bLoop = false);
	void Play_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, _fvector vPos, _fvector vVel = XMVectorSet(0.f, 0.f, 0.f, 0.f), bool bLoop = false);
	
	void Play_Unique_Sound_Channel(string ChannelName, SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bLoop = false);
	void Add_Unique_Sound_Channel(string ChannelName);

	void Pause_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bPausing = true);
	void Pause_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag, bool bPausing = true);

	void Stop_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag);
	void Stop_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag);

	void Set_Sound_Volume(SOUND_CHANNEL eChannel, const wstring& strSoundTag, float fVolume);

	void Set_Master_Speed(float fSpeed);
	void Set_Channel_Speed(SOUND_CHANNEL eChannel, float fSpeed);
	void Set_Sound_Speed(SOUND_CHANNEL eChannel, const wstring& strSoundTag, float fSpeed);

	void Set_BGM_Position(_float fPosition);
	void Set_Sound_Position(SOUND_CHANNEL eChannel, const wstring& strSoundTag, _float fPosition);

	shared_ptr<class CSound> Find_Sound(SOUND_CHANNEL eChannel, const wstring& strSoundTag);
	shared_ptr<class C3D_Sound>Find_Sound_3D(SOUND_CHANNEL eChannel, const wstring& strSoundTag);

	void Set_Listener(shared_ptr<class CGameObject> pListener);
	void Set_3DSound_Coord(SOUND_CHANNEL eChannel, const wstring& strSoundTag, _fvector vPos);

#pragma endregion

#pragma region MATH_MANAGER
public:
	_float2 Lissajous_Curve(IN _float _fTimeDelta, IN OUT _float& _fLissajousTime, IN _float _fWitth, IN _float _fHeight, IN _float _fLagrangianX, IN _float _fLagrangianY, IN _float _fPhaseDelta, IN _float _fLissajousSpeed);
	_float3 Vector3x3_BezierCurve(IN _float3 _vStart, IN _float3 _vControl, IN _float3 _vEnd, IN OUT _float& _fTimeControl, IN _float _fTimeDelta, IN _float _fSpeed = 1.f);

	_float  Random_Float(_float _fMin, _float _fMax);
	_float2 Random_Float2(_float2 _f2Min, _float2 _f2Max);
	_float3 Random_Float3(_float3 _f3Min, _float3 _f3Max);

	_int	Random_Int(_int _iMin, _int _iMax);
	_vector XMQuaternionRotationBetweenVectors(_vector start, _vector end);
	void QuaternionToEulerAngles(const XMVECTOR& quaternion, float& yaw, float& pitch, float& roll);
	_int	Random_Binary();
	_bool	Random_Bool();

	PxVec3  Add_PxVec3toXMVECTOR(PxVec3 _Pxvec3, _vector _Xmvec);
	_float4x4 Matrix_BillBoard(const _float4x4& _matWorld);

	_bool IsEqualWithEpsilon(_vector vec1, _vector vec2, float epsilon = 1e-6f);
#pragma endregion


#pragma region FONT_MANAGER
public:
	HRESULT Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath);
	HRESULT Render_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float rotation = 0.f, _float fScale = 1.f);
	HRESULT Render_Font(const _wstring& strFontTag, const _wstring& strText, _fvector vPosition, _fvector vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float rotation = 0.f, _float fScale = 1.f);
#pragma endregion
#pragma region FRUSTUM
public://Frustum
	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix);
	_bool isIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRange);
	_bool isIn_Frustum_WorldSpaceEX(_fvector vWorldPos, _float fRange);
	_bool isIn_Frustum_LocalSpace(_fvector vLocalPos, _float fRange);
	_bool isIn_Frustum_LightSpace(_fvector vWorldPos, _float fRange);
#pragma endregion
#pragma region OCTREE
public://Octree
	HRESULT Initialize_Octree(_float fUnitScale, _int iMaxRecursion, const _float3& vCenter);
	shared_ptr<COctree::Node> Get_Node();
	void Clear_Octree();
	void Start_Octree();
	void Stop_Octree();
#pragma endregion
private:
	shared_ptr<class CGraphic_Device>			m_pGraphic_Device = { nullptr };
	shared_ptr<class CLevel_Manager>			m_pLevel_Manager = { nullptr };
	shared_ptr<class CRenderer>					m_pRenderer = { nullptr };
	shared_ptr<class CObject_Manager>			m_pObject_Manager = { nullptr };
	shared_ptr<class CObject_Pool>				m_pObject_Pool = { nullptr };
	shared_ptr<class CComponent_Manager>		m_pComponent_Manager = { nullptr };
	shared_ptr<class CResource_Container>		m_pResource_Container = { nullptr };
	shared_ptr<class CDelayedProcessManager>	m_pDelayedProcessManager = { nullptr };
	shared_ptr<class CPipeLine>					m_pPipeLine = { nullptr };
	shared_ptr<class CInput_Device>				m_pInput_Device = nullptr;
	shared_ptr<class CLight_Manager>			m_pLightManager = nullptr;
	shared_ptr<class CCollision_Manager>		m_pCollisionManager = nullptr;
	shared_ptr<class CFrustum>					m_pFrustum = nullptr;
	shared_ptr<class CPhysxManager>				m_pPhysxManager = nullptr;
	shared_ptr<class CTimer_Manager>			m_pTimer_Manager = nullptr;
	shared_ptr<class CSoundManager>				m_pSoundManager = nullptr;
	shared_ptr<class CMath_Manager>				m_pMath_Manager = { nullptr };
	shared_ptr<class CFont_Manager>				m_pFont_Manager = { nullptr };
	shared_ptr<class COctree>					m_pOctree = { nullptr };

	_uint	m_iWinSizeX = 0;
	_uint	m_iWinSizeY = 0;
	HWND	m_hWnd = NULL;
	HINSTANCE m_hInstance = NULL;
	_uint   m_iLoadingLevelIdx = 0;

public:
	static void Release_Engine();
	void Free();
};

END