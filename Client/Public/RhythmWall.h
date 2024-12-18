#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CRhythmWall : public CGameObject, public CMisc_Interface, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	enum MODELINDEX { BLOCK1, BLOCK2, BLOCK3, BLOCK_END };

	struct BLOCKINFO
	{
		_uint iModelIndex = { 0 };
		_float fMaxHeight = 0.f;
		_float fMinHeight = 0.f;
		_float4x4 vLocalMatrix;
		_float4x4 vWorldMatrix;
	};

private:
	CRhythmWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRhythmWall(const CRhythmWall& rhs);
public:
	~CRhythmWall();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom[3] = { nullptr, };

	vector<BLOCKINFO> m_vecPartBlocks;

	_int m_iBlockIndex = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CRhythmWall> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END