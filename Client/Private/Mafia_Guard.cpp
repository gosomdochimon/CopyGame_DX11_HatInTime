#include "stdafx.h"
#include "..\Public\Mafia_Guard.h"
#include "GameInstance.h"
#include "Detect.h"
#include "HatKid.h"
#include "Equipments.h"
#include "Deligate_Monster.h"
#include "Clerk.h"
CMafia_Guard::CMafia_Guard(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CMafia_Guard::CMafia_Guard(const CMafia_Guard & rhs)
	: CMonster(rhs)
{
}

HRESULT CMafia_Guard::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMafia_Guard::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	memcpy(&m_CloneInfo, (CLONEINFO*)pArg, sizeof(CLONEINFO));

	m_pTransformCom->Set_WorldMatrix(m_CloneInfo.WorldMat);

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;
	XMStoreFloat3(&NaviDesc.fCurrent_Position, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Navigation_Platform"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_Group(CCollider_Manager::GROUP_TYPE::TYPE_MONSTER, this, m_pSPHERECom);
	Safe_AddRef(this);
	XMStoreFloat3(&m_vHomePos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	RELEASE_INSTANCE(CGameInstance);



	Update_Collider();

	Init_Type();

	return S_OK;
}

void CMafia_Guard::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_bDelete)
		return;



	Play_FSM(fTimeDelta);

	Set_Height();

	m_pModelCom->Play_Animation(fTimeDelta, &m_bAnimFinished);



	Update_Collider();

	if (m_pEquipments != nullptr)
		m_pEquipments->Tick(fTimeDelta);

}

void CMafia_Guard::Late_Tick(_float fTimeDelta)
{
	if (m_bDelete)
		return;

	/*CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);*/

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		//m_pRendererCom->Add_Debug(m_pSPHERECom);
		//m_pRendererCom->Add_Debug(m_pDetect->Get_MaxSphere());
		//m_pRendererCom->Add_Debug(m_pDetect->Get_MinSphere());
		//m_pRendererCom->Add_Debug(m_pNavigationCom);
#endif

	}
	if (m_pEquipments != nullptr)
		m_pEquipments->Late_Tick(fTimeDelta);

}

HRESULT CMafia_Guard::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iPassType)))
			return E_FAIL;
	}

#ifdef _DEBUG
	/*m_pAABBCom->Render();
	m_pOBBCom->Render();*/
	//m_pNavigationCom->Render_Navigation();

#endif

	return S_OK;
}

_float CMafia_Guard::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (m_iLife > 0 && !m_bSuper)
	{

		if (m_eCurrentAnimState == ANIM_STATE::GUARDIDLE)
		{
			m_bSuper = true;
			Create_Ring();
			CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
			//std::function<void(CMonster&)> func = &CMonster::Release_Super;
			pDeligate->Add_SuperEvent(CDeligate_Monster::NOTIFY_SUPERMODE, this, (&CMonster::Release_Super));
			RELEASE_INSTANCE(CDeligate_Monster);
			return 0.f;
		}

		m_iLife -= (_uint)fDamage;
		m_bSuper = true;
		if (m_iLife > 0)
		{
			CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
			//std::function<void(CMonster&)> func = &CMonster::Release_Super;
			pDeligate->Add_SuperEvent(CDeligate_Monster::NOTIFY_SUPERMODE, this, (&CMonster::Release_Super));
			m_eCurrentState = BEHAVIOR_STATE::HURT;
			RELEASE_INSTANCE(CDeligate_Monster);
			m_pEquipments->Set_CanAttack(false);
			Create_HitImpact(0.f);
			Create_Ring();
			Play_Sound(5);
		}
		else
		{
			Play_Sound(5);
			m_pEquipments->Set_CanAttack(false);
			Create_HitImpact(0.f);
			m_eCurrentState = BEHAVIOR_STATE::DEAD;
		}
	}

	return fDamage;
}

void CMafia_Guard::Update_Collider(void)
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

void CMafia_Guard::Play_FSM(_float fTimeDelta)
{
	//Check_FightMode();

	//if (!m_bIntroEnd)
	//{
	//	//Walk_ArrviePoint(fTimeDelta);
	//	return;
	//}

	switch (m_eCurrentState)
	{
	case BEHAVIOR_STATE::IDLE:
		Idle(fTimeDelta);
		break;
	case BEHAVIOR_STATE::WALK:
		if (m_bFightMode)
			Follow_Enemy(fTimeDelta);
		else
			Walk_ArrviePoint(fTimeDelta);
		break;
	case BEHAVIOR_STATE::ATTACK:
		Attack(fTimeDelta);
		break;
	case BEHAVIOR_STATE::HURT:
		Hurt(fTimeDelta);
		break;
	case BEHAVIOR_STATE::DEAD:
		Death(fTimeDelta);
		break;
	case BEHAVIOR_STATE::GUARD_IDLE:
		Guard_Idle(fTimeDelta);
		break;
	case BEHAVIOR_STATE::BULLY:
		Bully(fTimeDelta);
		break;
	}
}

_bool CMafia_Guard::Idle(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::IDLE)
	{
		m_eCurrentAnimState = ANIM_STATE::IDLE;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		//m_bSuper = false;
	}

	m_fIdleTime -= fTimeDelta;

	if (m_fIdleTime <= 0.f)
	{
		m_fIdleTime = 1.f;
		m_eCurrentState = BEHAVIOR_STATE::WALK;
		m_eAttackState = (ATTACK_STATE)(rand() % 2);

		/*if (m_bFightMode == false)
		{
		_vector TargetPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		XMStoreFloat3(&m_vTargetPos, TargetPos);
		m_vTargetPos.x += (_float)(rand() % 5 + 3);
		m_vTargetPos.z += (_float)(rand() % 5 + 2);

		if (false == XMVector3Equal(XMLoadFloat3(&m_vHomePos), m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
		{
		_bool bMove = (_bool)(rand() % 2);
		m_vTargetPos = bMove ? m_vTargetPos : m_vHomePos;
		}
		}*/

		m_bArrive = false;

	}

	return true;
}

_bool CMafia_Guard::Move(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::WALK)
	{
		m_eCurrentAnimState = ANIM_STATE::WALK;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
	}

	if (m_bFightMode)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CHatKid* pTarget = dynamic_cast<CHatKid*>(pGameInstance->Get_FirstObject(LEVEL_STATIC, TEXT("Layer_Player")));

		_vector vLook = pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		m_pTransformCom->LookAt(pTarget->Get_State(CTransform::STATE_TRANSLATION));

		_float fDistance = 0.f;
		XMStoreFloat(&fDistance, (XMVector3Length(pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION))));


		if (false == pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_PLAYER, this, m_pSPHERECom))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 0.5f, m_pNavigationCom);
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
				m_eCurrentState = BEHAVIOR_STATE::IDLE;
				m_fIdleTime = 1.f;
				m_bArrive = true;
			}
			else
			{
				m_pTransformCom->Go_Straight(fTimeDelta * 0.5f, m_pNavigationCom);
			}

		}
	}

	return true;
}

_bool CMafia_Guard::Death(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::DEAD)
	{
		m_eCurrentAnimState = ANIM_STATE::DEAD;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_pModelCom->Set_OffsetPerSecond(m_eCurrentAnimState, 2.f);
	}

	if (m_bAnimFinished)
	{
		Create_HitImpact(fTimeDelta);
		Create_Puff();
		CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);

		pDeligate->Notify_MafiaDead();
		RELEASE_INSTANCE(CDeligate_Monster);

		m_bSoundPlay = true;
		Play_Sound(2);
		m_bSoundPlay = true;
		m_bDelete = true;
	}

	return true;
}

_bool CMafia_Guard::Attack(_float fTimeDelta)
{
	_bool bAttackFinished = false;
	switch (m_eAttackState)
	{
	case ATTACK_STATE::PUNCH:
		bAttackFinished = Punch(fTimeDelta);
		break;
	case ATTACK_STATE::DIVE:
		bAttackFinished = Dive_Attack(fTimeDelta);
		break;
	case ATTACK_STATE::THROW:
		bAttackFinished = Throw_Item(fTimeDelta);
		break;
	}

	return bAttackFinished;
}

_bool CMafia_Guard::Follow_Enemy(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::WALK)
	{
		m_eCurrentAnimState = ANIM_STATE::WALK;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);
		m_bAnimFinished = false;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CHatKid* pTarget = dynamic_cast<CHatKid*>(pGameInstance->Get_FirstObject(LEVEL_STATIC, TEXT("Layer_Player")));

	if (pTarget == nullptr)
		return false;

	CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
	if (nullptr == pTargetCollider)
		return false;

	RELEASE_INSTANCE(CGameInstance);

	_vector vLook = pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	m_pTransformCom->LookAt(pTarget->Get_State(CTransform::STATE_TRANSLATION));

	_float fDistance = 0.f;
	XMStoreFloat(&fDistance, (XMVector3Length(pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION))));

	switch (m_eAttackState)
	{
	case ATTACK_STATE::PUNCH:
		if (3.f < fDistance)
		{//거리가 먼경우
			m_pTransformCom->Go_Straight(fTimeDelta * 0.5f, m_pNavigationCom);
			return false;
		}
		else if (3.f >= fDistance && 2.5f <= fDistance)
		{
			//printf_s("Punch Execute \n");
			m_eCurrentState = BEHAVIOR_STATE::ATTACK;
			return true;
		}
		break;
	case ATTACK_STATE::DIVE:
		if (false == IsDetect((_uint)CDetect::SPHERE_SCALE::MIN, pTargetCollider))
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			return false;
		}
		else
		{
			//printf_s("Dive Execute \n");
			m_eCurrentState = BEHAVIOR_STATE::ATTACK;
			return true;
		}
		break;
	case ATTACK_STATE::THROW:
		if (false == IsDetect((_uint)CDetect::SPHERE_SCALE::MAX, pTargetCollider))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 0.5f, m_pNavigationCom);
			return false;
		}
		else
		{
			//printf_s("Throw Execute \n");
			m_eCurrentState = BEHAVIOR_STATE::ATTACK;
			return true;
		}
		break;
	}

	return false;
}

_bool CMafia_Guard::Punch(_float fTimeDelta)
{
	//TODO: 나중에 키프레임제어로 걷고있을 때와 때릴때 애니메이션 제어 해야함.
	if (m_eCurrentAnimState != ANIM_STATE::PUNCH)
	{
		m_eCurrentAnimState = ANIM_STATE::PUNCH;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_bSoundPlay = true;
		Play_Sound(0);
		m_bSoundPlay = true;
	}

	m_fCurrentTime += fTimeDelta;

	if (m_bAnimFinished == true)
	{
		m_eCurrentState = BEHAVIOR_STATE::IDLE;
		m_fCurrentTime = 0.f;
		m_pEquipments->Set_CanAttack(false);

	}
	else
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CHatKid* pTarget = dynamic_cast<CHatKid*>(pGameInstance->Get_FirstObject(LEVEL_STATIC, TEXT("Layer_Player")));

		if (pTarget == nullptr)
			return false;

		CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
		if (nullptr == pTargetCollider)
			return false;

		RELEASE_INSTANCE(CGameInstance);

		_float fDistance = 0.f;
		XMStoreFloat(&fDistance, (XMVector3Length(pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION))));

		if (fDistance >= 1.f)
		{
			_vector vLook = pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			m_pTransformCom->LookAt(pTarget->Get_State(CTransform::STATE_TRANSLATION));

			if (m_fAnimTime > m_fCurrentTime && m_fCurrentTime > 0.5f)
			{
				m_pTransformCom->Go_Straight(fTimeDelta*1.3f, m_pNavigationCom);
				Play_Sound(1);
			}

		}

		if (m_fAnimTime <= m_fCurrentTime)
		{
			m_pEquipments->Set_CanAttack(true);
		}

	}
	return m_bAnimFinished;
}

_bool CMafia_Guard::Throw_Item(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::THROW_ITEM)
	{
		m_eCurrentAnimState = ANIM_STATE::THROW_ITEM;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::ORANGE);
		//m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::TYPE_NONE);
		m_bThrow = true;

		m_bSoundPlay = true;
		Play_Sound(0);
		m_bSoundPlay = true;
	}

	m_fCurrentTime += fTimeDelta;

	if (m_bAnimFinished == true)
	{
		m_eCurrentState = BEHAVIOR_STATE::IDLE;
		m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::PUNCH);
		m_bThrow = true;
		m_fCurrentTime = 0.f;
	}
	else
	{
		if (m_fCurrentTime >= 1.5f)
		{
			//Throwing Obj로 바꾸자.
			if (m_bThrow == false)
			{
				m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::PUNCH);
				//m_eWeaponType = WEAPON_TYPE::PUNCH;
				//return false;
			}
			else
			{
				m_bThrow = false;

				CEquipment::EQUIPDESC EquipDesc;
				//Test_Flask
				CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("bip_handR_index01");
				if (nullptr == pSocket)
					return false;

				EquipDesc.pSocket = pSocket;
				EquipDesc.szName = TEXT("ThrowingObject");
				EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
				EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
				EquipDesc.bIsDepartment = true;
				EquipDesc.fOwnerMatrix = m_pTransformCom->Get_World4x4();
				EquipDesc.bIsEnemy = true;
				//XMStoreFloat4x4(&(EquipDesc.fOwnerMatrix), pSocket->Get_CombinedTransformationMatrix());
				Safe_AddRef(pSocket);

				if (FAILED(m_pEquipments->Add_Department_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::ORANGE, &EquipDesc)))
					return false;
			}

		}
	}

	return m_bAnimFinished;
}

_bool CMafia_Guard::Dive_Attack(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::DIVE)
	{
		m_eCurrentAnimState = ANIM_STATE::DIVE;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_fCurrentTime = 0.f;
		m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::HEAD);

		m_bSoundPlay = true;
		Play_Sound(0);
		m_bSoundPlay = true;
	}
	m_fCurrentTime += fTimeDelta;
	if (m_bAnimFinished == true)
	{
		m_fCurrentTime = 0.f;
		m_eCurrentState = BEHAVIOR_STATE::IDLE;
		m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::PUNCH);
		m_pEquipments->Set_CanAttack(false);
	}
	else
	{
		if (m_fCurrentTime <= 1.5f)
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 1.3f, m_pNavigationCom);
			m_pEquipments->Set_CanAttack(true);
		}

	}
	return m_bAnimFinished;
}

_bool CMafia_Guard::Hurt(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::FLINCH)
	{
		m_eCurrentAnimState = ANIM_STATE::FLINCH;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::TYPE_NONE);
		m_fCurrentTime = 0.f;
		m_iPassType = 1;

		m_bSoundPlay = true;
		Play_Sound(3);
		m_bSoundPlay = true;
	}

	//m_bSuper = true;
	m_fCurrentTime += fTimeDelta;

	if (m_fCurrentTime >= .3f)
	{
		m_iPassType = 0;
		//m_bSuper = false;
		m_bAnimFinished = true;
		m_fCurrentTime = 0.f;
		m_eCurrentState = BEHAVIOR_STATE::IDLE;
		m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::TYPE_NONE);
	}


	return m_bAnimFinished;
}

_bool CMafia_Guard::Sleep(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::SLEEP)
	{
		m_eCurrentAnimState = ANIM_STATE::SLEEP;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::TYPE_NONE);
		m_fCurrentTime = 0.f;
		m_iPassType = 1;

		m_bSoundPlay = true;
		Play_Sound(3);
		m_bSoundPlay = true;
	}

	//m_bSuper = true;
	//m_fCurrentTime += fTimeDelta;

	//if (m_fCurrentTime >= .3f)
	//{
	//	m_iPassType = 0;
	//	//m_bSuper = false;
	//	m_bAnimFinished = true;
	//	m_fCurrentTime = 0.f;
	//	m_eCurrentState = BEHAVIOR_STATE::IDLE;
	//	m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::TYPE_NONE);
	//}


	return m_bAnimFinished;
}

_bool CMafia_Guard::Guard_Idle(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::GUARDIDLE)
	{
		m_eCurrentAnimState = ANIM_STATE::GUARDIDLE;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::TYPE_NONE);
		m_fCurrentTime = 0.f;
		m_iPassType = 0;

		m_bSoundPlay = true;
		Play_Sound(3);
		m_bSoundPlay = true;
	}
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (false == pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_PLAYER, this, m_pSPHERECom))
	{
		CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
		//std::function<void(CMonster&)> func = &CMonster::Release_Super;
		CClerk* pTarget = dynamic_cast<CClerk*>(pDeligate->Get_Npc());
		RELEASE_INSTANCE(CDeligate_Monster);

		if (pTarget->Get_Happy() == true)
		{
			if (pGameInstance->Key_Down(VK_RETURN))
			{//TODO: 대화 후 문 열어주기 
				CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);

				CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_head");
				_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ /*m_pStuckSocket->Get_OffsetMatrix() **/
					m_pStuckSocket->Get_CombinedTransformationMatrix() *
					XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

				SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
				SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
				SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
				_float4x4 m_CombinedWorldMatrix;
				XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

				CEffect::EFFECTINFO EffectInfo;
				EffectInfo.iTypeNum = 1;
				EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
				EffectInfo.iMoveType = 0;
				EffectInfo.fAngle = 0.f;
				EffectInfo.fLifeTime = 2.f;

				m_pNavigationCom->Change_CellType_Group(0, 35, 0);
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Talk"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_Effect"), &EffectInfo)))
					return m_bAnimFinished;

				Safe_Release(pGameInstance);
			}
		}
		else
		{//TODO: 거절맨트 띄우기
			if (pGameInstance->Key_Down(VK_RETURN))
			{//TODO: 대화 후 문 열어주기 
				CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);

				CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_head");
				_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ /*m_pStuckSocket->Get_OffsetMatrix() **/
					m_pStuckSocket->Get_CombinedTransformationMatrix() *
					XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

				SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
				SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
				SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
				_float4x4 m_CombinedWorldMatrix;
				XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

				CEffect::EFFECTINFO EffectInfo;
				EffectInfo.iTypeNum = 0;
				EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
				EffectInfo.iMoveType = 0;
				EffectInfo.fAngle = 0.f;
				EffectInfo.fLifeTime = 2.f;


				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Talk"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_Effect"), &EffectInfo)))
					return m_bAnimFinished;

				Safe_Release(pGameInstance);
			}
		}

	}
	RELEASE_INSTANCE(CGameInstance);

	return m_bAnimFinished;
}

_bool CMafia_Guard::Bully(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::BULL)
	{
		m_eCurrentAnimState = ANIM_STATE::BULL;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::TYPE_NONE);
		m_fCurrentTime = 0.f;
		m_iPassType = 1;

		m_bSoundPlay = true;
		Play_Sound(3);
		m_bSoundPlay = true;
	}

	//m_bSuper = true;
	//m_fCurrentTime += fTimeDelta;



	//if (m_fCurrentTime >= .3f)
	//{
	//	m_iPassType = 0;
	//	//m_bSuper = false;
	//	m_bAnimFinished = true;
	//	m_fCurrentTime = 0.f;
	//	m_eCurrentState = BEHAVIOR_STATE::IDLE;
	//	m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::TYPE_NONE);
	//}


	return m_bAnimFinished;
}

void CMafia_Guard::Detect_TeamFight()
{


}

void CMafia_Guard::Create_Ring()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("Root");
	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ /*m_pStuckSocket->Get_OffsetMatrix() **/
		m_pStuckSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 m_CombinedWorldMatrix;
	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

	CEffect::EFFECTINFO EffectInfo;
	EffectInfo.iTypeNum = 0;
	EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
	EffectInfo.iMoveType = 0;
	EffectInfo.fAngle = 0.f;
	EffectInfo.fLifeTime = 0.f;


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Ring"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_Effect"), &EffectInfo)))
		return;

	Safe_Release(pGameInstance);
}

void CMafia_Guard::Create_HitImpact(_float fTimeDelta)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("Root");
	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ /*m_pStuckSocket->Get_OffsetMatrix() **/
		m_pStuckSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 m_CombinedWorldMatrix;
	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

	CEffect::EFFECTINFO EffectInfo;
	EffectInfo.iTypeNum = 0;
	EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
	EffectInfo.iMoveType = 0;
	EffectInfo.fAngle = 0.f;
	EffectInfo.fLifeTime = 1.f;


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HitEffect"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_Effect"), &EffectInfo)))
		return;

	Safe_Release(pGameInstance);
}

void CMafia_Guard::Create_Puff()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("Root");
	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ /*m_pStuckSocket->Get_OffsetMatrix() **/
		m_pStuckSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 m_CombinedWorldMatrix;
	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

	for (_uint i = 0; i < 3; ++i)
	{
		CEffect::EFFECTINFO EffectInfo;
		EffectInfo.iTypeNum = 3;
		EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
		EffectInfo.vPosition.x += rand() % 3;
		EffectInfo.vPosition.y += rand() % 3;
		EffectInfo.vPosition.z += rand() % 3;
		EffectInfo.iMoveType = 0;
		EffectInfo.fAngle = 0.f;
		EffectInfo.fLifeTime = 1.f;


		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_Effect"), &EffectInfo)))
			return;
	}



	Safe_Release(pGameInstance);

}

void CMafia_Guard::Play_Sound(_uint iSoundNum)
{
	if (!m_bSoundPlay)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_tchar	szFullPath[MAX_PATH];// = TEXT("footstep_grass_%d.wav");
	_uint iNum = 0;
	_float fVolume = 0.7f;
	switch (iSoundNum)
	{
	case 0://걸어가며 때리기 
		iNum = rand() % 6 + 1;
		wcscpy_s(szFullPath, TEXT("attack_charge0%d.ogg"));
		wsprintf(szFullPath, szFullPath, iNum);
		pGameInstance->PlaySounds(szFullPath, SOUND_ENEMY, fVolume);
		break;
	case 1://펀치 DIVE
		iNum = rand() % 8 + 1;
		wcscpy_s(szFullPath, TEXT("attack_impact0%d.ogg"));
		wsprintf(szFullPath, szFullPath, iNum);
		pGameInstance->PlaySounds(szFullPath, SOUND_ENEMY, fVolume);
		break;
	case 2://죽을 때 
		pGameInstance->PlaySounds(TEXT("enemy_explosion.ogg"), SOUND_ENEMY, fVolume);
		break;
	case 3: //HURT
		iNum = rand() % 2;
		if (iNum == 1)
			pGameInstance->PlaySounds(TEXT("hurt_short01.ogg"), SOUND_ENEMY, fVolume);
		else
			pGameInstance->PlaySounds(TEXT("hurt_short02.ogg"), SOUND_ENEMY, fVolume);
		break;
	case 4: // blocking
		pGameInstance->PlaySounds(TEXT("blocking01.ogg"), SOUND_ENEMY, fVolume);
		break;
	case 5:
		fVolume = 0.5f;
		iNum = rand() % 3;
		wcscpy_s(szFullPath, TEXT("AttackOverlay%d.ogg"));
		wsprintf(szFullPath, szFullPath, iNum);
		pGameInstance->PlaySounds(szFullPath, SOUND_EFFECT, fVolume);
		break;
	}
	m_bSoundPlay = false;
	RELEASE_INSTANCE(CGameInstance);
}

void CMafia_Guard::Check_FightMode()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr == pGameInstance)
		return;

	CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
	if (nullptr == pTargetCollider)
		return;

	RELEASE_INSTANCE(CGameInstance);

	if (m_eCurrentAnimState == ANIM_STATE::DEAD)
		return;

	if (!m_bFightMode)
	{
		//if (m_pDetect->Detect_MinRange(pTargetCollider))
		if (IsDetect((_uint)CDetect::SPHERE_SCALE::MIN, pTargetCollider))
		{
			m_bFightMode = true;
			//m_eCurrentState = STATE::STATE_WALK;
			m_fIdleTime = 1.f;
		}
	}
	else

	{	//Fight_Mode일 때
		if (!(m_pDetect->Detect_MaxRange(pTargetCollider)))
		{
			if (true == m_bAnimLoop)
			{
				m_bFightMode = false;
				m_eCurrentState = BEHAVIOR_STATE::IDLE;
			}
			else
			{
				if (true == m_bAnimFinished)
				{
					m_bFightMode = false;
					m_eCurrentState = BEHAVIOR_STATE::IDLE;
				}
			}

		}
	}

}

_bool CMafia_Guard::IsDetect(_uint eSphere_Scale, CCollider* pTargetCollider)
{
	CDetect::SPHERE_SCALE eSphere = (CDetect::SPHERE_SCALE)eSphere_Scale;

	switch (eSphere)
	{
	case CDetect::SPHERE_SCALE::MAX:
		if (m_pDetect->Detect_MaxRange(pTargetCollider))
			return true;
		else
			return false;
		break;
	case CDetect::SPHERE_SCALE::MIN:
		if (m_pDetect->Detect_MinRange(pTargetCollider))
			return true;
		else
			return false;
		break;
	}

	return false;
}

void CMafia_Guard::Set_Height()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	fPos.y = m_pNavigationCom->Get_NavigationHeight(fPos);
	vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	//printf_s("%f", fPos.y);
	m_pNavigationCom->isMove(vPos);
}

void CMafia_Guard::IsShoot_Objects(_bool bShoot)
{
	//if (m_eAttackState != ATTACK_STATE::THROW)
	//	return;



	//if (bShoot == false)
	//{
	//	m_pEquipments->Delete_Part(PARTS_WEAPON, WEAPON_FLASK);
	//	m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_NONE);
	//	m_eWeaponType = WEAPON_TYPE::WEAPON_NONE;
	//	return;
	//}
	//else
	//{
	//	m_pEquipments->Delete_Part(PARTS_WEAPON, WEAPON_FLASK);
	//	m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_NONE);
	//	m_eWeaponType = WEAPON_TYPE::WEAPON_NONE;

	//	CEquipment::EQUIPDESC EquipDesc;
	//	//Test_Flask
	//	CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("bip_handR_index01");
	//	if (nullptr == pSocket)
	//		return;

	//	EquipDesc.pSocket = pSocket;
	//	EquipDesc.szName = TEXT("Flask");
	//	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	//	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	//	EquipDesc.bIsDepartment = true;
	//	EquipDesc.fOwnerMatrix = m_pTransformCom->Get_World4x4();
	//	//XMStoreFloat4x4(&(EquipDesc.fOwnerMatrix), pSocket->Get_CombinedTransformationMatrix());
	//	Safe_AddRef(pSocket);

	//	if (FAILED(m_pEquipments->Add_Department_Part(PARTS_WEAPON, WEAPON_FLASK, &EquipDesc)))
	//		return;

	//	m_bIsShoot = false;
	//}


}

HRESULT CMafia_Guard::Create_ThrowingObject()
{
	return E_NOTIMPL;
}

void CMafia_Guard::Walk_ArrviePoint(_float fTimeDelta)
{
	if (m_CloneInfo.iTypeNum == 1)
	{
		if (!m_bArrive)
		{
			if (m_eCurrentAnimState != ANIM_STATE::WALK)
			{
				m_eCurrentAnimState = ANIM_STATE::WALK;
				m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);
				m_bAnimFinished = false;

				_vector vMyPos = XMLoadFloat3(&m_vArrivePoints[0]);
				//vMyPos -= m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				//vMyPos = XMVector3Normalize(vMyPos);
				//m_pTransformCom->LookAt_Dir(vMyPos);
				m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&m_vArrivePoints[0]), 1.f));
			}

			_float3 Current_Pos;
			XMStoreFloat3(&Current_Pos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			if (fabs(Current_Pos.x - m_vArrivePoints[m_iPointNum].x) <= 0.2f &&
				fabs(Current_Pos.z - m_vArrivePoints[m_iPointNum].z) <= 0.2f)
			{
				
				m_eCurrentState = BEHAVIOR_STATE::IDLE;
				m_fIdleTime = 1.f;
				m_bArrive = true;
				m_bIntroEnd = true;

				if (m_iPointNum == 0)
				{
					CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
					pDeligate->Notify_MafiaArrvie();
					RELEASE_INSTANCE(CDeligate_Monster);

					Set_DeleteObject();
				}
				else
				{
					--m_iPointNum;
				}
			}
			else
			{
				m_pTransformCom->Go_Straight(fTimeDelta * 0.6f, m_pNavigationCom);
			}

		}
		else
		{
			if (m_eCurrentAnimState != ANIM_STATE::IDLE)
			{
				m_eCurrentAnimState = ANIM_STATE::IDLE;
				m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);
				m_bAnimFinished = false;
			}
		}

	}
}

void CMafia_Guard::Init_Type()
{
	_uint i = rand() % 3 + 13;
	CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
	_float3 vPos;
	switch (m_CloneInfo.iTypeNum)
	{
	case 0://Guard_Idle
		m_eCurrentState = BEHAVIOR_STATE::GUARD_IDLE;
		m_eCurrentAnimState = ANIM_STATE::IDLE;
		m_pModelCom->Set_CurrentAnimIndex(m_eCurrentAnimState);

		
		m_bFightMode = true;
		break;
	case 1://WalkTO Enter
		m_iLife = 2;
		m_eCurrentState = BEHAVIOR_STATE::IDLE;
		m_eCurrentAnimState = ANIM_STATE::IDLE;
		m_pModelCom->Set_CurrentAnimIndex(m_eCurrentAnimState);
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);
		pDeligate->Add_ArriveEvent();
		m_vArrivePoints[0] = _float3(-0.1f, 1.6f, -6.5f);
		m_vArrivePoints[1] = _float3(-0.1f, 1.6f, -10.f);
		m_vArrivePoints[2] = _float3(-0.1f, 1.6f, -16.f);
		
		XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		if (m_vArrivePoints[0].z > vPos.z && vPos.z >= m_vArrivePoints[1].z)
		{
			m_iPointNum = 0;
		}
		else if (m_vArrivePoints[1].z > vPos.z && vPos.z >= m_vArrivePoints[2].z)
		{
			m_iPointNum = 1;
		}
		else
		{
			m_iPointNum = 2;
		}
		m_bFightMode = false;
		m_bArrive = false;
		break;
	case 2://Bully
		m_eCurrentState = BEHAVIOR_STATE::BULLY;
		m_eCurrentAnimState = ANIM_STATE::BULL;
		m_pModelCom->Set_CurrentAnimIndex(m_eCurrentAnimState);
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);
		m_bFightMode = true;
		break;
	}

	


	RELEASE_INSTANCE(CDeligate_Monster);

}

void CMafia_Guard::Ready_Detect(_float3 fColliderPos)
{
	CDetect::DETECTDESC DetectDesc;
	CDetect::COLLIDERDESC Col_MaxDesc, Col_MinDesc;

	ZeroMemory(&DetectDesc, sizeof(CDetect::DETECTDESC));
	ZeroMemory(&Col_MaxDesc, sizeof(CDetect::COLLIDERDESC));
	ZeroMemory(&Col_MinDesc, sizeof(CDetect::COLLIDERDESC));
	Col_MaxDesc.vPosition = fColliderPos;
	Col_MaxDesc.vRotation = _float3(0.f, 0.f, 0.f);
	Col_MaxDesc.vScale = _float3(15.f, 15.f, 15.f);

	Col_MinDesc.vPosition = fColliderPos;
	Col_MinDesc.vRotation = _float3(0.f, 0.f, 0.f);
	Col_MinDesc.vScale = _float3(30.f, 30.f, 30.f);

	memcpy(&DetectDesc.MaxDetect_Collider, &Col_MaxDesc, sizeof(CDetect::COLLIDERDESC));
	memcpy(&DetectDesc.MinDetect_Collider, &Col_MinDesc, sizeof(CDetect::COLLIDERDESC));

	m_pDetect = new CDetect();
	m_pDetect->Initialize(&DetectDesc);
}

HRESULT CMafia_Guard::Ready_Parts()
{
	m_pEquipments = new CEquipments;
	if (FAILED(m_pEquipments->Initialize(1)))
		return E_FAIL;

	//0번째는 장비착용 X
	if (FAILED(m_pEquipments->Add_Parts(0, (_uint)WEAPON_TYPE::TYPE_NONE)))
		return E_FAIL;


	CEquipment::EQUIPDESC EquipDesc;
	CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("bip_handR_index01");
	//Test_Weapon_Punch
	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Punch");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	EquipDesc.bIsEnemy = true;
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::PUNCH, &EquipDesc)))
		return E_FAIL;

	//Throwing Item
	//pSocket = m_pModelCom->Get_BonePtr("bip_handL_middle01");
	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("ThrowingObject");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	EquipDesc.bIsEnemy = true;
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::ORANGE, &EquipDesc)))
		return E_FAIL;

	//Diveing Head
	pSocket = m_pModelCom->Get_BonePtr("bip_head");
	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Punch");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	EquipDesc.bIsEnemy = true;
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::HEAD, &EquipDesc)))
		return E_FAIL;


	m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::PUNCH);

	m_eWeaponType = WEAPON_TYPE::PUNCH;

	return S_OK;
}

HRESULT CMafia_Guard::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Mafia"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_SPHERE */
	m_pStuckSocket = m_pModelCom->Get_BonePtr("Root");
	Safe_AddRef(m_pStuckSocket);
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, -1.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	Ready_Detect(ColliderDesc.vPosition);
	if (FAILED(Ready_Parts()))
		return E_FAIL;




	return S_OK;
}

HRESULT CMafia_Guard::SetUp_ShaderResources()
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

CMafia_Guard * CMafia_Guard::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMafia_Guard*	pInstance = new CMafia_Guard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMafia_Guard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMafia_Guard::Clone(void * pArg)
{
	CMafia_Guard*	pInstance = new CMafia_Guard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMafia_Guard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMafia_Guard::Free()
{
	__super::Free();
	Safe_Release(m_pStuckSocket);
	//이상하게 왜 릭이 안났지??????????
	Safe_Release(m_pEquipments);
	////////////////////
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pDetect);
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}

