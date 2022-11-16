#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPawn(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CPawn(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK; 
}

HRESULT CMonster::Initialize(void * pArg)
{
	//if (FAILED(Ready_Components()))
	//	return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(rand() % 10, 0.f, rand() % 10, 1.f));
	//m_pModelCom->Set_CurrentAnimIndex(rand() % 10);
	//m_pModelCom->Set_NextAnimIndex(rand() % 10, true);
	////m_pModelCom->Set_CurrentAnimIndex(3);

	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{

	//m_pModelCom->Play_Animation(fTimeDelta, &m_bAnimFinished);

	//m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
	//if (nullptr == pTargetCollider)
	//	return;

	//m_pSPHERECom->Collision(pTargetCollider);

	//if (nullptr != m_pRendererCom)
	//	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	//RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMonster::Render()
{
//	if (nullptr == m_pShaderCom ||
//		nullptr == m_pModelCom)
//		return E_FAIL;
//
//	if (FAILED(SetUp_ShaderResources()))
//		return E_FAIL;
//
//	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();
//
//	for (_uint i = 0; i < iNumMeshes; ++i)
//	{
//		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
//			return E_FAIL;
//
//		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
//			return E_FAIL;
//	}
//
//#ifdef _DEBUG
//	/*m_pAABBCom->Render();
//	m_pOBBCom->Render();*/
//	m_pSPHERECom->Render();
//
//#endif
//
	return S_OK;
}

_bool CMonster::Idle(_float fTimeDelta)
{
	return _bool();
}

_bool CMonster::Move(_float fTimeDelta)
{
	return _bool();
}

_bool CMonster::Death(_float fTimeDelta)
{
	return _bool();
}

_bool CMonster::Attack(_float fTimeDelta)
{
	return _bool();
}

_bool CMonster::Jump(_float fTimeDelta)
{
	return _bool();
}

void CMonster::Update_Collider(void)
{
}



HRESULT CMonster::Ready_Components()
{
	///* For.Com_Renderer */
	//if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
	//	return E_FAIL;

	///* For.Com_Transform */
	//CTransform::TRANSFORMDESC		TransformDesc;
	//ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	//TransformDesc.fSpeedPerSec = 5.f;
	//TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	//if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
	//	return E_FAIL;

	///* For.Com_Shader */
	//if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* For.Com_Model*/
	//if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;

	//CCollider::COLLIDERDESC		ColliderDesc;
	//ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	///* For.Com_SPHERE */
	//ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	//ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMonster::SetUp_ShaderResources()
{
	//if (nullptr == m_pShaderCom)
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
	//	return E_FAIL;

	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	//	return E_FAIL;

	//RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMonster * CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster*	pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonster::Clone(void * pArg)
{
	CMonster*	pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	//Safe_Release(m_pSPHERECom);
	//Safe_Release(m_pTransformCom);
	//Safe_Release(m_pShaderCom);
	//Safe_Release(m_pModelCom);
	//Safe_Release(m_pRendererCom);
}
