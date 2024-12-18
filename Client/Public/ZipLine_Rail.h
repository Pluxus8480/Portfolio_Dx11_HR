#pragma once
#include "Client_Defines.h"
#include "GimmickBase.h"


BEGIN(Client)

class CZipLine_Rail : public CGimmickBase
{
public:
	static wstring ObjID;

private:
	struct OBSDATA
	{
		_int iRailIndex;
		_int iObsDir;
		_float4x4 localMat;
	};

private:
	CZipLine_Rail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CZipLine_Rail(const CZipLine_Rail& rhs);
public:
	~CZipLine_Rail();

public:
	virtual void Set_Dead(bool bDead = true) override;
	virtual void Execute(_bool bExecute = true) override;
	virtual void Done() override;
	virtual void Dying() override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Edit_Attribute() override;
	virtual void Save_Data(ofstream& os) override;
	virtual void Load_Data(ifstream& is) override;

	void RenderOff();

private:
	vector<shared_ptr<class CZipLine_RailPart>> m_vecRailParts;
	list<shared_ptr<class CZipLine_Obstacle>> m_ObstacleList;

	list<shared_ptr<class CZipLine_Obstacle>> m_ObstacleStoreList;

	_int m_iCurRailIndex = {};

	shared_ptr<class CZipLine_Magnet> m_pMagnet = { nullptr };
	shared_ptr<CTransform> m_pMagnetTransform = { nullptr };

	_float3 m_vMagnetDirection = {};
	_float m_fMagnetSpeed = { 25.f };

	_bool m_bObstacle = { false };

	_float m_fLerpTime = { 0.f };
	_bool m_bLerpRot = { false };
	_float3 m_vBeforeLerp = {};
	_float3 m_vAfterLerp = {};

private:
	_int m_iObjectIndex = {};
	string m_strFileName = "MagnetRailA";

	_int m_iObstacleOption = {};
	vector<OBSDATA> m_vecObsData;

	/* Talk Bar */
private:
	_bool m_isTalkOver = { true };
	_bool m_bTalk = { false };

private:
	void Ready_Parts();

public: // 테스트 끝나면 private로 바꾸기
	void Load_Children();

public:
	static shared_ptr<CZipLine_Rail> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();
};

END