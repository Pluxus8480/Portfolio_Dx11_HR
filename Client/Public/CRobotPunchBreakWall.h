#pragma once
#include "Client_Defines.h"
#include "CJeongGimmicBase.h"
#include "Boss_InteractObj.h"

//0 = ·Îº¿ÁÖ¸Ô Ä¡±â Àü
//1 = ·Îº¿ÁÖ¸Ô Ä¥¶§
//2 = ·Îº¿ÁÖ¸Ô Ä£ ÈÄ
class CRobotPunchBreakWall : public CJeongGimmicBase, public CBoss_InteractObj
{
public:
	static wstring ObjID;

private:
	CRobotPunchBreakWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRobotPunchBreakWall(const CRobotPunchBreakWall& rhs);


public:
	~CRobotPunchBreakWall();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	shared_ptr<CModel> m_pBeforeBreakModelCom = { nullptr };
	shared_ptr<CModel> m_pBeforeBreakSurroundModelCom = { nullptr };
	shared_ptr<CModel> m_pSurroundModelCom = { nullptr };
	_uint m_iAnimIndex = { 0 };
	_bool m_bRender = { true };


public:
	static shared_ptr<CRobotPunchBreakWall> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	virtual void Free();
};

