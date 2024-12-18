#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Misc_Interface.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMimosaWall : public CGameObject, public CMisc_Interface
{
public:
	static wstring ObjID;

private:
	CMimosaWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMimosaWall(const CMimosaWall& rhs);
public:
	~CMimosaWall();

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
	vector<_float4x4> m_UpperLine;
	vector<_float> m_vecUpperTH;
	vector<_float4x4> m_RightLine;
	vector<_float> m_vecRightTH;
	vector<_float4x4> m_LeftLine;
	vector<_float> m_vecLeftTH;

	_uint m_iPrevBeat = 0;

private:
	void Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CMimosaWall> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END