#include "stdafx.h"
#include "..\Public\HatKid.h"
#include "GameInstance.h"
#include "PlayerController.h"


CHatKid::CHatKid(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CPlayer(pDevice, pContext)
{
}

CHatKid::CHatKid(const CHatKid & rhs)
	:CPlayer(rhs)
{
}

HRESULT CHatKid::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHatKid::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	m_pController = new CPlayerController();
	m_pController->Initialize(this);

	//m_pModelCom->Set_CurrentAnimIndex(1);
	m_pModelCom->Set_CurrentAnimIndex_Upper(ITEM_PICKUP_LARGE);
	m_pModelCom->Set_CurrentAnimIndex_Lower(IDLE);

	m_eDirState = DIR_FORWARD;

	return S_OK;
}

void CHatKid::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pController->Input_Controller(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CHatKid::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CHatKid::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}
#ifdef _DEBUG
	/*m_pAABBCom->Render();
	m_pOBBCom->Render();*/
	m_pSPHERECom->Render();

#endif

	return S_OK;
}

HRESULT CHatKid::Move_Front(_float fTimeDelta)
{
	//m_pModelCom->Set_CurrentAnimIndex(RUN);
	m_pModelCom->Set_CurrentAnimIndex_Upper(ITEM_PICKUP_LARGE);
	m_pModelCom->Set_CurrentAnimIndex_Lower(RUN);
	
	m_pTransformCom->Go_Straight(fTimeDelta);

	return S_OK;
}

HRESULT CHatKid::Move_Back(_float fTimeDelta)
{
	//m_pModelCom->Set_CurrentAnimIndex(RUN);
	m_pModelCom->Set_CurrentAnimIndex_Upper(SHAKE_FLASK);
	m_pModelCom->Set_CurrentAnimIndex_Lower(RUN);
	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -1.5f);
	m_pTransformCom->Go_Straight(-fTimeDelta);
	return S_OK;
}

HRESULT CHatKid::Move_Left(_float fTimeDelta)
{
	//m_pModelCom->Set_CurrentAnimIndex(RUN);
	m_pModelCom->Set_CurrentAnimIndex_Upper(CRARRY_UMBRELLA_INTRO);
	m_pModelCom->Set_CurrentAnimIndex_Lower(RUN);
	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -1.f);
	//m_pTransformCom->Go_Straight(fTimeDelta);
	return S_OK;
}

HRESULT CHatKid::Move_Right(_float fTimeDelta)
{
	//m_pModelCom->Set_CurrentAnimIndex(RUN);
	m_pModelCom->Set_CurrentAnimIndex_Upper(ITEM_CARRY_LARGE);
	m_pModelCom->Set_CurrentAnimIndex_Lower(RUN);
	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 1);
	//m_pTransformCom->Go_Straight(fTimeDelta);
	return S_OK;
}

HRESULT CHatKid::Jump(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CHatKid::Action_1(_float fTimeDelta)
{// Sliding
	return S_OK;
}

HRESULT CHatKid::Action_2(_float fTimeDelta)
{	//Skill
	return S_OK;
}

HRESULT CHatKid::Action_3(_float fTimeDelta)
{	// Inventory && Menu
	return S_OK;
}

HRESULT CHatKid::Action_4(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CHatKid::Idle(_float fTimeDelta)
{
	//m_pModelCom->Set_CurrentAnimIndex(0);
	m_pModelCom->Set_CurrentAnimIndex_Upper(ITEM_CARRY_LARGE);
	m_pModelCom->Set_CurrentAnimIndex_Lower(IDLE);
	return S_OK;
}

void CHatKid::TestFunc(_float fTimeDelta)
{
}

HRESULT CHatKid::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	XMStoreFloat3(&m_vOriginal_Dir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	CModel::BONEINDEXDESC BoneIndexDesc;
	ZeroMemory(&BoneIndexDesc, sizeof(CModel::BONEINDEXDESC));
	/* For.Com_Model*/
	BoneIndexDesc.iUpperIndex = 0;
	BoneIndexDesc.iLowerIndex = 91;
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HatKid"), (CComponent**)&m_pModelCom, &BoneIndexDesc)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CHatKid::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CHatKid * CHatKid::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHatKid*	pInstance = new CHatKid(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHatKid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHatKid::Clone(void * pArg)
{
	CHatKid*	pInstance = new CHatKid(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHatKid"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHatKid::Free()
{
	__super::Free();
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pController);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
