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

	Update_Collider();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_Group(CCollider_Manager::GROUP_TYPE::TYPE_MONSTER, this, m_pSPHERECom);
	Safe_AddRef(this);
	RELEASE_INSTANCE(CGameInstance);

	m_eCurrentState = STATE::STATE_IDLE;
	m_eCurrentAnimState = ANIM_STATE::ANIM_IDLE;
	m_pModelCom->Set_CurrentAnimIndex(m_eCurrentAnimState);
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

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

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

_float CMadCrow::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (m_iLife > 0)
	{
		m_iLife -= (_uint)fDamage;
	}
	else
	{
		m_eCurrentState = STATE::STATE_DEAD;
	}

	return fDamage;
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

void CMadCrow::Check_FightMode(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr == pGameInstance)
		return;

	CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
	if (nullptr == pTargetCollider)
		return;

	RELEASE_INSTANCE(CGameInstance);
	if (m_eCurrentAnimState != ANIM_STATE::ANIM_DEATH)
	{
		if (!m_bFightMode)
		{
			if (m_pDetect->Detect_MinRange(pTargetCollider))
			{
				m_bFightMode = true;
				//m_eCurrentState = STATE::STATE_WALK;
				m_fIdleTime = 1.f;
			}
		}
		else

		{	//Fight_ModeÀÏ ¶§
			if (!(m_pDetect->Detect_MaxRange(pTargetCollider)))
			{
				m_bFightMode = false;
				m_eCurrentState = STATE::STATE_IDLE;
			}
		}
	}
}

void CMadCrow::Play_FSM(_float fTimeDelta)
{

	Check_FightMode();

	switch (m_eCurrentState)
	{
	case STATE::STATE_IDLE:
		Idle(fTimeDelta);
		break;
	case STATE::STATE_WALK:
		Move(fTimeDelta);
		break;
	case STATE::STATE_DEAD:
		Death(fTimeDelta);
		break;
	}
	
}

_bool CMadCrow::Idle(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::ANIM_IDLE)
	{
		m_eCurrentAnimState = ANIM_STATE::ANIM_IDLE;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);
	}

	m_fIdleTime -= fTimeDelta;

	if (m_fIdleTime <= 0.f)
	{
		m_fIdleTime = 1.f;
		m_eCurrentState = STATE::STATE_WALK;

		_vector TargetPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		XMStoreFloat3(&m_vTargetPos, TargetPos);
		m_vTargetPos.x += (_float)(rand() % 5 + 3);
		m_vTargetPos.z += (_float)(rand() % 5 + 2);


		if(false == XMVector3Equal(XMLoadFloat3(&m_vHomePos), m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
		{
			_bool bMove = (_bool)(rand() % 2);
			m_vTargetPos = bMove ? m_vTargetPos : m_vHomePos;
		}

		m_bArrive = false;

	}

	return true;
}

_bool CMadCrow::Move(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::ANIM_WALK)
	{
		m_eCurrentAnimState = ANIM_STATE::ANIM_WALK;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);
	}

	if (m_bFightMode)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CHatKid* pTarget = dynamic_cast<CHatKid*>(pGameInstance->Get_FirstObject(LEVEL_STATIC, TEXT("Layer_Player")));

		_vector vLook = pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		m_pTransformCom->LookAt(pTarget->Get_State(CTransform::STATE_TRANSLATION));

		_float fDistance = 0.f;
		XMStoreFloat(&fDistance, (XMVector3Length(pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION))));


		if (false == pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_PLAYER, this, m_pSPHERECom, 1.f))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	else
	{
		if (!m_bArrive)
		{
			m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f));

			_float3 Current_Pos;
			XMStoreFloat3(&Current_Pos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			if (fabs(Current_Pos.x - m_vTargetPos.x) <= 0.2f &&
				fabs(Current_Pos.z - m_vTargetPos.z) <= 0.2f)
			{
				m_eCurrentState = STATE::STATE_IDLE;
				m_fIdleTime = 1.f;
				m_bArrive = true;
			}
			else
			{
				m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);
			}
			
		}
	}

	return true;
}

_bool CMadCrow::Death(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::ANIM_DEATH)
	{
		m_eCurrentAnimState = ANIM_STATE::ANIM_DEATH;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, false);
	}

	if (m_bAnimFinished)
	{
		m_bDelete = true;
	}

	return true;
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

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(rand() % 10 - 15, 0.f, rand() % 10 - 15, 1.f));

	XMStoreFloat3(&m_vHomePos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_MadCrow"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_SPHERE */
	m_pStuckSocket = m_pModelCom->Get_BonePtr("MadCrow_LCollar");
	Safe_AddRef(m_pStuckSocket);

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, -0.5f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
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

