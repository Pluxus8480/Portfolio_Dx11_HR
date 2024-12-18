#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CPartObject;
class CCharacterController;
END


BEGIN(Client)

class CWire;
class CCamera_Player;

class CChai : public CGameObject
{
public:
	static wstring ObjID;

	enum class SkillType { Attack, Recover, End };
	using SkillDesc = struct PlayerSkillInfo
	{
		wstring SkillName = L"";
		SkillType SkillType = { SkillType::Attack};
		_uint ConsumeReverb = { 1 };
	};
	struct AttacK_Tree
	{
		AttacK_Tree* parent; // 부모 노드
		AttacK_Tree* children[3]; // 자식 노드들

		// 노드의 데이터 (예: 3D 모델의 변환 행렬, 이름 등)
		_bool bActivate;
		_uint iCombo_Level;
		string name;

		// 생성자
		AttacK_Tree(const std::string& nodeName) : parent(nullptr), name(nodeName)
		{
			for (size_t i = 0; i < 3; i++)
			{
				children[i] = nullptr;
			}
			iCombo_Level = 0;
			bActivate = false;
		}

		~AttacK_Tree()
		{
			// 자식 노드들을 해제
			for (AttacK_Tree* child : children)
			{
				if(child)
					delete child;
			}
		}
	};
private:
	enum class CHAI_MOVE_STATE {
		 RUN_START, RUN, DASH, JUMP,DOUBLE_JUMP, IDLE, DAMAGED , JUMP_FALL, DOUBLE_JUMP_FALL,ATTACK, 
		 AIR_ATTACK,PARRY,MAGNET,EVENT,MOVE_END
	};
	enum class CHAI_EVENT_STATE {
		EVENT_NONE, GENERATOR, RHYTHMPARRY, ZIPLINE, MIMOSA_DANCE, JUMPPAD, EVENT_STATE_END
	};
	enum ATTACK_TREE_TYPE {
		L, R, REST, ATTACK_TYPE_END
	};
private:
	CChai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChai(const CChai& rhs);
public:
	virtual ~CChai();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();

	void	Change_Level();
	virtual void Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller);
	virtual void TakeDamage(const DamageInfo& damageInfo);

	void Play_808_Just_Effect();
	void Set_Player_Position(_vector vPosition);
	void	Catch_Item(string strItemName, _int iSize);

	void Set_Player_Camera(shared_ptr<CCamera_Player> pCamera) { m_pPlayer_Camera = pCamera; }
	shared_ptr<CCamera_Player> Get_Player_Camera() const {return m_pPlayer_Camera; }

	void Set_Root_Move(_bool bRootMove) { m_bRootMove_On = bRootMove; }
	_bool Get_Root_Move() const { return m_bRootMove_On; }

	void Set_Event_Position(_float4 fEventPos) { m_fEventPosition = fEventPos; }
	_float4 Get_Event_Position() const { return m_fEventPosition; }
	
	void Set_MaxHp(_float fMaxHp) { m_fMaxHp = fMaxHp; }
	_float Get_MaxHp() const { return m_fMaxHp; }

	void Set_MaxReverb(_float fMaxReverb) { m_fMaxReverb = fMaxReverb; }
	void Set_Reverb(_float fReverb) { if (m_fMaxReverb >= fReverb) { m_fReverb = fReverb; } }
	_float Get_MaxReverb() const { return m_fMaxReverb; }

	void Set_Interaction(_bool bInteraction) { m_bInteraction = bInteraction; }

	_bool Is_OnAir() const { return m_bOnAir; }

	string Get_CurrCombo() const { return m_strCombo; }
	shared_ptr<CGameObject>  Get_Command_Icon() const { return m_pCommandIcon; }

	void Set_2D_Mode(_bool b2D_Mode) { m_b2D_Mode = b2D_Mode; }
	void Set_2D_Fixed_Pos(_float f2D_FixedPos) { m_f2D_FixedPos = f2D_FixedPos; }
	void Set_2D_Fixed_XYZ(string strXYZ) { m_strFixedXYZ = strXYZ; }

	_bool Get_IsEvent() const { return m_bIsEvent;  }
	void CallBack_Event(string strEvent) { m_strEvent = strEvent; }
	void Event_Tick(_float fTimeDelta);
	void Zoom_Tick(_float fTimeDelta);
	void Zoom_In(_float fDuration , _float fLinearZoom);
	void Tick_PartObj(_float fTimeDelta);

	PxExtendedVec3 Get_Foot_Position();
 private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };

	shared_ptr<CModel> m_pModelCom = { nullptr };
	shared_ptr<CModel> m_pChai_ModelCom = { nullptr };
	shared_ptr<CModel> m_pSans_ModelCom = { nullptr };

	shared_ptr<CCharacterController> m_pCCT = { nullptr };

private:
	HRESULT Ready_Components();
	void	Create_Attack_Tree();
	void	Delete_Attack_Tree(AttacK_Tree* node);
	void	Initialize_Attack_Tree(AttacK_Tree* node);

	void	Create_Air_Attack_Tree();


	void	PrepareCallbacks();
	void	Prepare_Callbacks(string strName);
	void	Prepare_Callbacks_2(string strName);

	void	Play_Just_Effect(_float fTimeDelta);
	void	Create_Just_Effect(_int iBeat);

public:
	static shared_ptr<CChai> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
	void Free();

private:
	void Animation_End();
	void Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);
	_bool Dash(_float fTimeDelta);
	void Turn(_float fTimeDelta);
	void Jump(_float fTimeDelta);
	void AirMove(_float fTimeDelta);
	_bool Attack(_float fTimeDelta);
	void Attack_Turn(_float fTimeDelta);
	_bool Battle_Engage(_float fTimeDelta);
	void Generator_Mode(_float fTimeDelta);
	void SP_Skill(string strSkillName);
	SkillDesc Skill_Change_Equip(string strEquipSkill);
	string Translate_Skill_Info(SkillDesc skillDesc);

	void Call_Assist(_float fTimedelta);
	void Adjust_Idle_Timing();
	void Adjust_Idle_Timing_Hard();
	_bool Find_R();
	void Traverse_AttacK_Tree(AttacK_Tree* node, ATTACK_TREE_TYPE eAttackType, vector<ATTACK_TREE_TYPE> vecCombo);
	void Rhythm_Parry_Mode(_float fTimeDelta);
	void ZiplineMove(_float fTimeDelta);
	void BuiltIn_Magnet_Move();
	void JumpPad_Move(_float fTimeDelta);
	void PlayEffect(string strEffect, _float4x4 matTrans, _float fmultiplier, _bool isBillBoard);
	/* for Rhythm Parry*/
public:
	void Start_AttackSequence(_float iBeat , _int iAttackReadyStartBeat, const vector<_float>& vecTimer, const vector<_bool>& vecParry);
	_float Get_ReactTimer() const { return m_fReactTimer; }
	void Set_ReactTimer(_float fReactTime)  { m_fReactTimer = fReactTime; }
	void Set_DamagedAnim();
	void Set_ParryAnim();
	void Set_EvadeAnim();
	void Set_DanceBattle_Anim(_int iIndex);


private:
	vector<_float> m_vecBattleTiming;
	vector<_bool> m_vecParry;
	_float m_iAttackReadyStartBeat = 0;
	_bool m_bValid = true;
	_float m_fReactTimer = 0.f;
	_float m_fCoolDown = 0.f;

private:
	shared_ptr<CGameObject> m_pRandSound = nullptr;
	shared_ptr<CGameObject> m_pCommandIcon = nullptr;
	shared_ptr<CGameObject> m_pRPMonster = nullptr;
	_bool m_bRPMode = false;
	_bool m_bCommandIcon = false;
	_bool m_bCCT_On = true;

	_float m_fEffectTimer = 0.f;
	string m_strEffectName = "";
	_int   m_iEffectNumber = 0;
	_float4x4 m_fEffectMatrix = {};

	_int m_iRenderGroup = 0;
	_int m_i808_Effect = 0;

	_bool m_bRimLight = false;
	_float m_fRimTimer = 0.f;
	_float4 m_fRimColor = {};

	_float3 m_fWeaponCollider_Extents = {};

	shared_ptr<CCamera_Player> m_pPlayer_Camera = nullptr;

	string m_strEvent = "";
	_float m_fEvent_Timer = 0.f;
	_bool m_bZoomIn = false;
	_float m_fZoomIn_Timer = 0.f;
	_bool m_bRootMove_On = true;
	_float4 m_fEventPosition = {};

	_bool m_bIsEvent = false;
	_bool m_bIsSans = false;
	_bool m_bRender_Can = false;

	_bool m_b2D_Mode = false;
	_float m_f2D_FixedPos = 0.f;
	string m_strFixedXYZ = "";

	shared_ptr<CGameObject> m_pWire_Target = nullptr;
	shared_ptr<CGameObject> m_pComboWire_Target = nullptr;
	_float m_fWire_Target_Distance = 0.f;
	_float m_fWire_Target_Length = 0.f;
	_bool m_bWire = false;
	_bool m_bGimmicWire = false;
	_bool m_bInteraction = false;
	_bool m_bLift_Macaron = false;
	_bool m_bODS = false;

	shared_ptr<CEffect_Union> m_SpotLight = {};
	shared_ptr<CEffect_Union> m_ZipLineBlur = {};
	CHAI_EVENT_STATE m_eEvent_State = CHAI_EVENT_STATE::EVENT_STATE_END;
private:
	_float m_fMaxHp = 150.f;
	_float m_fMaxReverb = 150.f;
	_float m_fHp = 150.f;
	_float m_fReverb = 150.f;

	vector<string> m_vecSkill = {};
private:
	vector<string> m_vecAnimList;

	CHAI_MOVE_STATE m_eMoveState = CHAI_MOVE_STATE::MOVE_END;
	vector <shared_ptr< CPartObject>> m_pWeapon ;

	_float m_fLookDiffDegree = 0.f;
	_float m_fTurnTimer = 0.1f;
	_bool m_bTrunRight = false;
	_bool m_bTurning = false;
	_bool m_bAttacKTurning = false;

	_float m_fAttack_Motion_Offset = 0.f;
	_float m_fIdle_Motion_Offset = 0.f;

	_float m_fAttackTimer = 0.0f;
	_bool m_bAttack_Input = false;
	AttacK_Tree* m_pTree_Attack = nullptr;
	AttacK_Tree* m_pTree_Air_Attack = nullptr;
	vector<ATTACK_TREE_TYPE> m_vecCombo = {};
	string m_strCombo = {};
	ATTACK_TREE_TYPE m_eNextAttack = ATTACK_TREE_TYPE::ATTACK_TYPE_END;
	_bool m_bNextAttack = false;

	_int m_iDashCount = 0;
	_float3 m_fDashDir = {0,0,0};
	_float m_fTime_Offset = 1.f;
	_float m_fBefore_Time_Multiflier = 1.f;

	_bool m_bUpperAttack = false;
	_bool m_bL_Rest_UpperAttack = false;
	_bool m_bFirstTick = false;
private:
	shared_ptr<CCollider> m_pCollider_Hit = { nullptr };
	shared_ptr<CCollider> m_pCollider_Attack = { nullptr };
	_bool m_isAttackOn = false;
	const _float4x4* m_pMat_RightHand = { nullptr };
	const _float4x4* m_pMat_Spine = { nullptr };


private: // Jump
	_bool m_bOnAir = false;
		
private:	// For Effect
	_float4x4 m_matCurrentPos = {};

	shared_ptr<CWire> m_pWire = nullptr;
};

END