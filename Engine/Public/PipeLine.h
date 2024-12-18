#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CPipeLine 
{
public:
    enum PIPELINE_STATE
    {
        TS_VIEW,
        TS_PROJ,
        TS_VIEW_INV,
        TS_PROJ_INV,
        TS_END,
    };

private:
    CPipeLine();
public:
    virtual ~CPipeLine();
public:
    void    Update();
    void    Set_ShadowDir(_fvector vDir);
    void    Set_ShadowDir(const _float4& vDir);

    void    Set_Transform(PIPELINE_STATE eState, _fmatrix Matrix);
    void    Set_Transform(PIPELINE_STATE eState, const _float4x4& Matrix);

    _fmatrix  Get_LightView_Matrix() const;
    const _float4x4& Get_LightView_Float4x4() const;
    _fmatrix  Get_LightProj_Matrix() const;
    const _float4x4& Get_LightProj_Float4x4() const;

    _fmatrix  Get_Transform_Matrix(PIPELINE_STATE eState) const;
    const _float4x4& Get_Transform_Float4x4(PIPELINE_STATE eState) const;

    const _float4& Get_CamPosition_Float4() const;
    _fvector Get_CamPosition_Vector() const;

    const _float4& Get_CamLook_Float4() const;
    _fvector Get_CamLook_Vector() const;



    void    Add_Camera(const _wstring& strTag, shared_ptr<class CCamera> pCamera);
    shared_ptr<class CCamera> Get_Camera(const _wstring& strTag);
    shared_ptr<class CCamera> Get_MainCamera();

    void    Set_MainCamera(const _wstring& strTag);
    void    Remove_Camera(const _wstring& strTag);
    void    Clear_Camera();

private:
    void Calc_Shadow_Matrix();

private:
    _float4x4 m_TransformationMatrices[TS_END] = {};
    _float4x4 m_LightView{};
    _float4x4 m_LightProjection{};
    _float4   m_vCamPosition = {};
    _float4   m_vCamLook = {};
    _float4   m_vShadowDir{};

    shared_ptr<class CCamera> m_pMainCamera = nullptr;
    _wstring m_strMainCamTag;
    map<_wstring, shared_ptr<class CCamera>> m_Cameras;

public:
    static shared_ptr<CPipeLine> Create();
    void Free();
};

END