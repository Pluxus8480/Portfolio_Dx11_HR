#include "GimmickBase.h"
#include "GimmickManager.h"

CGimmickBase::CGimmickBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CGimmickBase::CGimmickBase(const CGimmickBase& rhs)
	: CGameObject{rhs}
{
}

CGimmickBase::~CGimmickBase()
{
	Free();
}

void CGimmickBase::Save_Data(ofstream& os)
{
	write_typed_data(os, m_strGimmickID.size());
	os.write(&m_strGimmickID[0], m_strGimmickID.size());
}

void CGimmickBase::Load_Data(ifstream& is)
{
	//read_typed_data(is, m_iGimmickID);

	size_t iLength = {};
	read_typed_data(is, iLength);
	
	m_strGimmickID.resize(iLength);
	is.read(&m_strGimmickID[0], iLength);
}

void CGimmickBase::Link_Relatives()
{
	for (string strIDs : m_vecGimmickIDs)
	{
		shared_ptr<CGimmickBase> pObject = CGimmickManager::Get_Instance()->Get_GimmckObj(strIDs);

		if (pObject != nullptr)
		{
			m_vecRelatives.emplace_back(pObject);
			pObject->Set_Other(shared_from_this());
		}
	}
}

void CGimmickBase::Set_Other(shared_ptr<CGameObject> pOther)
{
	m_pOther = pOther;
}

void CGimmickBase::Update_Relatives()
{
	for (_int i = 0; i < m_vecRelatives.size();)
	{
		if (m_vecRelatives[i]->Is_Dead())
			m_vecRelatives.erase(m_vecRelatives.begin() + i);
		else
			++i;
	}
}

void CGimmickBase::Save_Relatives(ofstream& os)
{
	write_typed_data(os, m_vecRelatives.size());

	for (auto pRelative : m_vecRelatives)
	{
		string strID = pRelative->Get_GimmickID();

		write_typed_data(os, strID.size());
		os.write(&strID[0], strID.size());
	}
}

void CGimmickBase::Load_Relatives(ifstream& is)
{
	size_t iNumRelative = {};
	read_typed_data(is, iNumRelative);

	for (_int i = 0; i < iNumRelative; ++i)
	{
		size_t stringSize;
		read_typed_data(is, stringSize);
		string strID;
		strID.resize(stringSize);
		is.read(&strID[0], stringSize);

		m_vecGimmickIDs.emplace_back(strID);
	}
}

void CGimmickBase::Free()
{
	m_vecRelatives.clear();
	m_vecGimmickIDs.clear();
}
