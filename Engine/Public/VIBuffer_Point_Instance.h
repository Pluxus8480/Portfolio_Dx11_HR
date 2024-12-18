#pragma once

#include "VIBuffer_instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance : public CVIBuffer_Instance
{
private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
public:
	virtual ~CVIBuffer_Point_Instance();

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Update_PtInstance_Style(_float fTimeDelta);
	virtual void Update_Spread(_float fTimeDelta) override;
	virtual void Update_Drop(_float fTimeDelta) override;
	virtual void Update_Fountain(_float fTimeDelta) override;
	virtual void Update_Rising(_float fTimeDelta) override;
	virtual void Update_Cycle(_float fTimeDelta) override;
	virtual void Update_Orbital(_float fTimeDelta) override;
	virtual void Reset() override;

public:
	void Set_ParentMatrix(const _matrix pParentMatrix) { m_pParentMatrix = pParentMatrix; m_bParentUse = true; }
	void Clear_ParentMatrix() { m_pParentMatrix = XMMatrixIdentity(); m_bParentUse = false; }

	void Set_BillBoardMatrix(const _matrix pBillBoardMatrix) { m_pBillBoardMatrix = pBillBoardMatrix; m_bBillboardUse = true; }
	void Clear_BillBoardMatrix() { m_pBillBoardMatrix = XMMatrixIdentity(); m_bBillboardUse = false; }

private:
	VTXMATRIX* m_pInstanceVertex = nullptr;
	_matrix m_pParentMatrix = XMMatrixIdentity();
	_bool   m_bParentUse = false;
	_matrix m_pBillBoardMatrix = XMMatrixIdentity();
	_bool   m_bBillboardUse = false;
public:
	static shared_ptr<CVIBuffer_Point_Instance> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	void Free();
};

END