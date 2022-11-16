#include "stdafx.h"
#include "..\Public\MadCrow.h"
#include "GameInstance.h"
#include "HatKid.h"
#include "Detect.h"

CMadCrow::CMadCrow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CMadCrow::CMadCrow(const CMadCrow & rhs)
	: CMonster(rhs)
{
}

HRESULT CMadCrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMadCrow::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(rand() % 10 -15, 0.f, rand() % 10 - 15, 1.f));
	m_pModelCom->Set_CurrentAnimIndex(rand() % (_uint)ANIM_END);
	m_pModelCom->Set_NextAnimIndex(rand() % (_uint)ANIM_END, true);
	//m_pModelCom->Set_CurrentAnimIndex(3);

	return S_OK;
}

void CMadCrow::Tick(_float fTimeDelta)
{
	if (m_bDelete)
		return;


	Play_FSM(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, &m_bAnimFinished);

	Update_Collider();
}

void CMadCrow::Late_Tick(_float fTimeDelta)
{
	if (m_bDelete)
		return;

	if (m_bAnimFinished == true)
	{
		m_bDelete = true;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
	if (nullptr == pTargetCollider)
		return;

	if (m_pSPHERECom->Collision(pTargetCollider))
	{
		m_pModelCom->Set_NextAnimIndex(ANIM_DEATH, false);
		m_eAnimState = ANIM_DEATH;
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMadCrow::Render()
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
	m_pSPHERECom->Render();
	m_pDetect->Render();
#endif

	return S_OK;
}

void CMadCrow::Update_Collider(void)
{
	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ m_pStuckSocket->Get_OffsetMatrix() *
		m_pStuckSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);
	m_pSPHERECom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
	//For.DetectUpdate
	m_pDetect->Update_Position(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CMadCrow::Play_FSM(_float fTimeDelta)
{
	if (m_eAnimState == ANIM_STATE::ANIM_DEATH)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr == pGameInstance)
		return;

	CHatKid* pTarget = dynamic_cast<CHatKid*>(pGameInstance->Get_FirstObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));

	CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
	if (nullptr == pTargetCollider)
		return;


	
	if (!m_bFightMode)
	{
		if (m_eAnimState != ANIM_STATE::ANIM_IDLE)
		{
			m_eAnimState = ANIM_STATE::ANIM_IDLE;
			m_pModelCom->Set_NextAnimIndex(ANIM_IDLE);
		}
		
		if (m_pDetect->Detect_MinRange(pTargetCollider))
		{
			m_bFightMode = true;
		}
	}
	else
	{	//Fight_ModeÀÏ ¶§
		if (m_eAnimState != ANIM_STATE::ANIM_WALK)
		{
			m_eAnimState = ANIM_STATE::ANIM_WALK;
			m_pModelCom->Set_NextAnimIndex(ANIM_WALK);
		}

		_vector vLook = pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		/*if (XMVectorGetX(XMVector3Length(vLook)) < 8.f){ ]*/
		m_pTransformCom->LookAt(pTarget->Get_State(CTransform::STATE_TRANSLATION));

		_float fDistance = 0.f;
		XMStoreFloat(&fDistance, (XMVector3Length(pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION))));

		if (fDistance >= 1.f)
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);
		}

		if (!(m_pDetect->Detect_MaxRange(pTargetCollider)))
		{
			m_bFightMode = false;
		}

	}
	

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMadCrow::Ready_Others(_float3 fColliderPos)
{
	CDetect::DETECTDESC DetectDesc;
	CDetect::COLLIDERDESC Col_MaxDesc, Col_MinDesc;

	ZeroMemory(&DetectDesc, sizeof(CDetect::DETECTDESC));
	ZeroMemory(&Col_MaxDesc, sizeof(CDetect::COLLIDERDESC));
	ZeroMemory(&Col_MinDesc, sizeof(CDetect::COLLIDERDESC));
	Col_MaxDesc.vPosition = fColliderPos;
	Col_MaxDesc.vRotation = _float3(0.f, 0.f, 0.f);
	Col_MaxDesc.vScale = _float3(5.f, 5.f, 5.f);

	Col_MinDesc.vPosition = fColliderPos;
	Col_MinDesc.vRotation = _float3(0.f, 0.f, 0.f);
	Col_MinDesc.vScale = _float3(10.f, 10.f, 10.f);

	memcpy(&DetectDesc.MaxDetect_Collider, &Col_MaxDesc, sizeof(CDetect::COLLIDERDESC));
	memcpy(&DetectDesc.MinDetect_Collider, &Col_MinDesc, sizeof(CDetect::COLLIDERDESC));

	m_pDetect = new CDetect();
	m_pDetect->Initialize(&DetectDesc);

	return S_OK;
}

HRESULT CMadCrow::Ready_Components()
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

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MadCrow"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_SPHERE */
	m_pStuckSocket = m_pModelCom->Get_BonePtr("MadCrow_LCollar");
	Safe_AddRef(m_pStuckSocket);

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, -0.5f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	Ready_Others(ColliderDesc.vPosition);

	return S_OK;
}

HRESULT CMadCrow::SetUp_ShaderResources()
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

CMadCrow * CMadCrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMadCrow*	pInstance = new CMadCrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMadCrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMadCrow::Clone(void * pArg)
{
	CMadCrow*	pInstance = new CMadCrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMadCrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMadCrow::Free()
{
	__super::Free();

	Safe_Release(m_pDetect);

	Safe_Release(m_pStuckSocket);
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}

