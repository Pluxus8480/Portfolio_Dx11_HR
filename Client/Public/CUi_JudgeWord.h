#pragma once

#include "Client_Defines.h"
#include "CUi_Default.h"

BEGIN(Client)

class CUi_JudgeWord : public CUi_Default
{
public:
	static wstring ObjID;
	enum class WordType { Perfect, Good, Miss, End };

public:
	using Desc = struct CUi_JudgeWordInfo : public CUi_2DInfo
	{
		WordType eWordType = { WordType::End };
	};

protected:
	CUi_JudgeWord(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi_JudgeWord(const CUi_JudgeWord& rhs);


public:
	virtual ~CUi_JudgeWord();


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	

private:
	_float m_fTime = { 0 };
	_float2 m_fOriginScale = { 200 , 80 };
	_uint m_iWordDir = { 0 };


public:
	static shared_ptr<CUi_JudgeWord> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;
};

END
