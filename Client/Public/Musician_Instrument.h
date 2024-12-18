#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMusician_Instrument : public CPartObject
{
public:
	static wstring ObjID;

public:
	struct INSTRUMENT_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_uint* pState;
		_uint  iLevelIndex;
		_int   iType;
	};

private:
	CMusician_Instrument(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMusician_Instrument(const CMusician_Instrument& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void Animation(_float fTimeDelta);
	void Play_Instrument();
private:
	shared_ptr <CShader>  m_pShaderCom = { nullptr };
	shared_ptr <CModel> m_pModelCom = { nullptr };

private:
	const _float4x4* m_pBoneMatrix = { nullptr };
	_int   m_iType = 0;

public:
	static shared_ptr<CMusician_Instrument> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
	virtual ~CMusician_Instrument();
};

END