#include "stdafx.h"
#include "..\Public\MafiaBoss.h"
#include "GameInstance.h"
#include "Detect.h"
#include "HatKid.h"
#include "Equipments.h"
#include "Deligate_Monster.h"
#include "BT_MafiaBoss.h"
#include "MafiaBoss.h"
#include "Effect.h"
#include "Interactive_Manager.h"
#include "Camera_Manager.h"
CMafiaBoss::CMafiaBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CMafiaBoss::CMafiaBoss(const CMafiaBoss & rhs)
	: CMonster(rhs)
{
}

HRESULT CMafiaBoss::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMafiaBoss::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_PawnDesc.fRunSpeed = 1.5f;
	m_PawnDesc.fWalkSpeed = 0.7f;
	m_PawnDesc.iMaxHp = 4;
	m_PawnDesc.iCurrentHp = m_PawnDesc.iMaxHp;

	if (FAILED(Init_Position()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	Set_Height();

	pGameInstance->Add_Group(CCollider_Manager::GROUP_TYPE::TYPE_MONSTER, this, m_pSPHERECom);
	Safe_AddRef(this);
	RELEASE_INSTANCE(CGameInstance);

	m_eCurrentState = BEHAVIOR_STATE::IDLE;
	m_eCurrentAnimState = ANIM_STATE::IDLE;
	m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);

	Update_Collider();

	/*BehaviorTree Init*/
	m_pBehaviorTree = new CBT_MafiaBoss(this);
	m_pBehaviorTree->Initialize();


	CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
	pDeligate->Add_BossEvent(CDeligate_Monster::NOTIFY_TYPE::NOTIFY_END, this);
	RELEASE_INSTANCE(CDeligate_Monster);

	CInteractive_Manager* pInter = GET_INSTANCE(CInteractive_Manager);

	pInter->Set_Bossptr(this);
	RELEASE_INSTANCE(CInteractive_Manager);
	return S_OK;
}

void CMafiaBoss::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_bDelete)
		return;


	
	if (m_bEffectStart)
	{
		m_fDeadTimer += fTimeDelta;
		m_fPuffTimer += fTimeDelta;

		if (m_fPuffTimer >= 1.f)
		{
			Create_Puff();
		}
		if (m_fDeadTimer >= 3.f)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			_float fVolume = 0.7f;
			pGameInstance->PlayBGM(TEXT("Clocktowers_Remix_Loop.ogg"), fVolume);
			RELEASE_INSTANCE(CGameInstance);
			Set_DeleteObject();
		}
	}
	else {
		if (!m_bIntroEnd)
		{
			Intro(fTimeDelta);
		}
		else
		{
			if (m_pBehaviorTree != nullptr)
				m_pBehaviorTree->Tick(fTimeDelta);

		}		
	}

	

	m_pModelCom->Play_Animation(fTimeDelta, &m_bAnimFinished);

	Set_Height();

	Update_Collider();

	if (m_pEquipments != nullptr)
		m_pEquipments->Tick(fTimeDelta);

}

void CMafiaBoss::Late_Tick(_float fTimeDelta)
{
	if (m_bDelete)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom || !m_bEffectStart)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		if (m_pEquipments != nullptr)
			m_pEquipments->Late_Tick(fTimeDelta);

#ifdef _DEBUG
		//m_pRendererCom->Add_Debug(m_pSPHERECom);
		//m_pRendererCom->Add_Debug(m_pDetect->Get_MaxSphere());
		//m_pRendererCom->Add_Debug(m_pDetect->Get_MinSphere());
#endif
	}
	
}

HRESULT CMafiaBoss::Render()
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


}

_float CMafiaBoss::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (m_PawnDesc.iCurrentHp > 0 && !m_bSuper && !m_bHurt && m_bCanDmged)
	{
		m_PawnDesc.iCurrentHp -= (_uint)1;
		m_bSuper = true;
		CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
		//std::function<void(CMonster&)> func = &CMonster::Release_Super;
		m_bHurt = true;
		pDeligate->Add_SuperEvent(CDeligate_Monster::NOTIFY_SUPERMODE, this, (&CMonster::Release_Super));
		RELEASE_INSTANCE(CDeligate_Monster);
		m_bCanDmged = false;

		if (m_eCurrentAnimState == ANIM_STATE::DIZZY)
		{
			CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
			m_fDizzyTime = 3.f;
			pDeligate->Notify_DizzyEffect();
			RELEASE_INSTANCE(CDeligate_Monster);
		}

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("MafiaBoss_Hurt03.ogg"), SOUND_ENEMY, fVolume);
		RELEASE_INSTANCE(CGameInstance);


		if (/*m_PawnDesc.iCurrentHp == 6 ||*/
			m_PawnDesc.iCurrentHp == 3 ||
			m_PawnDesc.iCurrentHp == 0)
		{
			printf_s("HP: %d \n", m_PawnDesc.iCurrentHp);

			if (m_PawnDesc.iCurrentHp == 0)
			{
				m_bDead = true;
			}
			else
			{
				m_bPhaseChanged = true;
			}
		}

		//Reset해야할것들
		{
			m_fSpinTime = 3.f;
			m_fJumpPower = 2.f;
			m_fIdleTime = 1.f;
		}


	}
	//else if (m_PawnDesc.iCurrentHp == 0 && !m_bSuper)
	//{
	//	m_eCurrentState = BEHAVIOR_STATE::DEAD;
	//}

	return fDamage;
}

_float4x4 CMafiaBoss::Get_BonePtrMatrix(const char * pBoneName)
{
	CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr(pBoneName);
	if (nullptr == m_pStuckSocket)
		return _float4x4();
	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ /*m_pStuckSocket->Get_OffsetMatrix() **/
		m_pStuckSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 m_CombinedWorldMatrix;
	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

	//EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];

	return m_CombinedWorldMatrix;
}

void CMafiaBoss::Update_Collider(void)
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

void CMafiaBoss::Play_FSM(_float fTimeDelta)
{
	Check_FightMode();

	switch (m_eCurrentState)
	{
	case BEHAVIOR_STATE::IDLE:
		Idle(fTimeDelta);
		break;
	case BEHAVIOR_STATE::WALK:
		Follow_Enemy(fTimeDelta);
		break;
	case BEHAVIOR_STATE::ATTACK:
		Attack(fTimeDelta);
		break;
	case BEHAVIOR_STATE::DEAD:
		Death(fTimeDelta);
		break;
	}
}

_bool CMafiaBoss::Idle(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::IDLE)
	{
		printf_s("Idle\n");
		m_eCurrentAnimState = ANIM_STATE::IDLE;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bPatternEnd = true;
	}
	m_fIdleTime -= fTimeDelta;

	if (m_fIdleTime <= 0.f)
	{
		m_fIdleTime = 1.f;

		Select_Pattern();
		m_bPatternEnd = false;
	}
	else
	{
		m_bFightMode = true;
	}
	/*m_fIdleTime -= fTimeDelta;

	if (m_fIdleTime <= 0.f)
	{
	m_fIdleTime = 1.f;
	m_eCurrentState = BEHAVIOR_STATE::WALK;

	m_eAttackState = (ATTACK_STATE)(rand() % 3);

	if (m_bFightMode == false)
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
	}

	m_bArrive = false;

	}*/

	return !m_bPatternEnd;
}

_bool CMafiaBoss::Move(_float fTimeDelta)
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
				m_eCurrentState = BEHAVIOR_STATE::IDLE;
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

_bool CMafiaBoss::Death(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::BACKFLIP)
	{
		m_eCurrentAnimState = ANIM_STATE::BACKFLIP;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
	}

	if (m_bAnimFinished)
	{
		m_bDelete = true;
	}

	return true;
}

_bool CMafiaBoss::Attack(_float fTimeDelta)
{
	_bool bAttackFinished = false;
	switch (m_eAttackState)
	{
	case ATTACK_STATE::SLASH:
		bAttackFinished = SlashAttack(fTimeDelta);
		break;
	case ATTACK_STATE::SPIN_MOVE:
		bAttackFinished = SpinAttack_Move(fTimeDelta);
		break;
	case ATTACK_STATE::THROW:
		bAttackFinished = Throw_Blade(fTimeDelta);
		break;
	}

	return bAttackFinished;
}

_bool CMafiaBoss::Damaged(_float fTimeDelta)
{
	return _bool();
}

_bool CMafiaBoss::Follow_Enemy(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::WALK)
	{
		m_eCurrentAnimState = ANIM_STATE::WALK;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState);
		m_bAnimFinished = false;
	}

	if (m_bFightMode)
	{
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

		return false;
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
				return true;
			}
			else
			{
				m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);
				return false;
			}

		}
	}

	return false;
}

_bool CMafiaBoss::SlashAttack(_float fTimeDelta)
{
	//TODO: 나중에 키프레임제어로 걷고있을 때와 때릴때 애니메이션 제어 해야함.
	if (m_eCurrentAnimState != ANIM_STATE::SLASHWALK)
	{
		//printf_s("slash\n");
		m_eCurrentAnimState = ANIM_STATE::SLASHWALK;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_pModelCom->Set_OffsetPerSecond(m_eCurrentAnimState, 2.f);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("Knife_Throw_SFX.ogg"), SOUND_ENEMY, fVolume);
		RELEASE_INSTANCE(CGameInstance);
	}

	if (m_bAnimFinished == true)
	{
		int i = 1;
		//m_eCurrentState = BEHAVIOR_STATE::IDLE;
	}
	return m_bAnimFinished;
}

_bool CMafiaBoss::Throw_Blade(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::WEPON_REACH)
	{
		printf_s("Throw\n");
		m_eCurrentAnimState = ANIM_STATE::WEPON_REACH;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		m_pEquipments->Change_Part((_uint)PARTS_TYPE::PARTS_WEAPON, (_uint)WEAPON_TYPE::TYPE_NONE);
		m_pEquipments->Change_Part((_uint)PARTS_TYPE::PARTS_SUBWEAPON, (_uint)WEAPON_TYPE::TYPE_NONE);


		CEquipment::EQUIPDESC EquipDesc;
		//Test_BLADE
		/* Blade
		1페이즈엔 2개 2페이즈엔 4개 3페이즈엔 6개를 던질 것이다.
		처음엔 2초의 시간 후에 하나씩 0.5초마다 날라가게 할 것이다.

		*/
		CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("Boss01-R-Hand");
		if (nullptr == pSocket)
			return false;

		EquipDesc.pSocket = pSocket;
		EquipDesc.szName = TEXT("Blade");
		EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
		EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		EquipDesc.bIsDepartment = true;
		EquipDesc.fOwnerMatrix = m_pTransformCom->Get_World4x4();
		EquipDesc.fPivotAngle = 1.5f;
		//XMStoreFloat4x4(&(EquipDesc.fOwnerMatrix), pSocket->Get_CombinedTransformationMatrix());
		Safe_AddRef(pSocket);

		if (FAILED(m_pEquipments->Add_Department_Part(PARTS_WEAPON, (_uint)WEAPON_TYPE::THROWING_BLADE, &EquipDesc)))
			return false;

		pSocket = m_pModelCom->Get_BonePtr("Boss01-L-Hand");
		if (nullptr == pSocket)
			return false;

		EquipDesc.pSocket = pSocket;

		if (FAILED(m_pEquipments->Add_Department_Part(PARTS_SUBWEAPON, (_uint)WEAPON_TYPE::THROWING_BLADE, &EquipDesc)))
			return false;

		_vector vDir = XMLoadFloat3(&m_LedgePoints[1]) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vDir = XMVectorSetY(vDir, 0.f);
		vDir = XMVectorSetW(vDir, 0.f);
		_float3 fDir;
		XMStoreFloat3(&fDir, vDir);
		fDir.x = -fDir.x;
		vDir = XMLoadFloat3(&fDir);
		m_pTransformCom->LookAt_Dir(vDir);
	}

	/*if (m_bAnimFinished == true)
	{
	m_eCurrentState = BEHAVIOR_STATE::IDLE;
	m_pEquipments->Change_Part((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::BLADE);
	}*/
	return m_bAnimFinished;
}

_bool CMafiaBoss::Spin_Charge(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::SPIN_CHAREGE)
	{
		printf_s("charge\n");
		m_eCurrentAnimState = ANIM_STATE::SPIN_CHAREGE;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		_float fVolume = 0.7f;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_uint iNum = 0;
		iNum = rand() % 2;
		if(iNum == 0)
			pGameInstance->PlaySounds(TEXT("MafiaBoss_Spin_Charge01.ogg"), SOUND_ENEMY, fVolume);
		else
			pGameInstance->PlaySounds(TEXT("MafiaBoss_Spin_Charge02.ogg"), SOUND_ENEMY, fVolume);
		RELEASE_INSTANCE(CGameInstance);
	}

	if (m_bAnimFinished == true)
	{
		//m_eCurrentState = BEHAVIOR_STATE::IDLE;
	}
	return m_bAnimFinished;
}

_bool CMafiaBoss::SpinAttack_Idle(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::SPIN_ATTACK)
	{
		printf_s("spin_idle\n");
		m_eCurrentAnimState = ANIM_STATE::SPIN_ATTACK;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_bAttack = true;
		m_fSpinTime = 6.f;
		if (m_eCurrentPhase == PHASE_TYPE::PHASE_1)
		{
			m_fBulletDelayAcc = 3.f;
		}
		else
		{
			m_fBulletDelayAcc = 1.5f;
		}
		m_fSpinTime = 6.f;
		m_pModelCom->Set_OffsetPerSecond(m_eCurrentAnimState, 2.5f);
		Create_RotationEffect();
	}

	m_fSpinTime -= fTimeDelta;
	if (m_fSpinTime < 0.f)
	{
		m_fBulletDelayAcc = 0.f;
		m_bAttack = false;
		m_fSpinTime = 3.f;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("Spin_Spinning_Loop_SFX.ogg"), SOUND_ENEMYSKILL, fVolume);
		RELEASE_INSTANCE(CGameInstance);
	}
	else
	{
		

		m_fBulletDelayAcc -= fTimeDelta;

		if (0.f >= m_fBulletDelayAcc)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			_float fVolume = 0.7f;
			pGameInstance->PlaySounds(TEXT("Electro_RedShot_SFX.ogg"), SOUND_ENEMYSKILL, fVolume);
			RELEASE_INSTANCE(CGameInstance);

			for (_uint i = 0; i < 20; ++i)
			{
				/*_vector      vTranslation = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_vector      vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				vTranslation += vMyLook;
				vTranslation.m128_f32[1] += 1.3f;
				_float3      fTranslation, fLook;

				XMStoreFloat3(&fTranslation, vTranslation);
				XMStoreFloat3(&fLook, vMyLook);

				_float fAngle = XMConvertToRadians(9.f * i) - 45.f;*/

				_float fAngle = XMConvertToRadians(18.f * i)/* - 45.f*/;
				_float4x4	f4x4Mat;
				_matrix		MyMatrix = XMMatrixIdentity();//m_pTransformCom->Get_WorldMatrix();
				MyMatrix.r[3] = m_pTransformCom->Get_WorldMatrix().r[3];
				/*
				MyMatrix.r[3] += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fTimeDelta;
				*/
				_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.1f, 0.f, 0.f), fAngle);
				MyMatrix = RotationMatrix * MyMatrix;
				/*_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
				WorldMatrix.r[eState] = vState;
				XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);*/
				/*m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), RotationMatrix));
				m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_UP), RotationMatrix));
				m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_LOOK), RotationMatrix));*/

				CEquipment::EQUIPDESC EquipDesc;
				//Test_Flask
				EquipDesc.pSocket = nullptr;
				EquipDesc.szName = TEXT("BossBullet");
				EquipDesc.pParentWorldMatrix = nullptr;
				EquipDesc.fOwnerMatrix = m_pTransformCom->Get_World4x4();
				EquipDesc.bIsDepartment = true;
				XMStoreFloat4x4(&EquipDesc.SocketPivotMatrix, MyMatrix);
				EquipDesc.fPivotAngle = fAngle;
				if (m_eCurrentPhase == PHASE_TYPE::PHASE_1)
				{
					EquipDesc.iTypeNum = 0;
				}
				else
				{
					EquipDesc.iTypeNum = 1;
				}
				if (FAILED(m_pEquipments->Add_Department_Part(PARTS_WEAPON, (_uint)WEAPON_TYPE::THUNDER_SHOCK, &EquipDesc)))
					return false;


			}

			if (m_eCurrentPhase == PHASE_TYPE::PHASE_1)
			{
				m_fBulletDelayAcc = 3.f;
			}
			else
			{
				m_fBulletDelayAcc = 1.5f;
			}
		}

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


		RELEASE_INSTANCE(CGameInstance);


	}
	return !m_bAttack;
}

_bool CMafiaBoss::SpinAttack_Move(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_eCurrentAnimState != ANIM_STATE::SPIN_ATTACK)
	{
		//printf_s("spin_mov\n");
		m_eCurrentAnimState = ANIM_STATE::SPIN_ATTACK;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_bAttack = true;
		m_iBounceCnt = 0;
		m_pModelCom->Set_OffsetPerSecond(m_eCurrentAnimState, 2.5f);

		CHatKid* pTarget = dynamic_cast<CHatKid*>(pGameInstance->Get_FirstObject(LEVEL_STATIC, TEXT("Layer_Player")));

		if (pTarget == nullptr)
			return false;

		CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
		if (nullptr == pTargetCollider)
			return false;



		_vector vLook = pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_vector vDir = pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vDir = XMVectorSetY(vDir, 0.f);
		m_pTransformCom->LookAt_Dir(vDir);

		Create_RotationEffect();
		m_fSoundTimer = 0.f;
	}

	m_fSpinTime -= fTimeDelta;

	if (true == pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_PLAYER, this, m_pSPHERECom, 1.f))
	{//Damage

	}
	m_fSoundTimer -= fTimeDelta;
	if (m_fSoundTimer <= 0.f)
	{
		m_fSoundTimer = 4.f;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("Spin_Spinning_Loop_SFX.ogg"), SOUND_ENEMYSKILL, fVolume);
		RELEASE_INSTANCE(CGameInstance);
	}

	if (!m_bCanGo)
	{
		++m_iBounceCnt;
		if (m_iBounceCnt >= 2)
		{
			//printf_s("%d \n", m_iBounceCnt);
			_vector vLook = XMLoadFloat3(&m_LedgePoints[1]) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			vLook = XMVectorSetY(vLook, 0.f);
			vLook = XMVectorSetW(vLook, 0.f);
			m_pTransformCom->LookAt_Dir(vLook);

			m_fSpinTime = 0.5f;
			m_bCanGo = true;
		}
		else
		{
			//printf_s("%d \n", m_iBounceCnt);
			CHatKid* pTarget = dynamic_cast<CHatKid*>(pGameInstance->Get_FirstObject(LEVEL_STATIC, TEXT("Layer_Player")));

			if (pTarget == nullptr)
				return false;

			_vector vDir = pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			vDir = XMVectorSetY(vDir, 0.f);
			m_pTransformCom->LookAt_Dir(vDir);
			m_bCanGo = true;
		}
	}
	else
	{
		if (m_iBounceCnt >= 2)
		{
			if (m_fSpinTime < 0.f)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->StopSound(SOUND_ENEMYSKILL);
				RELEASE_INSTANCE(CGameInstance);

				m_iBounceCnt = 0;
				m_bAttack = false;
				m_fSpinTime = 3.f;
			}
		}
		if (m_eCurrentPhase == PHASE_TYPE::PHASE_1)
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 2.5f, m_pNavigationCom, 0, &m_bCanGo);
		}
		else
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 4.f, m_pNavigationCom, 0, &m_bCanGo);
		}
		
	}

	/*_float fDistance = 0.f;
	XMStoreFloat(&fDistance, (XMVector3Length(pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION))));*/


	RELEASE_INSTANCE(CGameInstance);
	return !m_bAttack;
}

_bool CMafiaBoss::Whistle(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::WHISTLE)
	{
		//printf_s("whistle \n");
		m_eCurrentAnimState = ANIM_STATE::WHISTLE;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		//Test
		m_fCreateTime = 1.f;
		m_iRandNum = rand() % 4;
		if (m_iRandNum < 1)
		{
			m_iRandNum += 2;
		}
		m_iReversNum = 5 - m_iRandNum;
		m_bisRawLine = (_bool)(rand() % 2);

		if (m_eCurrentPhase != PHASE_TYPE::PHASE_1)
		{
			CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
			pDeligate->Notify_EliteMafia_Open(1);
			RELEASE_INSTANCE(CDeligate_Monster);
		}
		/*if (m_iRandNum % 2 == 0)
		m_bisRawLine = true;
		else
		m_bisRawLine = false;*/
		//Log
		printf_s("RanNum: %d, ReversNum: %d \n", m_iRandNum, m_iReversNum);

		//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 2.f);
		_vector vDir = XMLoadFloat3(&m_LedgePoints[1]) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vDir = XMVectorSetY(vDir, 0.f);
		vDir = XMVectorSetW(vDir, 0.f);
		_float3 fDir;
		XMStoreFloat3(&fDir, vDir);
		fDir.x = -fDir.x;
		vDir = XMLoadFloat3(&fDir);

		m_pTransformCom->LookAt_Dir(vDir);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("MafiaBoss_whistle.ogg"), SOUND_ENEMY, fVolume);
		RELEASE_INSTANCE(CGameInstance);
	}

	m_fCreateTime -= fTimeDelta;
	if (m_fCreateTime <= 0.f)
	{
		m_fCreateTime = 5.f;
		Create_ThrowingObject(fTimeDelta);
	}

	return m_bAnimFinished;
}

_bool CMafiaBoss::High_Jump(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::HIGHJUMP)
	{
		//printf_s("jump\n");
		m_eCurrentAnimState = ANIM_STATE::HIGHJUMP;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_bJump = true;
		m_fJumpPower = 0.f;
		//m_fLandPower = -1.f * m_fJumpPower;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CHatKid* pTarget = dynamic_cast<CHatKid*>(pGameInstance->Get_FirstObject(LEVEL_STATIC, TEXT("Layer_Player")));

		if (pTarget == nullptr)
			return false;

		RELEASE_INSTANCE(CGameInstance);

		_vector vLook = pTarget->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		m_pTransformCom->LookAt(pTarget->Get_State(CTransform::STATE_TRANSLATION));
		XMStoreFloat3(&m_vTargetPos, vLook);

		XMStoreFloat3(&m_vMyOriginPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		_vector vInitUp = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		XMStoreFloat3(&m_vPreUp, vInitUp);

		m_fAddTime = 0.f;

		pGameInstance = GET_INSTANCE(CGameInstance);
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("MafiaBoss_GroundSlam01.ogg"), SOUND_ENEMY, fVolume);
		RELEASE_INSTANCE(CGameInstance);
	}



	if (m_bJump)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float3 fPos;
		XMStoreFloat3(&fPos, vPos);

		//m_fJumpPower -= 0.05f;
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 fMyPos;
		XMStoreFloat4(&fMyPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		m_fAddTime += fTimeDelta;
		if (fPos.y < m_pNavigationCom->Get_NavigationHeight(fPos))
			//if(m_fAddTime >2.f)
		{
			m_fAddTime = 0.f;
			m_fJumpPower = 2.f;
			m_bJump = false;
			fMyPos.y = 0.f;
			vMyPos = XMLoadFloat4(&fMyPos);

			Set_Height();
			Create_LedgeEffect();
			/*_vector vTemp = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4 fTemp;
			XMStoreFloat4(&fTemp, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);*/
		}
		else
		{
			/*
			theta = 60 degree sin60 = fsqrt(3.f) * 0.5f, cos60 = 0.5f;
			persecspeed = 2.f;
			Position += Normalize(vlook) * speed_persec * fTimeDelta
			Position += -(0.5f * g * ftimeDelta * fTimeDelta) + (normalize(vUp) * fTimeDelta * SpeedperSec)
			Position += Normalize(vRight) * Speed_PerSec * fTimeDelta

			new y = v0 * sin@ * t - 0.5f * 9.8f *time * time + originPos;
			*/
			//방법 1. 라업룩으로 올리고 내리기
			_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			_vector vTargetPos = XMLoadFloat3(&m_vTargetPos);
			/*vMyPos += XMVector3Normalize(vLook) * 2.f * fTimeDelta;
			vMyPos += XMVector3Normalize(vRight) * 2.f * fTimeDelta;*/
			vMyPos += XMVector3Normalize(vLook) * XMVector3Length(vTargetPos) * fTimeDelta / 2.f;
			//vMyPos += -(XMVector3Normalize(vUp) /** 0.3f*/ * 9.8f * fTimeDelta * fTimeDelta) + (XMVector3Normalize(vUp)* 1.f *fTimeDelta);		
			//vMyPos += -(XMVector3Normalize(vUp) * 0.5f * 9.8f * fTimeDelta * fTimeDelta) + (XMVector3Normalize(vUp)* 2.f *fTimeDelta);
			_vector vPreUp = XMLoadFloat3(&m_vPreUp);
			vMyPos += (-(XMVector3Normalize(vUp) * 0.45f * 9.8f * m_fAddTime * m_fAddTime) + (0.45f * XMVector3Normalize(vUp)* 9.8f *m_fAddTime)) * 0.5f;

			XMStoreFloat3(&m_vPreUp, -(XMVector3Normalize(vUp) * 0.5f * 9.8f * m_fAddTime * m_fAddTime) + (XMVector3Normalize(vUp)* 2.f *m_fAddTime));

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);

			vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			XMStoreFloat3(&fPos, vPos);

			m_pNavigationCom->Compute_CurrentCell(fPos);
			m_pNavigationCom->isMove(vPos);

			//m_pTransformCom->Jump(fTimeDelta, m_fJumpPower);

		}

	}

	return !m_bJump;
}

_bool CMafiaBoss::Dizzy(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::DIZZY)
	{
		//printf_s("dizzy\n");
		m_eCurrentAnimState = ANIM_STATE::DIZZY;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_bCanDmged = true;
		m_fDizzyTime = 3.f;
		Create_DizzyEffect();

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("MafiaBoss_Dizzy02.ogg"), SOUND_ENEMY, fVolume);
		pGameInstance->PlaySounds(TEXT("Stunned_SFX.ogg"), SOUND_ENEMYSKILL, fVolume);
		RELEASE_INSTANCE(CGameInstance);
	}
	m_fDizzyTime -= fTimeDelta;
	if (m_fDizzyTime <= 0.f)
	{
		m_bCanDmged = false;
		CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
		m_fDizzyTime = 3.f;
		pDeligate->Notify_DizzyEffect();
		m_bAnimFinished = true;
		RELEASE_INSTANCE(CDeligate_Monster);
	}

	return m_bAnimFinished;
}

_bool CMafiaBoss::BackFlip(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::BACKFLIP)
	{
		// 버그수정바람 아주 개판이야 그냥 사과가 땅에 박힙니다
		printf_s("backflip \n");
		m_eCurrentAnimState = ANIM_STATE::BACKFLIP;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		m_bJump = true;

		m_fAddTime = 0.f;
		_vector vLook = XMLoadFloat3(&m_LedgePoints[0]) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		XMStoreFloat3(&m_vTargetPos, vLook);
		XMStoreFloat3(&m_vGoalLook, XMVector3Normalize(XMLoadFloat3(&m_LedgePoints[0]) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)));
		if (m_bHurt)
		{
			m_fJumpPower = 2.f;
			m_fLandPower = -1.f * m_fJumpPower;
		}
		else
		{
			if (m_eAttackState == ATTACK_STATE::THROW)
			{
				m_fJumpPower = 5.f;
				m_fLandPower = -1.f * m_fJumpPower;
			}
			else
			{
				m_fJumpPower = 2.f;
				m_fLandPower = -1.f * m_fJumpPower;
			}
		}
	}

	if (m_bHurt&&m_bJump)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float3 fPos;
		XMStoreFloat3(&fPos, vPos);
		m_pNavigationCom->isMove(vPos);
		m_fJumpPower -= 0.05f;

		if (m_fJumpPower < -.5f)
		{
			if (m_bDead)
			{
				m_bEffectStart = true;
				m_pTransformCom->Go_Up(1.f);
				//Set_DeleteObject();
				Create_SphereDead();

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				_float fVolume = 0.7f;
				pGameInstance->StopSound(0);
				pGameInstance->PlaySounds(TEXT("MafiaBoss_Death01.ogg"), SOUND_ENEMY, fVolume);
				pGameInstance->PlaySounds(TEXT("boss_post_explosion.ogg"), SOUND_EFFECT, fVolume);
				RELEASE_INSTANCE(CGameInstance);
			}
		}

		if (m_fJumpPower < 1.f &&fPos.y < m_pNavigationCom->Get_NavigationHeight(fPos))
		{
			if (m_bHurt && !m_bPhaseChanged)
			{
				m_bHurt = false;
			}
			m_fJumpPower = 2.f;
			m_bJump = false;
			Set_Height();
			

		}
		else
		{
			m_pTransformCom->Jump(fTimeDelta, m_fJumpPower, m_pNavigationCom);
			m_pTransformCom->Go_Backward(fTimeDelta * 0.3f, m_pNavigationCom);
		}

	}

	if (m_bJump && !m_bHurt)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float3 fPos;
		XMStoreFloat3(&fPos, vPos);

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 fMyPos;
		XMStoreFloat4(&fMyPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		m_fAddTime += fTimeDelta;
		if (fPos.y < m_pNavigationCom->Get_NavigationHeight(fPos))
		{
			m_fAddTime = 0.f;
			m_fJumpPower = 2.f;
			m_bJump = false;
			fMyPos.y = 0.f;
			vMyPos = XMLoadFloat4(&fMyPos);

			Set_Height();
		}
		else
		{
			/*
			theta = 60 degree sin60 = fsqrt(3.f) * 0.5f, cos60 = 0.5f;
			persecspeed = 2.f;
			Position += Normalize(vlook) * speed_persec * fTimeDelta
			Position += -(0.5f * g * ftimeDelta * fTimeDelta) + (normalize(vUp) * fTimeDelta * SpeedperSec)
			Position += Normalize(vRight) * Speed_PerSec * fTimeDelta
			new y = v0 * sin@ * t - 0.5f * 9.8f *time * time + originPos;
			*/
			//방법 1. 라업룩으로 올리고 내리기
			_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
			_vector vLook = XMLoadFloat3(&m_vGoalLook);
			_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			_vector vTargetPos = XMLoadFloat3(&m_vTargetPos);
			vMyPos += XMVector3Normalize(vLook) * XMVector3Length(vTargetPos) * fTimeDelta * 0.6f;
			vMyPos += (-(XMVector3Normalize(vUp) * 0.45f * 9.8f * m_fAddTime * m_fAddTime) + (0.5f * XMVector3Normalize(vUp)* 9.8f *m_fAddTime)) * 0.5f;

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);
			vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			XMStoreFloat3(&fPos, vPos);

			m_pNavigationCom->Compute_CurrentCell(fPos);
			m_pNavigationCom->isMove(vPos);

		}

	}

	return !m_bJump;
}

_bool CMafiaBoss::Bravodo(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::BRAVADO)
	{
		//printf_s("bravode\n");
		m_eCurrentAnimState = ANIM_STATE::BRAVADO;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;
		if (m_eCurrentPhase != PHASE_TYPE::PHASE_1)
		{
			m_bFightMode = false;
		}
		m_fDeadTimer = 0.f;
	}

	m_fDeadTimer += fTimeDelta;

	if (m_bFightMode == true && m_fDeadTimer > 2.f)
	{
		m_bAnimLoop = false;
		m_fDeadTimer = 0.f;
	}
	return !m_bAnimLoop;
}

_bool CMafiaBoss::Weapon_Reach(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::WEPON_REACH)
	{
		//printf_s("reach\n");
		m_eCurrentAnimState = ANIM_STATE::WEPON_REACH;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		m_pEquipments->Change_Part((_uint)PARTS_TYPE::PARTS_WEAPON, (_uint)WEAPON_TYPE::BLADE);
		m_pEquipments->Change_Part((_uint)PARTS_TYPE::PARTS_SUBWEAPON, (_uint)WEAPON_TYPE::BLADE);
		m_bCanDmged = true;
	}
	if (m_bAnimFinished)
	{
		m_bCanDmged = false;
	}


	return m_bAnimFinished;
}

_bool CMafiaBoss::Angry(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::ANGRY)
	{
		//printf_s("angry\n");
		m_eCurrentAnimState = ANIM_STATE::ANGRY;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("MafiaBoss_Angry03.ogg"), SOUND_ENEMY, fVolume);
		RELEASE_INSTANCE(CGameInstance);
	}

	if (m_bAnimFinished == true)
	{
		m_bHurt = false;
		m_bPhaseChanged = false;

		switch (m_eCurrentPhase)
		{
		case PHASE_TYPE::PHASE_1:
			m_eCurrentPhase = PHASE_TYPE::PHASE_2;
			printf_s("PhaseNum: %d \n", (_uint)m_eCurrentPhase);
			break;
		case PHASE_TYPE::PHASE_2:
			m_eCurrentPhase = PHASE_TYPE::PHASE_3;
			printf_s("PhaseNum: %d \n", (_uint)m_eCurrentPhase);
			break;
		case PHASE_TYPE::PHASE_3:
			m_eCurrentPhase = PHASE_TYPE::PHASE_1;
			printf_s("PhaseNum: %d \n", (_uint)m_eCurrentPhase);
			break;
		}
	}

	return m_bAnimFinished;
}

_bool CMafiaBoss::Chair_Jump(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::CHAIR_JUMP)
	{
		m_eCurrentAnimState = ANIM_STATE::CHAIR_JUMP;
		m_bAnimLoop = false;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->StopSound(0);
		_float fVolume = 0.6f;
		pGameInstance->PlaySounds(TEXT("IntruderAlert1.ogg"), 0, fVolume);
		RELEASE_INSTANCE(CGameInstance);

		CCamera_Manager* pMgr = GET_INSTANCE(CCamera_Manager);

		pMgr->Set_CameraState(1);

		RELEASE_INSTANCE(CCamera_Manager);

	}

	if (m_bAnimFinished)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->StopSound(0);
		_float fVolume = 0.6f;
		pGameInstance->PlayBGM(TEXT("MafiaBoss_Orchestral_Remix_Loop.ogg"), fVolume);
		RELEASE_INSTANCE(CGameInstance);


		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&m_LedgePoints[2]));
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float3 fPos;

		_vector vDir = XMLoadFloat3(&m_LedgePoints[0]) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vDir = XMVectorSetY(vDir, 0.f);
		vDir = XMVectorSetW(vDir, 0.f);
		m_pTransformCom->LookAt_Dir(vDir);

		XMStoreFloat3(&fPos, vPos);

		m_pNavigationCom->Compute_CurrentCell(fPos);
		m_pNavigationCom->isMove(vPos);

		Set_Height();



	}

	return m_bAnimFinished;
}

_bool CMafiaBoss::Chair_Idle(_float fTimeDelta)
{
	if (m_eCurrentAnimState != ANIM_STATE::SIT_IDLE)
	{
		m_eCurrentAnimState = ANIM_STATE::SIT_IDLE;
		m_bAnimLoop = true;
		m_pModelCom->Set_NextAnimIndex(m_eCurrentAnimState, m_bAnimLoop);
		m_bAnimFinished = false;

		
	}

	return m_bFightMode;
}

void CMafiaBoss::Intro(_float fTimeDelta)
{
	if (!m_bFightMode)
	{
		Chair_Idle(fTimeDelta);
	}
	else
	{
		m_bIntroEnd = Chair_Jump(fTimeDelta);
		if (m_bIntroEnd)
		{
			Idle(fTimeDelta);
			m_pModelCom->Play_Animation(0.2f, &m_bAnimFinished);

			CCamera_Manager* pMgr = GET_INSTANCE(CCamera_Manager);

			pMgr->Set_CameraState(0);

			RELEASE_INSTANCE(CCamera_Manager);
		
		}

	}

}

_bool CMafiaBoss::Select_Pattern(void)
{
	while (m_eCurrentPattern == m_ePrePattern)
	{
		m_eCurrentPattern = (PATTERN_TYPE)(rand() % (_uint)PATTERN_TYPE::PATTERN_END);
	}
	m_ePrePattern = m_eCurrentPattern;


	//m_eCurrentPattern = PATTERN_TYPE::PATTERN_1;

	switch (m_eCurrentPhase)
	{
	case PHASE_TYPE::PHASE_1:
		switch (m_eCurrentPattern)
		{
		case PATTERN_TYPE::PATTERN_1:
			m_eAttackState = ATTACK_STATE::SLASH;
			break;
		case PATTERN_TYPE::PATTERN_2:
			m_eAttackState = ATTACK_STATE::SPIN_MOVE;
			break;
		case PATTERN_TYPE::PATTERN_3:
			m_eAttackState = ATTACK_STATE::SPIN_IDLE;
			break;
		case PATTERN_TYPE::PATTERN_4:
			m_eAttackState = ATTACK_STATE::WHISTLE;
			break;
		}
		break;
	case PHASE_TYPE::PHASE_2:
		switch (m_eCurrentPattern)
		{
		case PATTERN_TYPE::PATTERN_1:
			m_eAttackState = ATTACK_STATE::THROW;
			break;
		case PATTERN_TYPE::PATTERN_2:
			m_eAttackState = ATTACK_STATE::SPIN_MOVE;
			break;
		case PATTERN_TYPE::PATTERN_3:
			m_eAttackState = ATTACK_STATE::SPIN_IDLE;
			break;
		case PATTERN_TYPE::PATTERN_4:
			m_eAttackState = ATTACK_STATE::WHISTLE;
			break;
		}
		break;
	case PHASE_TYPE::PHASE_3:
		switch (m_eCurrentPattern)
		{
		case PATTERN_TYPE::PATTERN_1:
			m_eAttackState = ATTACK_STATE::THROW;
			break;
		case PATTERN_TYPE::PATTERN_2:
			m_eAttackState = ATTACK_STATE::SPIN_MOVE;
			break;
		case PATTERN_TYPE::PATTERN_3:
			m_eAttackState = ATTACK_STATE::SPIN_IDLE;
			break;
		case PATTERN_TYPE::PATTERN_4:
			m_eAttackState = ATTACK_STATE::WHISTLE;
			break;
		}
		break;
	}

	return S_OK;
}

void CMafiaBoss::Play_Sound()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_tchar	szFullPath[MAX_PATH];// = TEXT("footstep_grass_%d.wav");
	_uint iNum = 0;
	_float fVolume = 0.7f;
	fVolume = 0.5f;
	iNum = rand() % 3;
	wcscpy_s(szFullPath, TEXT("AttackOverlay%d.ogg"));
	wsprintf(szFullPath, szFullPath, iNum);
	pGameInstance->PlaySounds(szFullPath, SOUND_EFFECT, fVolume);
	RELEASE_INSTANCE(CGameInstance);

}

HRESULT CMafiaBoss::Init_Position()
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Start_Pos/BossLedgePoint.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints;
	while (true)
	{
		ReadFile(hFile, &vPoints, sizeof(_float3), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		m_LedgePoints.push_back(vPoints);
	}

	CloseHandle(hFile);

	_vector vPoint = XMLoadFloat3(&vPoints);
	vPoint = XMVectorSet(m_LedgePoints[0].x, m_LedgePoints[0].y, m_LedgePoints[0].z, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPoint);

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;
	NaviDesc.fCurrent_Position = m_LedgePoints[0];
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	//Sandback 포지션담기

	dwByte = 0;
	hFile = CreateFile(TEXT("../Bin/Data/Start_Pos/SandBackPoint.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	vPoints;
	while (true)
	{
		ReadFile(hFile, &vPoints, sizeof(_float3), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		m_SandbackPoints.push_back(vPoints);
	}

	CloseHandle(hFile);

	return S_OK;
}

void CMafiaBoss::Set_Height()
{
	if (m_bJump)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_pNavigationCom->isMove(vPos);
		return;
	}


	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	fPos.y = m_pNavigationCom->Get_NavigationHeight(fPos);
	vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	m_pNavigationCom->isMove(vPos);
}

void CMafiaBoss::Check_FightMode()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr == pGameInstance)
		return;

	CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
	if (nullptr == pTargetCollider)
		return;

	RELEASE_INSTANCE(CGameInstance);

	/*if (m_eCurrentAnimState == ANIM_STATE::DEAD)
	return;*/

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

_bool CMafiaBoss::IsDetect(_uint eSphere_Scale, CCollider* pTargetCollider)
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

void CMafiaBoss::IsShoot_Objects(_bool bShoot)
{

}

HRESULT CMafiaBoss::Create_ThrowingObject(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (m_bisRawLine)
	{
		if (m_iRandNum > m_iReversNum)
		{
			for (_uint i = m_iReversNum; i < 5; ++i)
			{
				for (_uint j = 0; j <= 4; ++j)
				{
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sandback"), LEVEL_GAMEPLAY, TEXT("Enemy_Object"), &m_SandbackPoints[i * 5 + j])))
						return E_FAIL;
				}

			}
		}
		else
		{
			for (_uint i = 0; i <= m_iReversNum; ++i)
			{
				for (_uint j = 0; j < 5; ++j)
				{
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sandback"), LEVEL_GAMEPLAY, TEXT("Enemy_Object"), &m_SandbackPoints[i * 5 + j])))
						return E_FAIL;
				}

			}
		}
	}
	else
	{
		if (m_iRandNum > m_iReversNum)
		{
			for (_uint i = m_iReversNum; i < 5; ++i)
			{
				for (_uint j = 0; j < 9; ++j)
				{
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sandback"), LEVEL_GAMEPLAY, TEXT("Enemy_Object"), &m_SandbackPoints[j * 5 + i])))
						return E_FAIL;
				}

			}
		}
		else
		{
			for (_uint i = 0; i < 5; ++i)
			{
				for (_uint j = 0; j <= m_iReversNum; ++j)
				{
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sandback"), LEVEL_GAMEPLAY, TEXT("Enemy_Object"), &m_SandbackPoints[j * 10 + i])))
						return E_FAIL;
				}

			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMafiaBoss::Create_DizzyEffect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("Boss01-HeadNub");
	if (nullptr == m_pStuckSocket)
		return;
	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **//* m_pStuckSocket->Get_OffsetMatrix() **/
		m_pStuckSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 m_CombinedWorldMatrix;
	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

	CEffect::EFFECTINFO EffectInfo;
	EffectInfo.iTypeNum = 0;
	EffectInfo.fLifeTime = 2.f;
	EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
	//EffectInfo.vPosition.z += 5.f;
	EffectInfo.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EffectInfo.pSocket = m_pStuckSocket;
	Safe_AddRef(m_pStuckSocket);
	for (int i = 0; i < 5; ++i)
	{
		EffectInfo.vRotation = _float3(i* 72.f, 0.f, 0.f);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DizzyEffect"), LEVEL_GAMEPLAY, TEXT("Layer_Effect2"), &EffectInfo)))
			return;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CMafiaBoss::Create_LedgeEffect()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CEffect::EFFECTINFO EffectInfo;
	EffectInfo.iTypeNum = 0;
	EffectInfo.iMoveType = 0;
	EffectInfo.fAngle = 0.f;
	EffectInfo.fLifeTime = 0.f;
	XMStoreFloat3(&EffectInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CrownEffect"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
		return;
	EffectInfo.iTypeNum = 1;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CrownEffect"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
		return;


	Safe_Release(pGameInstance);
}

void CMafiaBoss::Create_RotationEffect()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CEffect::EFFECTINFO EffectInfo;
	EffectInfo.iTypeNum = 0;
	EffectInfo.iMoveType = 0;
	EffectInfo.fAngle = 0.f;
	EffectInfo.fLifeTime = 0.f;
	EffectInfo.pSocket = m_pModelCom->Get_BonePtr("Boss01-Spine");
	EffectInfo.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EffectInfo.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	EffectInfo.bOwnerEnd = &m_bAttack;
	XMStoreFloat3(&EffectInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_RotationEffect"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
		return;



	Safe_Release(pGameInstance);

}

void CMafiaBoss::Create_Puff()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("Boss01-Spine");
	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ /*m_pStuckSocket->Get_OffsetMatrix() **/
		m_pStuckSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 m_CombinedWorldMatrix;
	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

	for (_uint i = 0; i < 5; ++i)
	{
		_int Num = rand() % 2;
		_int iOffsetX = 1, iOffsetY = 1, iOffsetZ = 1;
		if (Num == 0)
			iOffsetX *= -1;
		Num = rand() % 2;
		if (Num == 1)
			iOffsetY *= -1;
		Num = rand() % 2;
		if (Num == 0)
			iOffsetZ *= -1;

		CEffect::EFFECTINFO EffectInfo;
		EffectInfo.iTypeNum = 4;
		XMStoreFloat3(&EffectInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		EffectInfo.vPosition.x += iOffsetX * rand() % 7;
		EffectInfo.vPosition.y += iOffsetY * rand() % 4;
		EffectInfo.vPosition.z += iOffsetZ * rand() % 7;
		EffectInfo.iMoveType = 0;
		EffectInfo.fAngle = 0.f;
		EffectInfo.fLifeTime = 2.f;


		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
			return;
	}

	Safe_Release(pGameInstance);
}

void CMafiaBoss::Create_SphereDead()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("Boss01-Spine");
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


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DeadSphere"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &EffectInfo)))
		return;


	Safe_Release(pGameInstance);
}

void CMafiaBoss::Ready_Detect(_float3 fColliderPos)
{
	CDetect::DETECTDESC DetectDesc;
	CDetect::COLLIDERDESC Col_MaxDesc, Col_MinDesc;

	ZeroMemory(&DetectDesc, sizeof(CDetect::DETECTDESC));
	ZeroMemory(&Col_MaxDesc, sizeof(CDetect::COLLIDERDESC));
	ZeroMemory(&Col_MinDesc, sizeof(CDetect::COLLIDERDESC));
	Col_MaxDesc.vPosition = fColliderPos;
	Col_MaxDesc.vRotation = _float3(0.f, 0.f, 0.f);
	Col_MaxDesc.vScale = _float3(10.f, 10.f, 10.f);

	Col_MinDesc.vPosition = fColliderPos;
	Col_MinDesc.vRotation = _float3(0.f, 0.f, 0.f);
	Col_MinDesc.vScale = _float3(20.f, 20.f, 20.f);

	memcpy(&DetectDesc.MaxDetect_Collider, &Col_MaxDesc, sizeof(CDetect::COLLIDERDESC));
	memcpy(&DetectDesc.MinDetect_Collider, &Col_MinDesc, sizeof(CDetect::COLLIDERDESC));

	m_pDetect = new CDetect();
	m_pDetect->Initialize(&DetectDesc);
}

HRESULT CMafiaBoss::Ready_Parts()
{
	m_pEquipments = new CEquipments;
	if (FAILED(m_pEquipments->Initialize(PARTS_END)))
		return E_FAIL;

	//0번째는 장비착용 X
	if (FAILED(m_pEquipments->Add_Parts(PARTS_TYPE::PARTS_WEAPON, (_uint)WEAPON_TYPE::TYPE_NONE)))
		return E_FAIL;
	//0번째는 장비착용 X
	if (FAILED(m_pEquipments->Add_Parts(PARTS_TYPE::PARTS_SUBWEAPON, (_uint)WEAPON_TYPE::TYPE_NONE)))
		return E_FAIL;

	CEquipment::EQUIPDESC EquipDesc;
	CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("Boss01-R-Hand");
	//Test_RightHand_Weapon
	_matrix RotationXMatrix = XMMatrixRotationX(XMConvertToRadians(90.0f));
	_matrix RotationYMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	_matrix RotationZMatrix = XMMatrixRotationZ(XMConvertToRadians(180.0f));
	_matrix RotationMatrix = RotationXMatrix * RotationYMatrix * RotationZMatrix * XMMatrixTranslation(2.f, 1.f, 0.f);
	_matrix PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) *RotationMatrix;

	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Blade");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	XMStoreFloat4x4(&EquipDesc.EquipPivotMatrix, PivotMatrix);
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts((_uint)PARTS_TYPE::PARTS_WEAPON, (_uint)WEAPON_TYPE::BLADE, &EquipDesc)))
		return E_FAIL;

	//Test_LeftHand_Weapon
	pSocket = m_pModelCom->Get_BonePtr("Boss01-L-Hand");
	if (nullptr == pSocket)
		return E_FAIL;

	RotationXMatrix = XMMatrixRotationX(XMConvertToRadians(270.0f));
	RotationYMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	RotationZMatrix = XMMatrixRotationZ(XMConvertToRadians(180.0f));
	RotationMatrix = RotationXMatrix * RotationYMatrix * RotationZMatrix * XMMatrixTranslation(2.f, 1.f, 0.f);
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) *RotationMatrix;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Blade");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	XMStoreFloat4x4(&EquipDesc.EquipPivotMatrix, PivotMatrix);
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts((_uint)PARTS_TYPE::PARTS_SUBWEAPON, (_uint)WEAPON_TYPE::BLADE, &EquipDesc)))
		return E_FAIL;

	////Diveing Head
	//pSocket = m_pModelCom->Get_BonePtr("Boss01-Head");
	//if (nullptr == pSocket)
	//	return E_FAIL;

	//EquipDesc.pSocket = pSocket;
	//EquipDesc.szName = TEXT("Punch");
	//EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	//EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	//Safe_AddRef(pSocket);

	//if (FAILED(m_pEquipments->Add_Parts((_uint)WEAPON_TYPE::TYPE_NONE, (_uint)WEAPON_TYPE::HEAD, &EquipDesc)))
	//	return E_FAIL;


	m_pEquipments->Change_Part((_uint)PARTS_TYPE::PARTS_WEAPON, (_uint)WEAPON_TYPE::BLADE);
	m_pEquipments->Change_Part((_uint)PARTS_TYPE::PARTS_SUBWEAPON, (_uint)WEAPON_TYPE::BLADE);

	m_eWeaponType = WEAPON_TYPE::BLADE;

	return S_OK;
}

HRESULT CMafiaBoss::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MafiaBoss"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_SPHERE */
	m_pStuckSocket = m_pModelCom->Get_BonePtr("Boss01-Spine");
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

HRESULT CMafiaBoss::SetUp_ShaderResources()
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

CMafiaBoss * CMafiaBoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMafiaBoss*	pInstance = new CMafiaBoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMafiaBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMafiaBoss::Clone(void * pArg)
{
	CMafiaBoss*	pInstance = new CMafiaBoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMafiaBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMafiaBoss::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBehaviorTree);
	Safe_Release(m_pStuckSocket);
	//이상하게 왜 릭이 안났지??????????
	Safe_Release(m_pEquipments);
	////////////////////
	Safe_Release(m_pDetect);
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}

