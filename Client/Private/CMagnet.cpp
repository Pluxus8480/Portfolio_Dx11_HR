#include "CMagnet.h"
#include "CUi_MapInterAction.h"
#include "PlayerManager.h"
#include "GameManager.h"
#include "MapManager.h"


wstring CMagnet::ObjID = TEXT("CMagnet");

CMagnet::CMagnet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGimmickBase(pDevice, pContext)
{
}

CMagnet::CMagnet(const CMagnet& rhs)
	:CGimmickBase(rhs)
{
}

CMagnet::~CMagnet()
{
}

void CMagnet::Set_MagnetUiActive(_bool Active)
{
	if (m_pUi)
	{
		if (!m_bUseMagnet)
		{
			m_pUi->Phase_Active(Active);
		}
	}
}

_float CMagnet::Magnet_DistancePlayer()
{
	return PLAYERMANAGER->Get_DistancePlayer(m_pTransformCom);
}

void CMagnet::Player_UseMagnet()
{
	m_bUseMagnet = true;
	if (m_pUi)
		m_pUi->Phase_Active(false);
}

void CMagnet::Initialize_Ui()
{
	_uint iLevelIndex =  CMapManager::Get_Instance()->Get_NextLevel();

	CUi_MapInterAction::Desc Desc;
	Desc.OwnerTransform = m_pTransformCom;
	Desc.RelativeDistance = { 0, 0 };
	Desc.iWorkTextureIndex = { CUi_MapInterAction::WorkTexture::Wire };
	Desc.fWorkDistance = 50;
	Desc.fAwareDistance = 50;
	Desc.Size = { 30, 30 };
	m_pUi = static_pointer_cast<CUi_MapInterAction>(CUi_Mgr::Get_Instance()->Add_Clone_Return(CUi_MapInterAction::ObjID, iLevelIndex,
		L"CUi", CUi_MapInterAction::ObjID, &Desc));
}
