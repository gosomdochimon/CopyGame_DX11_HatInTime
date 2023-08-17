#include "stdafx.h"
#include "..\Public\Clerk.h"
#include "GameInstance.h"
#include "Apple.h"
#include "Deligate_Monster.h"

CClerk::CClerk(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPawn(pDevice, pContext)
{
}

CClerk::CClerk(const CClerk & rhs)
	: CPawn(rhs)
{
}

HRESULT CClerk::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CClerk::Initialize(void * pArg)
{
	if (pArg != nullptr)
	{
		memcpy(&m_CloneInfo, (CLONEINFO*)pArg, sizeof(CLONEINFO));
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
	//std::function<void(CMonster&)> func = &CMonster::Release_Super;
	pDeligate->Add_NPC(this);
	RELEASE_INSTANCE(CDeligate_Monster);

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-20.f, -1.f, 70.f, 1.f));
	//m_eCurrentAnimState = ANIM_STATE::ANIM_IDLE;
	//m_pModelCom->Set_CurrentAnimIndex(m_eCurrentAnimState);
	//NaviDesc.fCurrent_Position = { -10.f, 0.f, 61.f };
	m_eBehaviorState = BEHAVIOR_STATE::BULL;
	m_eCurrentAnimState = ANIM_BULL;
	Set_Height();
	return S_OK;
}

void CClerk::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Play_FSM(fTimeDelta);
	Set_Height();
	m_pModelCom->Play_Animation(fTimeDelta, &m_bAnimFinished);

	m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CClerk::Late_Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/*CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("InteractionObject"), TEXT("Com_SPHERE"));
	if (nullptr == pTargetCollider)
		return;*/

	//if (!m_bCollider &&m_pSPHERECom->Collision(pTargetCollider))
	//	m_eBehaviorState = BEHAVIOR_STATE::ABSORBED;

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		//m_pRendererCom->Add_Debug(m_pSPHERECom);
#endif

	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CClerk::Render()
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

_bool CClerk::Idle(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::ANIM_IDLE)
	{
		m_eCurrentAnimState = ANIM_STATE::ANIM_IDLE;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("CoffeeMan01-Head");
		_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ /*m_pStuckSocket->Get_OffsetMatrix() **/
			m_pStuckSocket->Get_CombinedTransformationMatrix() *
			XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
		_float4x4 m_CombinedWorldMatrix;
		XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

		CEffect::EFFECTINFO EffectInfo;
		EffectInfo.iTypeNum = 2;
		EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
		EffectInfo.iMoveType = 0;
		EffectInfo.fAngle = 0.f;
		EffectInfo.fLifeTime = 2.f;


		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Talk"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_Effect"), &EffectInfo)))
			return m_bAnimFinished;

		Safe_Release(pGameInstance);

	}

	//if (m_bAnimFinished == true)
	//{
	//	m_bAnimFinished = false;
	//	//m_iIdleCount++;
	//}

	//if (m_iIdleCount >= 1)
	//{
	//	m_iIdleCount = 0;
	//	//m_eBehaviorState = BEHAVIOR_STATE::SPIN;
	//}

	return m_bAnimFinished;
}

_bool CClerk::Kimochi(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::ANIM_KIMOCHI)
	{
		m_eCurrentAnimState = ANIM_STATE::ANIM_KIMOCHI;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_bCollider = true;
	}
	
	if (m_bAnimFinished)
	{
		m_bHappy = true;
		m_eBehaviorState = BEHAVIOR_STATE::IDLE;
	}

	return m_bAnimFinished;
}

_bool CClerk::Bulled(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::ANIM_BULL)
	{
		m_eCurrentAnimState = ANIM_STATE::ANIM_BULL;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

	}

	return m_bAnimFinished;
}

_bool CClerk::Talking(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::ANIM_TALK)
	{
		m_eCurrentAnimState = ANIM_STATE::ANIM_TALK;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_iIdleCount = 0;
	}

	//if (m_iIdleCount >= 1)
	//{
	//	m_iIdleCount = 0;
	//	//m_eBehaviorState = BEHAVIOR_STATE::SPIN;
	//}

	return m_bAnimFinished;
}

void CClerk::Play_FSM(_float fTimeDelta)
{

	switch (m_eBehaviorState)
	{
	case BEHAVIOR_STATE::IDLE:
		Idle(fTimeDelta);
		break;
	case BEHAVIOR_STATE::BULL:
		Bulled(fTimeDelta);
		break;
	case BEHAVIOR_STATE::KIMOCHI:
		Kimochi(fTimeDelta);
		break;
	case BEHAVIOR_STATE::TALK:
		Talking(fTimeDelta);
		break;

	}

}


void CClerk::Update_Collider(void)
{


}

void CClerk::Functor()
{
	m_eBehaviorState = BEHAVIOR_STATE::KIMOCHI;
}

void CClerk::Set_Height()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	fPos.y = m_pNavigationCom->Get_NavigationHeight(fPos);
	vPos = XMVectorSet(fPos.x, fPos.y+ 1.f, fPos.z, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	//printf_s("%f", fPos.y);
	m_pNavigationCom->isMove(vPos);
}



HRESULT CClerk::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_Clerk"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 2.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(m_CloneInfo.WorldMat);

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;
	XMStoreFloat3(&NaviDesc.fCurrent_Position, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Navigation_Platform"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CClerk::SetUp_ShaderResources()
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

CClerk * CClerk::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CClerk*	pInstance = new CClerk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CClerk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CClerk::Clone(void * pArg)
{
	CClerk*	pInstance = new CClerk(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CClerk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CClerk::Free()
{
	__super::Free();
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
