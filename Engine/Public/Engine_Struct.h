#pragma once


namespace Engine
{
	struct ENGINE_DESC
	{
		HWND			hWnd;
		HINSTANCE		hInstance;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		unsigned int	iLoadingLevelIdx;
		bool			isWindowed;
	};

	struct ENGINE_DLL VTXPOSTEX
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};
	struct ENGINE_DLL VTXNORTEX
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};
	struct ENGINE_DLL VTXMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int	iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct ENGINE_DLL VTXPOINT
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vSize;
	};

	struct ENGINE_DLL VTXMATRIX
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT2		vLifeTime;

		static const unsigned int	iNumElements = 5;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};
	
	struct ENGINE_DLL VTXCUBE
	{
		XMFLOAT3        vPosition;
		XMFLOAT3        vTexcoord;

		static const unsigned int    iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC    Elements[iNumElements];
	};

	struct ENGINE_DLL VTXRECTINSTANCE
	{
		static const unsigned int	iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct ENGINE_DLL VTXPOINTINSTANCE
	{
		static const unsigned int	iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct ENGINE_DLL VTXMESHINSTANCE
	{
		static const unsigned int	iNumElements = 9;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	typedef struct tagVertex_AnimModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;

		tagVertex_AnimModel()
		{
			ZeroMemory(this, sizeof(tagVertex_AnimModel));
		}
	}VTXANIM;

	typedef struct tagVertex_Position
	{
		_float3		vPosition;

		static const unsigned int	iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOS;

	typedef struct tagVertex_Ground_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VTXGROUND;

	typedef struct tagFaceIndices32
	{
		unsigned long _1, _2, _3;
		int	numIndices;
	}FACEINDICES32;

	typedef struct ENGINE_DLL VertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		/* 이 정점은 어떤 뼈들의 상태를 받아와야하는가? 저장의 형태 : 뼈의 인덱스로 저장.*/
		/* 전체 뼈 중의 인덱스를 의미하는게 아니다. */
		/* 이 메시에 영향을 주는 뼈들 중에서의 인덱스를 지정하는거다. */
		XMUINT4			vBlendIndices;

		/* 위에서 정의한 각각의 뼈가 몇 프로나 정점에 적용되야하는지에 대한 정보를 정규화한 실수로 저장한다. */
		XMFLOAT4		vBlendWeights;

		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMESH;

	typedef struct tag_MeshVertextPostion
	{
		_float3 vMin;
		_float3 vMax;
		_float3 vCenter;

	} MESH_VTX_INFO;

	typedef struct
	{
		shared_ptr<class CTexture> pTextures[AI_TEXTURE_TYPE_MAX];
	}MATERIAL_MESH;

	using DELAYED = struct tDelayed
	{
		DELAYED_TYPE eType;
		DWORD_PTR wParam;
		DWORD_PTR lParam;
		shared_ptr<class CLevel> pNextLevel;
	};

	union COLLIDER_ID
	{
		struct {
			_uint Left_ID;
			_uint Right_ID;
		};
		_ull ID;
	};

	enum class KEY_STATE
	{
		DOWN,
		PRESSING,
		UP,
		NONE,
	};
	struct KEY
	{
		bool bPrevPush = false;
		KEY_STATE eKeyState = KEY_STATE::NONE;
	};

	enum class SOUND_CHANNEL
	{
		MUSIC,
		SOUND_EFFECT,
		CHANNEL_END
	};

	typedef struct
	{
		float				fTrackPosition;
		XMFLOAT3			vTranslation;
		XMFLOAT3			vScale;
		XMFLOAT4			vRotation;
		float				fTime;
	}KEYFRAME;

	typedef struct ROOT_ANIM_PRE
	{
		XMFLOAT4			vPreTrans;
		XMFLOAT4			vPreRot;
	}ROOT_ANIM_PRE;

	typedef struct LightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT, TYPE_END };
		TYPE        eType;
		_char        szName[MAX_PATH] = {};
		XMFLOAT4    vDiffuse;
		XMFLOAT4    vAmbient;
		XMFLOAT4    vSpecular;

		XMFLOAT4    vDirection;
		XMFLOAT4    vPosition;
		float        fRange;
		float		g_fIntensity;
		float        g_fSpotInnerConeAngle;
		float        g_fSpotOuterConeAngle;
		float       g_fLightFalloff;


	}LIGHT_DESC;

	typedef struct ENGINE_DLL MapObjData
	{
		_bool			bCollision = { true };
		_uint			iObjType;
		_char			szPrototypeTag[MAX_PATH] = {};
		_float4			Buffer = {};
		_float4x4		matWorld;
		LightDesc		tLightDesc;
	}MAPDATA;

	struct PointLight
	{
		DirectX::XMFLOAT3 position;
		float range;
		DirectX::XMFLOAT3 color;
		float intensity;
		DirectX::XMFLOAT3 specColor;
		float pad;
		DirectX::XMFLOAT4 ambientColor;
	};
	struct DirectionalLight
	{
		DirectX::XMFLOAT3 direction;
		float intensity;
		DirectX::XMFLOAT3 color;
		float pad1;
		DirectX::XMFLOAT3 specColor;
		float pad2;
		DirectX::XMFLOAT4 ambientColor;
	};
	struct SpotLight
	{
		DirectX::XMFLOAT3 position;
		float range;
		DirectX::XMFLOAT3 direction;
		float innerConeCos;
		DirectX::XMFLOAT3 color;
		float outerConeCos;
		float intensity;
		DirectX::XMFLOAT3 specColor;
	};

	const XMFLOAT4X4 Identity = { 1.f,0.f,0.f,0.f,
								  0.f,1.f,0.f,0.f,
								  0.f,0.f,1.f,0.f,
								  0.f,0.f,0.f,1.f };

#pragma region RESOURCE
	typedef struct RESOURCE_INFO
	{
	protected:
		RESOURCE_INFO() = default;

	public:
		enum class E_Res_Type
		{
			NONE, TEXTURE, MODEL, SOUND, SOUND_3D, PARTICLE, SHADER
		};
		E_Res_Type type{ E_Res_Type::NONE };

		std::wstring	strTag = L"";
		std::wstring	strFilePath = L"";
	}RESOURCE_INFO;

	typedef struct RES_TEXTURE : public RESOURCE_INFO
	{
		RES_TEXTURE(const std::wstring& _strTag, const std::wstring& _strFilePath, unsigned int _iNumIndex = 1U)
		{
			type = E_Res_Type::TEXTURE;

			strTag = _strTag;
			strFilePath = _strFilePath;

			iNumTexture = _iNumIndex;
		}

		unsigned int	iNumTexture;
	}RES_TEXTURE;

	typedef struct RES_MODEL : public RESOURCE_INFO
	{
		RES_MODEL(const std::wstring& _strTag, const std::wstring& _strFilePath, MODEL_TYPE eType, _float4x4 _Transform)
		{
			type = E_Res_Type::MODEL;

			strTag = _strTag;
			strFilePath = _strFilePath;

			modelType = eType;
			Transform = _Transform;
		}

		MODEL_TYPE modelType;
		_float4x4 Transform;
	}RES_MODEL;

	typedef struct RES_SOUND : public RESOURCE_INFO
	{
		//enum class ESoundType
		//{
		//	NONE, SOUND_EFFECT, SOUND_BGM
		//};

		RES_SOUND(const std::wstring& _strTag, const std::wstring& _strFilePath, SOUND_CHANNEL _eSoundType = SOUND_CHANNEL::SOUND_EFFECT)
		{
			type = E_Res_Type::SOUND;

			eSoundType = _eSoundType;

			strTag = _strTag;
			strFilePath = _strFilePath;
		}

		SOUND_CHANNEL eSoundType{ SOUND_CHANNEL::CHANNEL_END };
	}RES_SOUND;

	typedef struct RES_SOUND_3D : public RESOURCE_INFO
	{
		//enum class ESoundType
		//{
		//	NONE, SOUND_EFFECT, SOUND_BGM
		//};

		RES_SOUND_3D(const std::wstring& _strTag, const std::wstring& _strFilePath, SOUND_CHANNEL _eSoundType = SOUND_CHANNEL::SOUND_EFFECT)
		{
			type = E_Res_Type::SOUND_3D;

			eSoundType = _eSoundType;

			strTag = _strTag;
			strFilePath = _strFilePath;
		}

		SOUND_CHANNEL eSoundType{ SOUND_CHANNEL::CHANNEL_END };
	}RES_SOUND_3D;

#pragma endregion TYPE_RESOURCES

	enum class EAttackType {
		Base, Hard, Air, Special, SP, ELECTRIC, Fire, Parry,ZiplineHit ,Peppermint, Macaron, Korsica
	};
	enum class EAttackDirection {
		LEFT, RIGHT, UP, DOWN, DEFAULT
	};
	struct DamageInfo
	{
		float					fAmountDamage;
		EAttackType				eAttackType;
		EAttackDirection		eAttack_Direction;
		shared_ptr<class CGameObject>	pObjectOwner;
		string					strAttackName;
	};

	typedef struct PhysXColliderDesc
	{
		PhysXColliderDesc()
		{
			ZeroMemory(this, sizeof(PhysXColliderDesc));
		}

		PxConvexMesh* pConvecMesh;
		PxMaterial* pMaterial;
		PHYSXACTOR_TYPE        eActorType;
		XMVECTOR            vPosition;
		XMVECTOR            vAngles;
		PHYSXCOLLIDER_TYPE    eShape;
		_uint                iFilterType;
		XMVECTOR            vScale;
		float                fDensity;
		_bool                bTrigger;

	}PHYSXCOLLIDERDESC;

}
