#pragma once

#include "Client_Defines.h"
#include "CUi_2D.h"

BEGIN(Client)

class CUi_SkillCutScene : public CUi_2D
{
public:
	enum class WordType {
		PowerCode, GuitaRride, PickMeUp, PickSlide, HighPitchPunch, AirGuitar,
		RipAndTear, DoubleLeaf, BarrierWall, TwinAssualt, StealTheShow,
		HoloChai, HiBiKi, OverDriveSlash, GigaWath, End
	};

	enum class CutSceneType { Chai, ChaiCat, ChaiCNMN, ChaiKorsica, ChaiMacaron, ChaiPepper, End };

public:
	static wstring ObjID;


public:
	using Desc = struct CUi_SkillCutSceneInfo : public CUi_2DInfo
	{
		WordType Wordtype = WordType::End;
		CutSceneType CutScenetype = CutSceneType::End;
	};

protected:
	CUi_SkillCutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_SkillCutScene(const CUi_2D& rhs);


public:
	virtual ~CUi_SkillCutScene();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);


private:
	void Initialize_Back();
	void Initialize_CutScene(CutSceneType Scene);
	void Initialize_Word(WordType Word);
	void Moving(_float fTimeDelta);
	virtual void Dying() override;


private:
	_float2 m_fDestPos = { 400, 0 };
	_float2 m_fOriginPos = { 800, 200 };
	_float m_fDieTime = { 0 };


private:
	shared_ptr<class CUi_Default> m_pCutScene;
	shared_ptr<class CUi_Default> m_pWord;
	shared_ptr<class CUi_Default> m_pBack;


public:
	static shared_ptr<CUi_SkillCutScene> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
