#include "Grass.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "GrassMatrix.h"
#include "Collider.h"
#include "Bounding_Sphere.h"

#include "MapManager.h"

wstring CGrass::ObjID = TEXT("CGrass");
CGrass::CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CGrass::CGrass(const CGrass& rhs)
    : CGameObject{rhs}
{
}

CGrass::~CGrass()
{
    Free();
}

HRESULT CGrass::Initialize_Prototype()
{
    _float4x4 fMat = {};
    //XMStoreFloat4x4(&fMat, XMMatrixRotationZ(XMConvertToRadians(90.f)));
    XMStoreFloat4x4(&fMat, XMMatrixScaling(0.01f, 0.01f, 0.01f));

    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Grass01"), TEXT("../Bin/Resources/Models/Map/General/SM_Glass_WB_01.FBX"), MODEL_TYPE::NONANIM, fMat)
    );
    m_List_ResourceInfo.push_back(
        make_shared<RES_MODEL>(TEXT("Prototype_Component_Model_Grass02"), TEXT("../Bin/Resources/Models/Map/General/SM_Glass_WB_02.FBX"), MODEL_TYPE::NONANIM, fMat)
    );

    return S_OK;
}

HRESULT CGrass::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MISCOBJ_DESC* pDesc = static_cast<MISCOBJ_DESC*>(pArg);
    m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);

    if (FAILED(Ready_Components()))
        return E_FAIL;
    
    return S_OK;
}

void CGrass::PriorityTick(_float fTimeDelta)
{
}

void CGrass::Tick(_float fTimeDelta)
{
}

void CGrass::LateTick(_float fTimeDelta)
{
    auto pGI = m_pGameInstance.lock();

    if (!pGI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_Position(), m_fCullingRange)) return;

    pGI->Add_RenderObjects(CRenderer::RENDER_NONBLEND_NON_LIT, shared_from_this());

    for (size_t i = 0; i < 2; i++)
    {
        for (auto& GrassMatrix : m_vecMatrixObj[i])
            GrassMatrix->LateTick(fTimeDelta);
    }
}

HRESULT CGrass::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    for (size_t i = 0; i < 2; i++)
    {
        if (FAILED(m_pModelCom[i]->Bind_Material(m_pShaderCom, "g_Texture", (_uint)0, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pModelCom[i]->Bind_Buffers((_uint)0);

        for (auto& pGrassMatrix : m_vecMatrixObj[i])
        {
            pGrassMatrix->Bind_Matrix();

            m_pShaderCom->Begin((_uint)MESH_PASS::Grass);
            m_pModelCom[i]->Draw((_uint)0);
        }
    }

#ifdef _DEBUG
    _matrix ColliderOffset = XMMatrixScaling(m_fCullingRange, m_fCullingRange, m_fCullingRange);
    m_pColliderCom->Update(ColliderOffset * m_pTransformCom->Get_WorldMatrix_XMMat());
    if (m_pColliderCom)
        m_pColliderCom->Render();
#endif

    return S_OK;
}

void CGrass::Edit_Attribute()
{
    ImGui::InputFloat("Distance", &m_fDistance);
    ImGui::InputFloat("CullingRange", &m_fCullingRange);
    ImGui::SliderInt("Model Index", &m_iModelIndex, 0, 1);

    if (ImGui::Button("Clear Grass"))
    {
        for (size_t i = 0; i < 2; i++)
            m_vecMatrixObj[i].clear();
    }

    auto pGI = m_pGameInstance.lock();

    auto pObj = GAMEINSTANCE->Get_Object_Prototype(CGrassMatrix::ObjID);
    assert(pObj);

    if (pGI->Get_MousePressing(MOUSEKEYSTATE::DIMK_LBUTTON))
    {
        auto& io = ImGui::GetIO();
        if (io.WantCaptureMouse)
            return;

        auto MousePos = io.MousePos;

        _uint iPickedObj = pGI->Pick_Object(MousePos.x, MousePos.y)->Get_ObjectID();
        _float4 vPickedPos = pGI->Pick_Position(MousePos.x, MousePos.y);

        for (size_t i = 0; i < 2; i++)
        {
            for (auto& GrassMatrix : m_vecMatrixObj[i])
            {
                if (GrassMatrix->Get_ObjectID() == iPickedObj) return;

                _vector vPrevPos = GrassMatrix->Get_Transform()->Get_Position();
                if (XMVectorGetX(XMVector3Length(vPrevPos - XMLoadFloat4(&vPickedPos))) < m_fDistance)
                    return;
            }
        }

        CGrassMatrix::GRASS_DESC Desc{};
        Desc.pParentTransform = m_pTransformCom;
        Desc.iModelIndex = m_iModelIndex;
        XMStoreFloat4x4(&Desc.worldMatrix, XMMatrixTranslation(vPickedPos.x, vPickedPos.y, vPickedPos.z));
        
        m_vecMatrixObj[m_iModelIndex].push_back(static_pointer_cast<CGrassMatrix>(pObj->Clone(&Desc)));
    }
}

void CGrass::Save_Data(ofstream& os)
{
    write_typed_data(os, m_fCullingRange);

    for (size_t i = 0; i < 2; i++)
    {
        write_typed_data(os, m_vecMatrixObj[i].size());
        for (auto& GrassMatrix : m_vecMatrixObj[i])
        {
            _float4x4 worldMat = GrassMatrix->Get_Transform()->Get_WorldMatrix();
            write_typed_data(os, worldMat);
        }
    }
}

void CGrass::Load_Data(ifstream& is)
{
    _uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();

    read_typed_data(is, m_fCullingRange);

    for (_uint i = 0; i < 2; i++)
    {
        size_t Size = {};
        read_typed_data(is, Size);
        m_vecMatrixObj[i].resize(Size, nullptr);
        for (size_t j = 0; j < Size; j++)
        {
            _float4x4 worldMat = { Identity };
            read_typed_data(is, worldMat);

            CGrassMatrix::GRASS_DESC Desc{};
            Desc.pParentTransform = m_pTransformCom;
            Desc.iModelIndex = i;
            Desc.worldMatrix = worldMat;

            auto pObj = GAMEINSTANCE->Get_Object_Prototype(CGrassMatrix::ObjID);
            assert(pObj);

            m_vecMatrixObj[i][j] = static_pointer_cast<CGrassMatrix>(pObj->Clone(&Desc, iLevelIndex));
        }
    }
}

HRESULT CGrass::Ready_Components()
{
    _uint iLevelIndex = CMapManager::Get_Instance()->Get_NextLevel();

    m_pShaderCom = static_pointer_cast<CShader>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader_Com")));
    assert(m_pShaderCom);

    m_pModelCom[0] = static_pointer_cast<CModel>(Add_Component(iLevelIndex, TEXT("Prototype_Component_Model_Grass01"), TEXT("Model01_Com")));
    m_pModelCom[1] = static_pointer_cast<CModel>(Add_Component(iLevelIndex, TEXT("Prototype_Component_Model_Grass02"), TEXT("Model02_Com")));

    assert(m_pModelCom[0]);
    assert(m_pModelCom[1]);

#ifdef _DEBUG
    CBounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc{};
    sphereDesc.vCenter = { 0.f, 0.f, 0.f };
    sphereDesc.fRadius = 1.f;

    m_pColliderCom = static_pointer_cast<CCollider>(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Collider_Com"), &sphereDesc));
#endif

    return S_OK;
}

HRESULT CGrass::Bind_ShaderResources()
{
    auto pGI = m_pGameInstance.lock();

    _float4x4 ViewMat;
    XMStoreFloat4x4(&ViewMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _float4x4 ProjMat;
    XMStoreFloat4x4(&ProjMat, pGI->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CGrass> CGrass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    struct MakeSharedEnabler :public CGrass
    {
        MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGrass(pDevice, pContext)
        {}
    };


    shared_ptr<CGrass> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGrass"));
        assert(false);
    }

    return pInstance;
}

shared_ptr<CGameObject> CGrass::Clone(void* pArg)
{
    struct MakeSharedEnabler :public CGrass
    {
        MakeSharedEnabler(const CGrass& rhs) : CGrass(rhs)
        {}
    };

    shared_ptr<CGrass> pInstance = make_shared<MakeSharedEnabler>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGrass"));
        assert(false);
    }

    return pInstance;
}

void CGrass::Free()
{
}
