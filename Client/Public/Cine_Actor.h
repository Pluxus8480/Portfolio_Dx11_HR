#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CAnimation;
END

BEGIN(Client)

class CCine_Actor : public CGameObject
{
public:
	struct DESC {
		string strModelTag;
		string strModelPath;
		string strActorName;
		string strDesc;
	};

	enum class EState {
		NONE, ACTING
	};


public:
	static wstring ObjID;

private:
	CCine_Actor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCine_Actor(const CCine_Actor& rhs);
public:
	virtual ~CCine_Actor();

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

private:
	HRESULT Ready_Components(const wstring& strModelTag);

public:
	void SetPosition(_fvector vInitPos);
	void SetTransform(const _float4x4& _matTrans);

public:
	void GetAnimStrList(vector<string>& listAnim) const;
	void SwitchAnim(const string& strAnim);

	_bool PlayAnimation(_float fFrame, _int iSeqIndex);
	void SetAnimFrame(_float fFrame);

	void GetAnimInfo(shared_ptr<CAnimation>& ppAnim, _uint iAnimIndex);

	HRESULT ExportAnimInfo(const string& strFolderPath);

public:
	void GetBoneList(vector<string>& listBone) const;
	const _float4x4* GetBoneMatrix(const string& strBoneName);

public:
	_float GetAnimDuration(const string& strAnimName);

public:
	const string& GetActorName() const {
		return m_strActorName;
	};
	const string& GetModelTag() const {
		return m_strModelTag;
	};
	const string& GetModelPath() const {
		return m_strModelPath;
	};
	const string& GetDesc() const {
		return m_strDesc;
	};
private:
	string m_strActorName;
	string m_strModelTag;
	string m_strModelPath;
	string m_strDesc;

public:
	const _float4x4 GetRootTrans() const {
		return m_matCurRoot;
	};

private:
	void SetException(const string& strDesc);
private:	// 예외처리
	_float	m_fSize = 0.004f;
	vector<_int>	m_ExcepRenderList;

	_bool m_isMimosa = false;
	_bool m_isMimosa_ExcepEnd = false;
	shared_ptr<CTexture> m_pWingTexture = nullptr;

	_bool m_isKaleArm_Red_On = false;
	vector<_int> m_Before_KaleArm_Red;
	vector<_int> m_After_KaleArm_Red;
	_bool m_isKaleArm_Blue_On = false;
	vector<_int> m_Before_KaleArm_Blue;
	vector<_int> m_After_KaleArm_Blue;

	_bool m_isExcepStart = true;
	string m_strAnim_Excep = "";
	_float m_fFrame_ExcepStart = 0.f;

public:
	EState GetState() { return m_eState; };
	void SetState(EState eState) {
		m_eState = eState;
	};
private:
	EState m_eState = EState::NONE;

private:	// Only for intro door object
	HRESULT RenderIntroDoor(size_t iMeshIndex);
	_uint m_iRenderGroup = 0;

private:	// For effect
	_float4x4 m_matCurrent;
	_float4x4 m_matCurRoot;

	_float m_fEffectSpeed = 1.f;

public:
	static shared_ptr<CCine_Actor> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END