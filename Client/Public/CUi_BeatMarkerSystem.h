#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_BeatMarkerSystem : public CUi_2D
{
public:
	static wstring ObjID;


public:
	enum class Type { Default, Rekka, Mimosa, Kale, End };
	using Desc = struct CUi_BeatMarkerSystemInfo
	{
		Type eType = { Type::Default };
	};

public:
	enum class JudgeType { Perfect, Good, Miss, End };


protected:
	CUi_BeatMarkerSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_BeatMarkerSystem(const CUi_BeatMarkerSystem& rhs);


public:
	virtual ~CUi_BeatMarkerSystem();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Create_BeatMarker();
	void Check_Beat();
	void Adjust_Beat(_float fTimeDelta);
	string Cal_Judge();
	void Create_Judge(JudgeType Type);
	void Delete_Marker();
	void Adjust_InitializeBeat();

private:
	_float Cal_Speed();
	void Initialize_PosY();
	void Adjust_DefaultVec(_float fTimeDelta);
	void DefaultVec_ColorAdjust(_float fTimeDelta, _bool Decrease);
	void DefaultVec_ScaleAdjust(_float fTimeDelta, _bool Decrease);
	void Initialize_DefaultVec();
	virtual void Dying() override;
	virtual void Set_Active(_bool Active) override;


private:
	_bool m_bBeatChange = { false };
	_uint m_iCheckBeat = { 0 };
	_float m_fBeatMarkerSizeX = { 40 };
	_float m_fBeatMarkerPosX = { 450 };
	_float m_fPosY = { 0 };
	Type m_eType = { Type::Default };


private:
	list<shared_ptr<class CUi_BeatMarker>> m_pMarkerList;
	vector<shared_ptr<class CUi_Default>> m_DefaultVec;


public:
	static shared_ptr<CUi_BeatMarkerSystem> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
