#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Boss_InteractObj.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CMimosa_Visualizer : public CGameObject, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CMimosa_Visualizer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMimosa_Visualizer(const CMimosa_Visualizer& rhs);
public:
	~CMimosa_Visualizer();

public:
	virtual void Change_Phase(_uint iPhase) override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	void Visualizer_Update();
	void Visualizer_Init();

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CModel> m_pModelCom = { nullptr };

private:

	_int m_iMaxY = 7;
	_int m_iMaxX = 16;

	vector<vector<_int>> m_iVisualizerArray;
	vector<_int> m_iVisualizerXArray;

	_bool m_bBeatInit = false;
	_float m_fBeatRate = 0.f;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CMimosa_Visualizer> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END