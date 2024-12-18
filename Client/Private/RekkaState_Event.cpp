#include "RekkaState_Event.h"
#include "Monster_Rekka.h"
#include "GameInstance.h"
#include "Transform.h"

#include "Player_Camera.h"
#include "PlayerManager.h"

#include "RekkaState_Idle.h"


CRekkaState_Event::CRekkaState_Event(weak_ptr<class CMonster_Rekka> pRekka, State preState)
{
    m_state = State::GIMMICK;
    m_pOwner = pRekka; 
}

CRekkaState* CRekkaState_Event::StepState(_float fTimeDelta)
{
    if (m_pOwner.lock()->IsAnimFinished())
    {
        auto pMainCam = dynamic_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main")));
        pMainCam->End_Following_Bone();

        m_pOwner.lock()->CCT_ON();
        m_pOwner.lock()->GravityOn();
        return new CRekkaState_Idle(m_pOwner, m_state);
    }

    return nullptr;
}

void CRekkaState_Event::ActiveState(_float fTimeDelta)
{
}

void CRekkaState_Event::EnterState()
{
    m_pOwner.lock()->SetAnimHard("em2000_event-rekka_100");
    m_pOwner.lock()->GravityOff();

    auto pMainCam = dynamic_pointer_cast<CCamera_Player>(GAMEINSTANCE->Get_Camera(TEXT("Camera_Player_Main")));
    pMainCam->Start_Following_Bone(m_pOwner.lock()->m_pTransformCom->Get_WorldMatrix_Ptr(), const_cast <_float4x4*>(m_pOwner.lock()->m_pMat_Camera));
}

void CRekkaState_Event::ExitState()
{
}
