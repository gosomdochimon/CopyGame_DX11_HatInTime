#include "stdafx.h"
#include "..\Public\SubConEye.h"
#include "GameInstance.h"
#include "Apple.h"


CSubConEye::CSubConEye(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CActor(pDevice, pContext)
{
}

CSubConEye::CSubConEye(const CSubConEye & rhs)
	: CActor(rhs)
{
}

HRESULT CSubConEye::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSubConEye::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-25.f, -1.f, 60.f, 1.f));
	
	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;
	//NaviDesc.fCurrent_Position = { -10.f, 0.f, 61.f };
	XMStoreFloat3(&NaviDesc.fCurrent_Position, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	//_vector vPos = XMLoadFloat3(&NaviDesc.fCurrent_Position);

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	m_eBehaviorState = BEHAVIOR_STATE::IDLE;

	return S_OK;
}

void CSubConEye::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Play_FSM(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, &m_bAnimFinished);

	m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CSubConEye::Late_Tick(_float fTimeDelta)
{
	/*CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("InteractionObject"), TEXT("Com_SPHERE"));
	if (nullptr == pTargetCollider)
		return;*/

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		//m_pRendererCom->Add_Debug(m_pSPHERECom);
#endif

	}

	//RELEASE_INSTANCE(CGameInstance);
}

HRESULT CSubConEye::Render()
{
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
#endif

	return S_OK;
}

_bool CSubConEye::Idle(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::ANIM_IDLE)
	{
		m_eCurrentAnimState = ANIM_STATE::ANIM_IDLE;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
	}

	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//CApple* pTarget = dynamic_cast<CApple*>(pGameInstance->Get_FirstObject(LEVEL_GAMEPLAY, TEXT("InteractionObject")));
	//pTarget->Set_Burned(true);

	//RELEASE_INSTANCE(CGameInstance);

	if (m_iIdleCount >= 2)
	{
		Set_DeleteObject();
		//m_iIdleCount = 0;
		
	}

	return m_bAnimFinished;
}

void CSubConEye::Play_FSM(_float fTimeDelta)
{

	switch (m_eBehaviorState)
	{
	case BEHAVIOR_STATE::IDLE:
		Idle(fTimeDelta);
		break;

	}

}


void CSubConEye::Update_Collider(void)
{
}



HRESULT CSubConEye::Ready_Components()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iLevelIndex = pGameInstance->Get_CurrentLevelIndex();

	RELEASE_INSTANCE(CGameInstance);

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

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SubConEye"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSubConEye::SetUp_ShaderResources()
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

CSubConEye * CSubConEye::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSubConEye*	pInstance = new CSubConEye(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSubConEye"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CSubConEye::Clone(void * pArg)
{
	CSubConEye*	pInstance = new CSubConEye(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSubConEye"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSubConEye::Free()
{
	__super::Free();
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}

