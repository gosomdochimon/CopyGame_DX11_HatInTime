#include "stdafx.h"
#include "..\Public\HatKid.h"
#include "GameInstance.h"
#include "PlayerController.h"
#include "Equipments.h"
#include "HierarchyNode.h"
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

	if (FAILED(Setup_Anim_Loop()))
		return E_FAIL;

	//m_pModelCom->Set_CurrentAnimIndex(1);
	m_pModelCom->Set_CurrentAnimIndex_Upper(IDLE);
	m_pModelCom->Set_CurrentAnimIndex_Lower(IDLE);

	m_UpperStates.push(IDLE);
	m_LowerStates.push(IDLE);
	m_eDirState = DIR_FORWARD;

	if(FAILED(Ready_Parts()))
		return E_FAIL;

	return S_OK;
}

void CHatKid::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Pop_Animation();

	Change_ActionBoolState_Anim(fTimeDelta);

	m_pController->Input_Controller(fTimeDelta);

	TestFunc(fTimeDelta);

	Play_Animation(fTimeDelta);

	if(m_pEquipments != nullptr)
		m_pEquipments->Tick(fTimeDelta);

	m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CHatKid::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_pEquipments != nullptr)
		m_pEquipments->Late_Tick(fTimeDelta);

	

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

_vector CHatKid::Get_State(_uint iState) const
{
	{ return m_pTransformCom->Get_State((CTransform::STATE)iState); }
}

HRESULT CHatKid::Setup_Anim_Loop(void)
{/*For AnimState Loop Setting*/
	m_AnimLoopStates.reserve(ANIM_STATE::STATE_END);
	m_AnimLoopStates.resize(ANIM_STATE::STATE_END);

	m_AnimLoopStates[ANIM_STATE::IDLE]			= true;
	m_AnimLoopStates[ANIM_STATE::IDLE_TAUNT]	= true;
	m_AnimLoopStates[ANIM_STATE::RUN]			= true;
	m_AnimLoopStates[ANIM_STATE::RUN_OLD]		= true;
	m_AnimLoopStates[ANIM_STATE::SPRINT]		= true;
	m_AnimLoopStates[ANIM_STATE::JUMP_FORWARD]	= false;
	m_AnimLoopStates[ANIM_STATE::JUMP_DOUBLE]	= false;
	m_AnimLoopStates[ANIM_STATE::JUMP_LEDGE] = false;
	m_AnimLoopStates[ANIM_STATE::DIVE_IDLE] = true;
	m_AnimLoopStates[ANIM_STATE::DIVE_SLIDE] = false;
	m_AnimLoopStates[ANIM_STATE::SLIDE_FINISH] = false;
	m_AnimLoopStates[ANIM_STATE::CRARRY_UMBRELLA_INTRO] = true;
	m_AnimLoopStates[ANIM_STATE::CARRY_UMBRELLA_IDLE] = true;
	m_AnimLoopStates[ANIM_STATE::UMBRELLA_ATK_A] = false;
	m_AnimLoopStates[ANIM_STATE::UMBRELLA_ATK_B] = false;
	m_AnimLoopStates[ANIM_STATE::UMBRELLA_ATK_C] = false;
	m_AnimLoopStates[ANIM_STATE::PUNCH_A] = false;
	m_AnimLoopStates[ANIM_STATE::PUNCH_B] = false;
	m_AnimLoopStates[ANIM_STATE::ITEM_PICKUP_LARGE] = false;
	m_AnimLoopStates[ANIM_STATE::ITEM_CARRY_LARGE] = true;
	m_AnimLoopStates[ANIM_STATE::ITEM_THROW_ONEHAND] = false;
	m_AnimLoopStates[ANIM_STATE::ITEM_THROW] = false;
	m_AnimLoopStates[ANIM_STATE::SHAKE_FLASK] = true;
	m_AnimLoopStates[ANIM_STATE::HOOKSHOT_START] = true;
	m_AnimLoopStates[ANIM_STATE::HOOK0SHOT_SWING] = true;
	m_AnimLoopStates[ANIM_STATE::HOOKSHOT_FINISH] = false;
	m_AnimLoopStates[ANIM_STATE::DEAD] = false;
	m_AnimLoopStates[ANIM_STATE::LELVEL_START] = false;
	m_AnimLoopStates[ANIM_STATE::VICTORY] = false;
	m_AnimLoopStates[ANIM_STATE::HURT] = false;

	return S_OK;
}

void CHatKid::Pop_Animation(void)
{
	//Upper
	if (m_bAnimFinished_Upper && !m_UpperStates.empty())
	{
		m_UpperStates.pop();
	}

	if (m_bAnimFinished_Lower && !m_LowerStates.empty())
	{
		m_LowerStates.pop();
	}
}

void CHatKid::Play_Animation(_float fTimeDelta)
{/*Play Anims all( Weapon etc... */
	if (!m_UpperStates.empty())
	{
		if(m_eAnimState != m_UpperStates.front())
			m_eAnimState = m_UpperStates.front();
		m_pModelCom->Set_NextAnimIndex_Upper(m_UpperStates.front(), m_AnimLoopStates[m_UpperStates.front()]);
	}
	
	if (!m_LowerStates.empty())
	{
		m_pModelCom->Set_NextAnimIndex_Lower(m_LowerStates.front(), m_AnimLoopStates[m_LowerStates.front()]);
	}
	m_pModelCom->Play_Animation_Seperate(fTimeDelta, &m_bAnimFinished_Upper, &m_bAnimFinished_Lower);
}

void CHatKid::Push_UpperState(UPPER_STATE eUpperState)
{
	/*각 함수 실행되는 부분으로 뺴는게 맞는 것 같다. 
	아니면이걸 각 함수내에서만 실행하는 것으로 하는 것은 어떨까? 그게 제일 효율적일 것 같다.
	*/
	switch (m_eCurrentLowerState)
	{
	case LOWER_STATE::IDLE:
		if (m_eCurrentUpperState != eUpperState)
		{	//UpperAnim Queue를 초기화 해주고 넣을 것.
			Reset_UpperAnim();
			//위에서 던져주는 상태에 따라 체크하기.
			switch (eUpperState)
			{
			case UPPER_STATE::IDLE:
				m_UpperStates.push(IDLE);
				break;		
			case UPPER_STATE::SKILL:
				//추후 모자 나눌 시 추가 예정.
				m_UpperStates.push(SHAKE_FLASK);
				break;
			case UPPER_STATE::CARRY:
				m_UpperStates.push(ITEM_PICKUP_LARGE);
				m_UpperStates.push(ITEM_CARRY_LARGE);
				break;
			case UPPER_STATE::UMBRELLA:
				m_UpperStates.push(CRARRY_UMBRELLA_INTRO);
				m_UpperStates.push(CARRY_UMBRELLA_IDLE);
				break;
			case UPPER_STATE::RUN:
			case UPPER_STATE::SPRINT:
				//m_UpperStates.push(IDLE);
			case UPPER_STATE::JUMP:
			case UPPER_STATE::DOUBLE_JUMP:
			case UPPER_STATE::ATTACK:
			case UPPER_STATE::THROW:
			case UPPER_STATE::DIVE:
			case UPPER_STATE::HOOK:
				break;
			}
			m_ePreUpperState = m_eCurrentUpperState;
			m_eCurrentUpperState = eUpperState;
		}
		break;
	case LOWER_STATE:: RUN:
		if (m_eCurrentUpperState != eUpperState)
		{
			Reset_UpperAnim();
			switch (eUpperState)
			{
			case UPPER_STATE::IDLE:
				break;
			case UPPER_STATE::RUN:
				m_UpperStates.push(RUN);
				break;
			case UPPER_STATE::SKILL:
				m_UpperStates.push(SHAKE_FLASK);
				break;
			case UPPER_STATE::CARRY:
				m_UpperStates.push(ITEM_CARRY_LARGE);
				break;
			case UPPER_STATE::UMBRELLA:
				m_UpperStates.push(CARRY_UMBRELLA_IDLE);
				break;
			case UPPER_STATE::JUMP:
			case UPPER_STATE::DOUBLE_JUMP:
			case UPPER_STATE::ATTACK:
			case UPPER_STATE::THROW:
			case UPPER_STATE::DIVE:
				m_UpperStates.push(DIVE_IDLE);
				break;
			case UPPER_STATE::HOOK:
				break;
			}
			m_ePreUpperState = m_eCurrentUpperState;
			m_eCurrentUpperState = eUpperState;
		}
		break;
	case LOWER_STATE::SPRINT:
		if (m_eCurrentUpperState != eUpperState)
		{
			Reset_UpperAnim();
			switch (eUpperState)
			{
			case UPPER_STATE::IDLE:
				break;
			case UPPER_STATE::RUN:
				m_UpperStates.push(RUN);
			case UPPER_STATE::SPRINT:
				m_UpperStates.push(SPRINT);
				break;
			case UPPER_STATE::DIVE:
				break;
			}
			m_ePreUpperState = m_eCurrentUpperState;
			m_eCurrentUpperState = eUpperState;
		}
		break;
	case LOWER_STATE::JUMP:
		if (m_eCurrentUpperState != eUpperState)
		{
			Reset_UpperAnim();
			switch (eUpperState)
			{
			case UPPER_STATE::IDLE:
				m_UpperStates.push(JUMP_FORWARD);
			case UPPER_STATE::RUN:
				break;
			case UPPER_STATE::SKILL:
				m_UpperStates.push(SHAKE_FLASK);
				break;
			case UPPER_STATE::CARRY:
				m_UpperStates.push(ITEM_CARRY_LARGE);
				break;
			case UPPER_STATE::JUMP:
				m_UpperStates.push(ANIM_STATE::JUMP_FORWARD);
				m_UpperStates.push(ANIM_STATE::JUMP_LEDGE);
				break;
			case UPPER_STATE::DOUBLE_JUMP:
				m_UpperStates.push(ANIM_STATE::JUMP_DOUBLE);
				m_UpperStates.push(ANIM_STATE::JUMP_LEDGE);
			case UPPER_STATE::UMBRELLA:
			case UPPER_STATE::ATTACK:
			case UPPER_STATE::THROW:
				break;
			case UPPER_STATE::DIVE:
				m_UpperStates.push(DIVE_IDLE);
			case UPPER_STATE::HOOK:
				break;
			}
			m_ePreUpperState = m_eCurrentUpperState;
			m_eCurrentUpperState = eUpperState;
		}
		break;
		
	case LOWER_STATE::DOUBLE_JUMP:
		if (m_eCurrentUpperState != eUpperState)
		{
			Reset_UpperAnim();
			m_UpperStates.push(ANIM_STATE::JUMP_DOUBLE);
			m_UpperStates.push(ANIM_STATE::JUMP_LEDGE);
			m_ePreUpperState = m_eCurrentUpperState;
			m_eCurrentUpperState = eUpperState;
		}
		break;

	case LOWER_STATE::ATTACK:
		if (m_eCurrentUpperState != eUpperState)
		{
			Reset_UpperAnim();
			m_ePreUpperState = m_eCurrentUpperState;
			m_eCurrentUpperState = eUpperState;
		}
		switch (m_iAtkCount)
		{
		case 1:
			if (m_eWeaponType == WEAPON_TYPE::WEAPON_NONE)
			{
				m_UpperStates.push(ANIM_STATE::PUNCH_A);
			}
			else if(m_eWeaponType == WEAPON_TYPE::WEAPON_UMBRELLA)
			{
				m_UpperStates.push(ANIM_STATE::UMBRELLA_ATK_A);
			}
			break;
		case 2:
			if (m_eWeaponType == WEAPON_TYPE::WEAPON_NONE)
			{
				m_UpperStates.push(ANIM_STATE::PUNCH_B);
			}
			else if (m_eWeaponType == WEAPON_TYPE::WEAPON_UMBRELLA)
			{
				m_UpperStates.push(ANIM_STATE::UMBRELLA_ATK_B);
			}
			break;
		case 3:
			m_UpperStates.push(ANIM_STATE::UMBRELLA_ATK_C);
			break;
		}
		break;
	case LOWER_STATE::THROW:
		if (m_bIsPickup)
		{
			Reset_UpperAnim();
			m_UpperStates.push(ANIM_STATE::ITEM_THROW);
		}
		else if (m_bSkillUsing)
		{
			Reset_UpperAnim();
			m_UpperStates.push(ANIM_STATE::ITEM_THROW_ONEHAND);
		}
		m_ePreUpperState = m_eCurrentUpperState;
		m_eCurrentUpperState = eUpperState;
		break;
	case LOWER_STATE::DIVE:
		Reset_UpperAnim();
		//이처리를 하는 이유 : Dive에서 슬라이딩으로 넘어가야함
		//UPPER_STATE::DIVE 
		if (m_eCurrentUpperState == eUpperState && !m_bJump)
		{//이미 현재 상태가 DIVE이고, 다이브중에 땅에 도착한 상황인가?
			m_UpperStates.push(DIVE_SLIDE);
			m_UpperStates.push(SLIDE_FINISH);
		}
		else if(m_eCurrentUpperState != eUpperState)
		{
			if (m_bJump)
			{ //점프중인상태였는가?
				m_UpperStates.push(DIVE_IDLE);
			}
			else
			{ //땅에서 슬라이딩하는 것인가?
				m_UpperStates.push(DIVE_SLIDE);
				m_UpperStates.push(SLIDE_FINISH);
			}
			m_ePreUpperState = m_eCurrentUpperState;
			m_eCurrentUpperState = eUpperState;

		}		
		break;
	case LOWER_STATE::HOOK:
		break;
	case LOWER_STATE::HURT:
		Reset_UpperAnim();
		m_ePreUpperState = m_eCurrentUpperState;
		m_eCurrentUpperState = eUpperState;
		m_UpperStates.push(HURT);
		break;
	case LOWER_STATE::DEAD:
		break;
	}


}

void CHatKid::Change_ActionBoolState_Anim(_float fTimeDelta)
{//애니메이션 종료 시 액션들에 대한 불변수 처리 함수.

	switch (m_eCurrentLowerState)
	{
	case LOWER_STATE::IDLE:
		if (m_eAnimState == ITEM_PICKUP_LARGE &&m_LowerStates.front() == ANIM_STATE::IDLE)
			m_bCanMove = true;
		break;
	case LOWER_STATE::JUMP:
	case LOWER_STATE::DOUBLE_JUMP:
		switch (m_eAnimState)
		{
			//점프에 대한 처리 어떻게 할 것인가?
			//점프에 대한 처리같은 함수는 따로 만드는 것이 맞는가
		case ITEM_CARRY_LARGE:
		case SHAKE_FLASK:
		case JUMP_FORWARD:
		case JUMP_DOUBLE:
		case JUMP_LEDGE:
			m_fJumpPower -= 0.05f;
			if (m_fJumpPower <= m_fJumpFinishPower)
			{
				m_fJumpPower = 2.f;
				m_bJump = false;
				m_bDoubleJump = false;
				Reset_LowerAnim();

			}
			else
				m_pTransformCom->Jump(fTimeDelta, m_fJumpPower);
			break;
	/*		m_fJumpPower -= 0.05f;
			if (m_fJumpPower <= -1.95f)
			{
				m_fJumpPower = 2.f;
				m_bJump = false;
				m_bDoubleJump = false;
				Reset_LowerAnim();
			}
			else
				m_pTransformCom->Jump(fTimeDelta, m_fJumpPower);
			break;*/
		}
		break;

	case LOWER_STATE::ATTACK:
		if (m_UpperStates.empty())
		{
			m_bAttacked = false;
			m_bCanMove = true;
		}
		break;
	case LOWER_STATE::THROW:
		switch (m_eAnimState)
		{
		case ITEM_PICKUP_LARGE:
		case ITEM_THROW_ONEHAND:
			if (m_LowerStates.empty())
			{
				m_bCanMove = true;
				if (m_bJump)
				{
					m_bCanMove = true;
					m_bIsPickup = false;

					/*Jump함수 다시 만들어서 넣어야할 듯.*/
					m_eCurrentLowerState = LOWER_STATE::JUMP;
					Push_UpperState(UPPER_STATE::JUMP);
				}
			}
			break;
		case ITEM_THROW:
			if (m_LowerStates.empty())
			{
				m_bCanMove = true;
				m_bIsPickup = false;
			}
			else if(m_LowerStates.front() != ITEM_THROW)
			{
				if (m_bJump)
				{
					m_bCanMove = true;
					m_bIsPickup = false;

					/*Jump함수 다시 만들어서 넣어야할 듯.*/
					m_eCurrentLowerState = LOWER_STATE::JUMP;
					Push_UpperState(UPPER_STATE::JUMP);
				}		
			}		
			break;
		}
		break;

	case LOWER_STATE::DIVE:
		switch (m_eAnimState)
		{
		case DIVE_IDLE:
			m_fJumpPower -= 0.05f;
			if (m_fJumpPower <= m_fJumpFinishPower)
			{
				m_fJumpPower = 2.f;
				m_bJump = false;
				m_bDoubleJump = false;
				m_ePreLowerState = m_eCurrentLowerState;
				Sliding();
			}
			else
				m_pTransformCom->Jump(fTimeDelta, m_fJumpPower);
			break;
		case DIVE_SLIDE:
			if (m_ePreLowerState == LOWER_STATE::JUMP ||
				m_ePreLowerState == LOWER_STATE::DOUBLE_JUMP ||
				m_ePreLowerState == LOWER_STATE::RUN ||
				m_ePreLowerState == LOWER_STATE::SPRINT ||
				m_ePreLowerState == LOWER_STATE::DIVE)
			{
				m_pTransformCom->Go_Straight(fTimeDelta * 2.f);				
			}
			break;
		}
		break;

	case LOWER_STATE::HOOK:
		switch (m_eAnimState)
		{
		case HOOKSHOT_START:
			break;
		case HOOK0SHOT_SWING:
			break;
		case HOOKSHOT_FINISH:
			break;
		}
		break;

	case LOWER_STATE::HURT:
		if (m_bHurt)
		{
			Hurt(fTimeDelta);
		}
		break;
	}
}

void CHatKid::Reset_LowerAnim()
{
	while (!m_LowerStates.empty())
	{
		m_LowerStates.pop();
	}
}

void CHatKid::Reset_UpperAnim()
{
	while (!m_UpperStates.empty())
	{
		m_UpperStates.pop();
	}
}

void CHatKid::Sliding()
{
	Reset_LowerAnim();
	m_LowerStates.push(DIVE_SLIDE);
	m_LowerStates.push(SLIDE_FINISH);
	Push_UpperState(UPPER_STATE::DIVE);
}

void CHatKid::Move(_float fTimeDelta)
{
	if (m_bAttacked || m_bDived)
		return;

	if (m_bJump)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		return;
	}

	/*if) 이미 queue안에 Run이 들어잇따면 안들어가게 하기.*/
	if (m_eHatType == HAT_TYPE::HAT_SPRINT &&
		m_bSkillUsing == true)
	{
		if (m_eCurrentLowerState != LOWER_STATE::SPRINT)
		{
			m_ePreLowerState = m_eCurrentLowerState;
			m_eCurrentLowerState = LOWER_STATE::SPRINT;

			Reset_LowerAnim();
			m_LowerStates.push(ANIM_STATE::SPRINT);

			if (!m_bIsPickup)
			{
				Push_UpperState(UPPER_STATE::SPRINT);
			}
		}
	}
	else
	{
		if (m_eCurrentLowerState != LOWER_STATE::RUN)
		{
			m_ePreLowerState = m_eCurrentLowerState;
			m_eCurrentLowerState = LOWER_STATE::RUN;

			Reset_LowerAnim();
			m_LowerStates.push(ANIM_STATE::RUN);

			if (!m_bIsPickup && !m_bSkillUsing)
			{
				Push_UpperState(UPPER_STATE::RUN);
			}

		}
	}
}

void CHatKid::Hurt(_float fTimeDelta)
{
	m_fHurtTime -= fTimeDelta;
	if (m_fHurtTime <= 0.f)
	{
		m_bHurt = false;
		m_fHurtTime = 1.f;
		m_bCanInputKey = true;
		Reset_LowerAnim();
		Reset_UpperAnim();
	}

}

void CHatKid::Shoot_Flask()
{
	if (!m_bIsShoot)
		return;

	if (m_eWeaponType != WEAPON_TYPE::WEAPON_NONE)
	{
		m_pEquipments->Delete_Part(PARTS_WEAPON, WEAPON_FLASK);
		m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_NONE);
		m_eWeaponType = WEAPON_TYPE::WEAPON_NONE;



		CEquipment::EQUIPDESC EquipDesc;

		//Test_Flask
		CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("bip_ItemPalmR01");
		if (nullptr == pSocket)
			return;

		EquipDesc.pSocket = pSocket;
		EquipDesc.szName = TEXT("Flask");
		EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
		EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		EquipDesc.bIsDepartment = true;
		EquipDesc.fOwnerMatrix = m_pTransformCom->Get_World4x4();
		//XMStoreFloat4x4(&(EquipDesc.fOwnerMatrix), pSocket->Get_CombinedTransformationMatrix());
		Safe_AddRef(pSocket);

		if (FAILED(m_pEquipments->Add_Department_Part(PARTS_WEAPON, WEAPON_FLASK, &EquipDesc)))
			return;

		m_bIsShoot = false;
	}
}



HRESULT CHatKid::Move_Front(_float fTimeDelta)
{
	Move(fTimeDelta);
	//임시용
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Camera", L"Com_Transform");

	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vCameraLook = pTransform->Get_State(CTransform::STATE_LOOK);
	vCameraLook = XMVectorSetY(vCameraLook, XMVectorGetY(vPlayerLook));
	_vector vLookDot = XMVector3AngleBetweenVectors(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(vLookDot));


	// 외적, 내적으로 angle 보정
	_vector vCross = XMVector3Cross(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_vector vDot = XMVector3Dot(XMVector3Normalize(vCross), XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_float fScala = XMVectorGetX(vDot);

	if (fScala < 0.f)
		fAngle = 360.f - fAngle;

	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fAngle - 5.f);
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);
	m_pTransformCom->Go_Straight(fTimeDelta);
	m_eCurKeyState = KEY_END;

	printf("fAngle: %d \n", (_int)fAngle);
	RELEASE_INSTANCE(CGameInstance);

	//m_pTransformCom->Go_Straight(fTimeDelta);

	return S_OK;
}

HRESULT CHatKid::Move_Back(_float fTimeDelta)
{
	Move(fTimeDelta);
	//임시
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Camera", L"Com_Transform");

	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vCameraLook = pTransform->Get_State(CTransform::STATE_LOOK);
	vCameraLook = XMVectorSetY(vCameraLook, XMVectorGetY(vPlayerLook));
	vCameraLook *= -1.f;
	_vector vLookDot = XMVector3AngleBetweenVectors(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(vLookDot));

	// 외적, 내적으로 angle 보정
	_vector vCross = XMVector3Cross(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_vector vDot = XMVector3Dot(XMVector3Normalize(vCross), XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_float fScala = XMVectorGetX(vDot);

	if (fScala < 0.f)
		fAngle = 360.f - fAngle;


	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fAngle - 5.f);
	m_pTransformCom->Go_Straight(fTimeDelta);
	m_eCurKeyState = KEY_END;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Move_Left(_float fTimeDelta)
{
	Move(fTimeDelta);

	//임시용
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Camera", L"Com_Transform");

	m_eCurKeyState = KEY_LEFT;

	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vCameraLook = pTransform->Get_State(CTransform::STATE_LOOK);
	vCameraLook = XMVectorSetY(vCameraLook, XMVectorGetY(vPlayerLook));
	_vector tempVec = XMVector3AngleBetweenVectors(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(tempVec));

	if (m_ePreKeyState == KEY_RIGHT)
		fAngle += 180.f;
	else
		fAngle += 270.f;

	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fAngle);
	m_pTransformCom->Go_Straight(fTimeDelta);
	m_ePreKeyState = m_eCurKeyState;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Move_Right(_float fTimeDelta)
{
	Move(fTimeDelta);
	//임시
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Camera", L"Com_Transform");

	m_eCurKeyState = KEY_RIGHT;

	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vCameraLook = pTransform->Get_State(CTransform::STATE_LOOK);
	vCameraLook = XMVectorSetY(vCameraLook, XMVectorGetY(vPlayerLook));
	_vector tempVec = XMVector3AngleBetweenVectors(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(tempVec));

	if (m_ePreKeyState == KEY_LEFT)
		fAngle += 180.f;
	else
		fAngle = 90.f - fAngle;

	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fAngle);
	m_pTransformCom->Go_Straight(fTimeDelta);
	m_ePreKeyState = m_eCurKeyState;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Move_RightFront(_float fTimeDelta)
{
	Move(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Camera", L"Com_Transform");

	m_eCurKeyState = KEY_RF;

	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vCameraLook = pTransform->Get_State(CTransform::STATE_LOOK);
	vCameraLook = XMVectorSetY(vCameraLook, XMVectorGetY(vPlayerLook));
	_vector tempVec = XMVector3AngleBetweenVectors(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(tempVec));

	// 외적, 내적으로 angle 보정
	_vector vCross = XMVector3Cross(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_vector vDot = XMVector3Dot(XMVector3Normalize(vCross), XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	_float fScala = XMVectorGetX(vDot);

	if (fScala < 0.f)
		fAngle = 360.f - fAngle;

	if (m_ePreKeyState == KEY_LB)
		fAngle += 180.f;
	else
		fAngle += 45.f;

	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fAngle);
	m_pTransformCom->Go_Straight(fTimeDelta);
	m_ePreKeyState = m_eCurKeyState;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Move_RightBack(_float fTimeDelta)
{
	Move(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Camera", L"Com_Transform");

	m_eCurKeyState = KEY_RB;

	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vCameraLook = pTransform->Get_State(CTransform::STATE_LOOK);
	vCameraLook = XMVectorSetY(vCameraLook, XMVectorGetY(vPlayerLook));
	vCameraLook *= -1.f;

	_vector tempVec = XMVector3AngleBetweenVectors(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(tempVec));

	// 외적, 내적으로 angle 보정
	_vector vCross = XMVector3Cross(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_vector vDot = XMVector3Dot(XMVector3Normalize(vCross), XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_float fScala = XMVectorGetX(vDot);

	if (fScala < 0.f)
		fAngle = 360.f - fAngle;

	if (m_ePreKeyState == KEY_LF)
		fAngle += 180.f;
	else
		fAngle += 315.f;

	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fAngle);
	m_pTransformCom->Go_Straight(fTimeDelta);
	m_ePreKeyState = m_eCurKeyState;

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CHatKid::Move_LeftFront(_float fTimeDelta)
{
	Move(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Camera", L"Com_Transform");

	m_eCurKeyState = KEY_LF;

	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vCameraLook = pTransform->Get_State(CTransform::STATE_LOOK);
	vCameraLook = XMVectorSetY(vCameraLook, XMVectorGetY(vPlayerLook));
	_vector tempVec = XMVector3AngleBetweenVectors(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(tempVec));

	// 외적, 내적으로 angle 보정
	_vector vCross = XMVector3Cross(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_vector vDot = XMVector3Dot(XMVector3Normalize(vCross), XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_float fScala = XMVectorGetX(vDot);

	if (fScala < 0.f)
		fAngle = 360.f - fAngle;

	if (m_ePreKeyState == KEY_RB)
		fAngle += 180.f;
	else
		fAngle += 315.f;

	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fAngle);
	m_pTransformCom->Go_Straight(fTimeDelta);
	m_ePreKeyState = m_eCurKeyState;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Move_LeftBack(_float fTimeDelta)
{
	Move(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Camera", L"Com_Transform");

	m_eCurKeyState = KEY_LB;

	_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vCameraLook = pTransform->Get_State(CTransform::STATE_LOOK);
	vCameraLook = XMVectorSetY(vCameraLook, XMVectorGetY(vPlayerLook));
	vCameraLook *= -1.f;

	_vector tempVec = XMVector3AngleBetweenVectors(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(tempVec));

	// 외적, 내적으로 angle 보정
	_vector vCross = XMVector3Cross(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCameraLook));
	_vector vDot = XMVector3Dot(XMVector3Normalize(vCross), XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_float fScala = XMVectorGetX(vDot);

	if (fScala < 0.f)
		fAngle = 360.f - fAngle;


	if (m_ePreKeyState == KEY_RF)
		fAngle += 180.f;
	else
		fAngle += 405.f;

	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fAngle);
	m_pTransformCom->Go_Straight(fTimeDelta);
	m_ePreKeyState = m_eCurKeyState;

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CHatKid::Jump(_float fTimeDelta)
{
	//기본 점프 : 공격중인가, 이단 점프가 끝난 상태인가?
	//이단점프 : 이미 점프를 한 상태인가? 다이브 상태인가? 물건을 들고있는가? 스킬을 쓴상태인건가?
	if (m_bAttacked || m_bHurt || m_bDived ||
		m_eCurrentLowerState == LOWER_STATE::THROW )
		return S_OK;

	if (!m_bJump && m_eCurrentLowerState != LOWER_STATE::JUMP)
	{
		Reset_LowerAnim();
		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::JUMP;

		m_LowerStates.push(JUMP_FORWARD);
		m_LowerStates.push(JUMP_LEDGE);
		if (m_eCurrentUpperState == UPPER_STATE::IDLE || m_eCurrentUpperState == UPPER_STATE::RUN)
		{
			Push_UpperState(UPPER_STATE::JUMP);
		}
		m_bJump = true;

		m_fJumpPower = 2.f;
		m_fJumpFinishPower = -1.f * m_fJumpPower;

		//m_pTransformCom->Jump(fTimeDelta, m_fJumpPower);
	}
	else if (m_bJump && m_eCurrentLowerState != LOWER_STATE::DOUBLE_JUMP)
	{//더블 점프 만들기.
		Reset_LowerAnim();
		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::DOUBLE_JUMP;
		m_LowerStates.push(JUMP_DOUBLE);
		m_LowerStates.push(JUMP_LEDGE);
		
		Push_UpperState(UPPER_STATE::DOUBLE_JUMP);

		m_bDoubleJump = true;
		m_fJumpPower += 2.f;
		m_fJumpFinishPower = -1.f *m_fJumpPower;
		

		//m_pTransformCom->Jump(fTimeDelta, m_fJumpPower);
	}
	

	return S_OK;
}

HRESULT CHatKid::Action_1(_float fTimeDelta)
{// Sliding
	
	//예외처리
	if (m_bAttacked || m_bIsPickup ||m_bHurt)
		return S_OK;

	if (m_eCurrentLowerState != LOWER_STATE::DIVE)
	{
		Reset_LowerAnim();
		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::DIVE;	
		m_bDived = true;
		m_bDoubleJump = false;
		if (m_bJump || m_bDoubleJump)
		{//점프 중일 때의 다이빙 구현
			m_LowerStates.push(DIVE_IDLE);
			//다이빙하다가 바닥에 닿았을 시에 대한 처리 필요.
			Push_UpperState(UPPER_STATE::DIVE);
		}
		else
		{//지상에 있을 때의 다이빙 구현.
			Sliding();
		}
	}
	else if (m_eCurrentLowerState == LOWER_STATE::DIVE)
	{//다이빙중 다시 ctrl키를 눌렀을 때 더블점프시행한다.
		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::DOUBLE_JUMP;
		Reset_LowerAnim();
		m_LowerStates.push(JUMP_DOUBLE);
		m_LowerStates.push(JUMP_LEDGE);
		Push_UpperState(UPPER_STATE::DOUBLE_JUMP);
		m_bDived = false;
		//m_LowerStates.push(JUMP_LEDGE);
	}

	return S_OK;
}

HRESULT CHatKid::Action_2(_float fTimeDelta)
{	//Press Skill
	

	if (m_bAttacked || m_bIsPickup || m_bDoubleJump)
		return S_OK;

	if (m_eCurrentUpperState != UPPER_STATE::IDLE &&m_eCurrentUpperState != UPPER_STATE::RUN
		&& m_eCurrentUpperState != UPPER_STATE::UMBRELLA)
		return S_OK;
	
	switch (m_eHatType)
	{
	case HAT_TYPE::HAT_WITCH:
		if (m_eCurrentUpperState != UPPER_STATE::SKILL)
		{
			m_bSkillUsing = true;
			Push_UpperState(UPPER_STATE::SKILL);

			CEquipment::EQUIPDESC EquipDesc;

			//Test_Flask
			CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("bip_ItemPalmR01");
			if (nullptr == pSocket)
				return E_FAIL;

			EquipDesc.pSocket = pSocket;
			EquipDesc.szName = TEXT("Flask");
			EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
			EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
			Safe_AddRef(pSocket);

			if (FAILED(m_pEquipments->Add_Parts(PARTS_WEAPON, WEAPON_FLASK, &EquipDesc)))
				return E_FAIL;
			m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_FLASK);
			m_eWeaponType = WEAPON_TYPE::WEAPON_FLASK;

			m_fSkillTime = 0.f;
		}
		m_fSkillTime += fTimeDelta;
		break;

	case HAT_TYPE::HAT_SPRINT:
		m_bSkillUsing = true;
		break;
	}



	return S_OK;
}

HRESULT CHatKid::Action_3(_float fTimeDelta)
{	// Inventory && Menu
	return S_OK;
}

HRESULT CHatKid::Action_4(_float fTimeDelta)
{//Pickup Action
	//Pickup시 상태가 들고있는 상태로 들어갑니다.
	if (m_bAttacked)
		return S_OK;

	if (!m_bIsPickup)
	{
		if (m_eCurrentLowerState == LOWER_STATE::IDLE &&
			(m_eCurrentUpperState == UPPER_STATE::IDLE || 
				m_eCurrentUpperState == UPPER_STATE::UMBRELLA))
		{
			m_bIsPickup = true;
			Reset_LowerAnim();

			m_LowerStates.push(ITEM_PICKUP_LARGE);
			m_LowerStates.push(IDLE);

			m_ePreLowerState = m_eCurrentLowerState;
			m_eCurrentLowerState = LOWER_STATE::IDLE;

			Push_UpperState(UPPER_STATE::CARRY);

			m_bCanMove = false;
			//m_bCanInputKey = false;
			//m_pController->Set_LockKeys(true);
		}	
	}
	else
	{//내려놓기.
		if (m_bJump)
		{
			m_ePreLowerState = m_eCurrentLowerState;
			m_eCurrentLowerState = LOWER_STATE::THROW;
			Reset_LowerAnim();
			m_LowerStates.push(ITEM_THROW);

			Push_UpperState(UPPER_STATE::THROW);
			//m_bIsPickup = false;
			m_bCanMove = false;
			m_LowerStates.push(JUMP_LEDGE);
			return S_OK;
		}


 		m_eCurrentLowerState = LOWER_STATE::THROW;
		Reset_LowerAnim();
		m_LowerStates.push(ITEM_THROW);

		Push_UpperState(UPPER_STATE::THROW);
		//m_bIsPickup = false;
		m_bCanMove = false;
		//m_pController->Set_LockKeys(true);
	}


	return S_OK;
}

HRESULT CHatKid::Action_5(_float fTimeDelta)
{//키를 땠을 때의 스킬 발동. 
	//후에 모자상태에 따른 switch값 분류.
	
	switch (m_eHatType)
	{
	case HAT_TYPE::HAT_WITCH:

		Reset_LowerAnim();

		m_bIsShoot = true;
		Shoot_Flask();

		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::THROW;
		m_LowerStates.push(ITEM_THROW_ONEHAND);
		Push_UpperState(UPPER_STATE::THROW);
		m_bSkillUsing = false;
		//Push_UpperState()
		m_bCanMove = false;
		//m_pController->Set_LockKeys(true);
		break;

	case HAT_TYPE::HAT_SPRINT:
		if (m_bDoubleJump)
		return S_OK;

		m_bSkillUsing = false;
		break;

	}

	

	return S_OK;
}

HRESULT CHatKid::Action_6(_float fTimeDelta)
{//FOR Test : 현재는 공격키

	if (m_bJump || m_bHurt || m_bIsPickup)
		return S_OK;

	if (m_eCurrentLowerState != LOWER_STATE::ATTACK)
	{
		m_bAttacked = true;
		m_fAtkDelay = 0.f;
		m_iAtkCount = 0;
	}

	if (m_iAtkCount == 2 &&
		m_fAtkDelay >= 0.f && m_fAtkDelay <= 0.2f &&
		m_eWeaponType == WEAPON_TYPE::WEAPON_UMBRELLA)
	{
		if (m_eWeaponType == WEAPON_TYPE::WEAPON_UMBRELLA)
		{
			m_LowerStates.push(UMBRELLA_ATK_C);
		}
		m_fAtkDelay = 0.f;
		++m_iAtkCount;
		Push_UpperState(UPPER_STATE::ATTACK);
	}

	if (m_iAtkCount == 1 &&
		m_fAtkDelay >= 0.f && m_fAtkDelay <= 0.2f)
	{
		if (m_eWeaponType == WEAPON_TYPE::WEAPON_UMBRELLA)
		{
			m_LowerStates.push(UMBRELLA_ATK_B);
		}
		else if (m_eWeaponType == WEAPON_TYPE::WEAPON_NONE)
		{
			m_LowerStates.push(PUNCH_B);
		}
		m_fAtkDelay = 0.f;
		++m_iAtkCount;
		Push_UpperState(UPPER_STATE::ATTACK);
	}

	if (m_iAtkCount == 0)
	{
		++m_iAtkCount;

		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::ATTACK;
		Reset_LowerAnim();
		if (m_eWeaponType == WEAPON_TYPE::WEAPON_UMBRELLA)
		{
			m_LowerStates.push(UMBRELLA_ATK_A);
		}
		else if (m_eWeaponType == WEAPON_TYPE::WEAPON_NONE)
		{
			m_LowerStates.push(PUNCH_A);
		}

		Push_UpperState(UPPER_STATE::ATTACK);
	}

	return S_OK;
}

HRESULT CHatKid::Idle(_float fTimeDelta)
{
	if (m_bJump || m_bDoubleJump || m_bDived)
		return S_OK;

	if (m_eCurrentLowerState != LOWER_STATE::IDLE)
	{
		if (m_LowerStates.empty()
			|| m_AnimLoopStates[m_LowerStates.front()] == true
			)
		{
			Reset_LowerAnim();
			m_ePreLowerState = m_eCurrentLowerState;
			m_eCurrentLowerState = LOWER_STATE::IDLE;
			m_LowerStates.push(ANIM_STATE::IDLE);
			if (!m_bIsPickup && m_eAnimState != SHAKE_FLASK)
			{
				Push_UpperState(UPPER_STATE::IDLE);
			}
		}
		
		
	}

	//나중에 점프가 바닥에 닿았을 때는 어떻게 처리할건데? 바닥에닿으면 clear해야지.

	//m_pModelCom->Set_NextAnimIndex_Upper(ITEM_CARRY_LARGE);
	//m_pModelCom->Set_NextAnimIndex_Lower(IDLE);
	return S_OK;
}

void CHatKid::TestFunc(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down('1'))
	{
		m_pEquipments->Change_Part(PARTS_HAT, HAT_DEFAULT);
		m_eHatType = HAT_TYPE::HAT_DEFAULT;
	}
	else if (pGameInstance->Key_Down('2'))
	{
		m_pEquipments->Change_Part(PARTS_HAT, HAT_WITCH);
		m_eHatType = HAT_TYPE::HAT_WITCH;
	}
	else if (pGameInstance->Key_Down('3'))
	{
		m_pEquipments->Change_Part(PARTS_HAT, HAT_SPRINT);
		m_eHatType = HAT_TYPE::HAT_SPRINT;
	}
	else if (pGameInstance->Key_Down('4'))
	{
		if (m_eWeaponType != WEAPON_TYPE::WEAPON_NONE)
		{
			m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_NONE);
			m_eWeaponType = WEAPON_TYPE::WEAPON_NONE;
		}
		else
		{
			m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_UMBRELLA);
			m_eWeaponType = WEAPON_TYPE::WEAPON_UMBRELLA;
		}
	}
	else if (pGameInstance->Key_Down('5'))
	{
		if (!m_bHurt)
		{
			Reset_LowerAnim();
			m_bHurt = true;
			m_bCanInputKey = false;
			m_ePreLowerState = m_eCurrentLowerState;
			m_eCurrentLowerState = LOWER_STATE::HURT;
			m_LowerStates.push(HURT);
			Push_UpperState(UPPER_STATE::HURT);
		}
		
	}


	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CHatKid::Ready_Parts()
{
	m_pEquipments = new CEquipments;
	if (FAILED(m_pEquipments->Initialize(PARTS_END)))
		return E_FAIL;

	//0번째는 장비착용 X
	if (FAILED(m_pEquipments->Add_Parts(PARTS_HAT, HAT_NONE)))
		return E_FAIL;
	//0번째는 장비착용 X
	if (FAILED(m_pEquipments->Add_Parts(PARTS_WEAPON, WEAPON_NONE)))
		return E_FAIL;

	//Test_Umbrella
	CEquipment::EQUIPDESC EquipDesc;
	CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("bip_ItemPalmR01");
	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Umbrella");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts(PARTS_WEAPON, WEAPON_UMBRELLA, &EquipDesc)))
		return E_FAIL;

	

	//Test_Hats_Default
	pSocket = m_pModelCom->Get_BonePtr("bip_hat01");
	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Hat");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	EquipDesc.iTypeNum = (_uint)HAT_DEFAULT;
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts(PARTS_HAT, HAT_DEFAULT, &EquipDesc)))
		return E_FAIL;

	//Test_Hats_Witch
	//CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("bip_hat01");
	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Hat");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	EquipDesc.iTypeNum = (_uint)HAT_WITCH;
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts(PARTS_HAT, HAT_WITCH, &EquipDesc)))
		return E_FAIL;

	//Test_Hats_Sprint
	//CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("bip_hat01");
	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Hat");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	EquipDesc.iTypeNum = (_uint)HAT_SPRINT;
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts(PARTS_HAT, HAT_SPRINT, &EquipDesc)))
		return E_FAIL;


	m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_UMBRELLA);
	m_pEquipments->Change_Part(PARTS_HAT, HAT_NONE);
	//Test
	m_eWeaponType = WEAPON_TYPE::WEAPON_UMBRELLA;

	return S_OK;
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
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
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
	/*if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HatKid"), (CComponent**)&m_pModelCom)))
		return E_FAIL;*/

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

	Safe_Release(m_pEquipments);

	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pController);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
