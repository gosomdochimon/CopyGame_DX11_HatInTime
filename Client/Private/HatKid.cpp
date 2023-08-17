#include "stdafx.h"
#include "..\Public\HatKid.h"
#include "GameInstance.h"
#include "PlayerController.h"
#include "Equipments.h"
#include "HierarchyNode.h"
#include "Deligate_Monster.h"
#include "Apple.h"
#include "Effect.h"
#include "Camera_Manager.h"
#include "Level_Loading.h"
CHatKid::CHatKid(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CPlayer(pDevice, pContext)
{
}

CHatKid::CHatKid(const CHatKid & rhs)
	: CPlayer(rhs)
{
}

HRESULT CHatKid::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHatKid::Initialize(void * pArg)
{
	__super::Initialize(pArg);


	if (FAILED(Init_Position()))
		return E_FAIL;

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

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_Group(CCollider_Manager::GROUP_TYPE::TYPE_PLAYER, this, m_pSPHERECom);
	Safe_AddRef(this);
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	//pGameInstance->PlayBGM(TEXT("OldMan_AlienChase_Phase2.ogg"), .5f);
	//RELEASE_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);
	m_bLoading = false;
	return S_OK;
}

void CHatKid::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	if (Change_Level() == true)
		return;

	CoolTime(fTimeDelta);
	
	Pop_Animation();

	Change_ActionBoolState_Anim(fTimeDelta);

	if (m_bLoading == true)
		return;

	m_pController->Input_Controller(fTimeDelta);

	TestFunc(fTimeDelta);

	Play_Animation(fTimeDelta);

	Set_Height();

	if (m_pEquipments != nullptr)
		m_pEquipments->Tick(fTimeDelta);

	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ m_pColliderBone->Get_OffsetMatrix() *
		m_pColliderBone->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);
	m_pSPHERECom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CHatKid::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_bLoading)
		return;
	if (m_pEquipments != nullptr)
		m_pEquipments->Late_Tick(fTimeDelta);



	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_WEAPON]);

#ifdef _DEBUG
		//m_pRendererCom->Add_Debug(m_pSPHERECom);
		//m_pRendererCom->Add_Debug(m_pNavigationCom);
#endif
	}
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
		//m_pSPHERECom->Render();
		if (m_pNavigationCom != nullptr)
			//m_pNavigationCom->Render_Navigation();
	#endif

	return S_OK;
}

HRESULT CHatKid::Init_Position()
{

	CGameInstance* pGame = GET_INSTANCE(CGameInstance);

	_float3 fPos;
	if (pGame->Get_DestinationLevel() == LEVEL_GAMEPLAY)
	{
		_ulong			dwByte = 0;
		HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Start_Pos/Player_Start.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return E_FAIL;

		_float3		vPoints;
		_float3		vPos;
		while (true)
		{
			ReadFile(hFile, &vPoints, sizeof(_float3), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}

		CloseHandle(hFile);

		_vector vPoint = XMLoadFloat3(&vPoints);
		vPoint = XMVectorSet(vPoints.x, vPoints.y, vPoints.z, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPoint);

		Safe_Release(m_pNavigationCom);
		
		/* For.Com_Navigation */
		CNavigation::NAVIDESC			NaviDesc;
		ZeroMemory(&NaviDesc, sizeof NaviDesc);

		NaviDesc.iCurrentCellIndex = 0;
		XMStoreFloat3(&NaviDesc.fCurrent_Position, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		//LevelIndex에 따라 네비게이션 컴포넌트 설정할 것
		if (FAILED(__super::Add_Components(TEXT("Com_Navigation_GamePlay"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;

	/*	XMStoreFloat3(&fPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		m_pNavigationCom->Change_CurrentPos(fPos);
		m_pNavigationCom->Compute_CurrentCell();*/
		//m_pNavigationCom->Change_CellType_Group(0, 1, 1);
		//m_pNavigationCom->Change_CellType_Group(2, 35, 0);
		Set_Height();

		

	}
	else
	{
		_vector vPoint = XMVectorSet(12.f, 0.f, -22.f, 1.f);
		//vPoint = XMVectorSet(vPoints.x, vPoints.y, vPoints.z, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPoint);


		XMStoreFloat3(&fPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		m_pNavigationCom->Change_CurrentPos(fPos);
		m_pNavigationCom->Compute_CurrentCell();
		m_pNavigationCom->Change_CellType_Group(0, 1, 1);
		m_pNavigationCom->Change_CellType_Group(2, 35, 0);
		Set_Height();
	}
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Setup_Anim_Loop(void)
{/*For AnimState Loop Setting*/
	m_AnimLoopStates.reserve(ANIM_STATE::STATE_END);
	m_AnimLoopStates.resize(ANIM_STATE::STATE_END);

	m_AnimLoopStates[ANIM_STATE::IDLE] = true;
	m_AnimLoopStates[ANIM_STATE::IDLE_TAUNT] = true;
	m_AnimLoopStates[ANIM_STATE::RUN] = true;
	m_AnimLoopStates[ANIM_STATE::RUN_OLD] = true;
	m_AnimLoopStates[ANIM_STATE::SPRINT] = true;
	m_AnimLoopStates[ANIM_STATE::JUMP_FORWARD] = false;
	m_AnimLoopStates[ANIM_STATE::JUMP_DOUBLE] = false;
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
		if (m_eCurrentUpperState == UPPER_STATE::ATTACK)
		{
			CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
			//std::function<void(CMonster&)> func = &CMonster::Release_Super;
			pDeligate->Notify_SuperEvent();

			RELEASE_INSTANCE(CDeligate_Monster);
		}
		m_UpperStates.pop();
		m_bAnimFinished_Upper = false;
	}

	if (m_bAnimFinished_Lower && !m_LowerStates.empty())
	{
		m_LowerStates.pop();
		m_bAnimFinished_Lower = false;
	}
}

void CHatKid::Play_Animation(_float fTimeDelta)
{/*Play Anims all( Weapon etc... */
	if (!m_UpperStates.empty())
	{
		if (m_eAnimState != m_UpperStates.front())
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
			printf_s("Lower: IDLE \n");
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
	case LOWER_STATE::RUN:
		if (m_eCurrentUpperState != eUpperState)
		{
			printf_s("Lower: RUN \n");
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
			printf_s("Lower: SPRINT \n");
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
			printf_s("Lower: JUMP \n");
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
				if (m_ePreLowerState == LOWER_STATE::HURT)
				{
					m_UpperStates.push(ANIM_STATE::JUMP_LEDGE);
				}
				else
				{
					m_UpperStates.push(ANIM_STATE::JUMP_FORWARD);
					m_UpperStates.push(ANIM_STATE::JUMP_LEDGE);
				}
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
			printf_s("Lower: DOUBLE_JUNP \n");
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
			if (m_eWeaponType == WEAPON_TYPE::WEAPON_PUNCH)
			{
				m_UpperStates.push(ANIM_STATE::PUNCH_A);
			}
			else if (m_eWeaponType == WEAPON_TYPE::WEAPON_UMBRELLA)
			{
				m_UpperStates.push(ANIM_STATE::UMBRELLA_ATK_A);
			}
			break;
		case 2:
			if (m_eWeaponType == WEAPON_TYPE::WEAPON_PUNCH)
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
		else if (m_eCurrentUpperState != eUpperState)
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
		switch (m_eCurrentUpperState)
		{
		case UPPER_STATE::CARRY:
			m_bIsPickup = false;
		}

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

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	switch (m_eCurrentLowerState)
	{
	case LOWER_STATE::IDLE:
		if (m_eAnimState == ITEM_PICKUP_LARGE &&m_LowerStates.front() == ANIM_STATE::IDLE)
			m_bCanMove = true;
		break;
	case LOWER_STATE::RUN:
		m_fRunningTime -= fTimeDelta;
		m_fSoundTimer -= fTimeDelta;
		if (m_fRunningTime <= 0.f)
		{			
			CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");
			Create_Puff(m_pStuckSocket, fTimeDelta);
			m_fRunningTime = 0.2f;
		}
		if (m_fSoundTimer <= 0.f)
		{
			m_fSoundTimer = 0.5f;
			Play_Sound_Lower(LOWER_STATE::RUN);
		}
		break;
	case LOWER_STATE::SPRINT:
		m_fRunningTime -= fTimeDelta;
		m_fWalkTIme -= fTimeDelta;
		if (m_fRunningTime <= 0.f)
		{
			CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");


			Create_Puff(m_pStuckSocket, fTimeDelta, 1, 0);
			m_fRunningTime = 0.05f;
		}
		if (m_fWalkTIme <= 0.f)
		{
			Play_Sound_Lower(LOWER_STATE::SPRINT);
			CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");
			Create_Puff(m_pStuckSocket, fTimeDelta);
			m_fWalkTIme = 0.2f;
		}

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
			if (m_pNavigationCom == nullptr)
				return;
			m_fJumpPower -= 0.05f;
			//임시로 점프파워가 0보다 작으면으로 막아둠 필요시 열어야함
			if (m_fJumpPower < 0.f && fPos.y <= m_pNavigationCom->Get_NavigationHeight(fPos))
			{
				//printf_s("Jump_End\n");
				m_fJumpPower = 2.f;
				m_bJump = false;
				m_bDoubleJump = false;
				Reset_LowerAnim();
				Set_Height();
			}
			else
				m_pTransformCom->Jump(fTimeDelta, m_fJumpPower, m_pNavigationCom, 0);
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
			m_pEquipments->Set_CanAttack(false);
		}
		else
		{
			m_fSoundTimer -= fTimeDelta;
			if (m_fSoundTimer <= 0.f)
			{
				Play_Sound_HatKid(SOUND_STATE::ATTACK);
				m_fSoundTimer = 0.5f;
			}
		}
		break;
	case LOWER_STATE::THROW:
		switch (m_eAnimState)
		{
		case ITEM_PICKUP_LARGE:
		case ITEM_THROW_ONEHAND:
			if (m_LowerStates.empty())
			{
				Play_Sound_HatKid(SOUND_STATE::THROW);
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
			else if (m_LowerStates.front() != ITEM_THROW)
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
			{
				m_pTransformCom->Jump(fTimeDelta, m_fJumpPower, m_pNavigationCom, 0);
				m_pTransformCom->Go_Straight(fTimeDelta * 2.f, m_pNavigationCom, 0);
			}

			break;
		case DIVE_SLIDE:
			if (m_ePreLowerState == LOWER_STATE::JUMP ||
				m_ePreLowerState == LOWER_STATE::DOUBLE_JUMP ||
				m_ePreLowerState == LOWER_STATE::RUN ||
				m_ePreLowerState == LOWER_STATE::SPRINT ||
				m_ePreLowerState == LOWER_STATE::DIVE)
			{
				m_pTransformCom->Go_Straight(fTimeDelta * 2.5f, m_pNavigationCom, 0);
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
	Play_Sound_Lower(LOWER_STATE::DIVE);
}

void CHatKid::Move(_float fTimeDelta)
{
	if (m_bAttacked || m_bDived)
		return;

	if (m_bJump || m_bDoubleJump)
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
		m_bSuper = true;
		m_bDived = false;
		m_bCanMove = true;
		if (m_bJump)
		{
			m_ePreLowerState = m_eCurrentLowerState;
			m_eCurrentLowerState = LOWER_STATE::JUMP;
			m_LowerStates.push(JUMP_LEDGE);
			Push_UpperState(UPPER_STATE::JUMP);
		}
		else
		{
			//m_eCurrentLowerState = m_ePreLowerState;
			m_eCurrentLowerState = LOWER_STATE::IDLE;
			m_LowerStates.push(IDLE);
			Push_UpperState(UPPER_STATE::IDLE);
		}
		//m_eCurrentUpperState = m_ePreUpperState;
	}

}

void CHatKid::CoolTime(_float fTimeDelta)
{
	if (m_bSuper)
	{
		m_fSuperTime -= fTimeDelta;
		if (m_fSuperTime <= 0.f)
		{
			m_bSuper = false;
			m_fSuperTime = 2.f;
		}
	}

}

void CHatKid::Set_Height()
{
	if (m_bJump || m_bDoubleJump || m_pNavigationCom == nullptr)
		return;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	fPos.y = m_pNavigationCom->Get_NavigationHeight(fPos);
	vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
}

void CHatKid::Create_Dizzy()
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_hat01");
	//if (nullptr == m_pStuckSocket)
	//	return;
	//_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ m_pStuckSocket->Get_OffsetMatrix() *
	//	m_pStuckSocket->Get_CombinedTransformationMatrix() *
	//	XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	//SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	//SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	//SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	//_float4x4 m_CombinedWorldMatrix;
	//XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

	//CEffect::EFFECTINFO EffectInfo;
	//EffectInfo.iTypeNum = 0;
	//EffectInfo.fLifeTime = 2.f;
	//EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
	////EffectInfo.vPosition.z += 5.f;
	//EffectInfo.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	//for (int i = 0; i < 5; ++i)
	//{
	//	EffectInfo.vRotation = _float3( i* 72.f , 0.f, 0.f);
	//	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DizzyEffect"), LEVEL_GAMEPLAY, TEXT("Layer_Effect2"), &EffectInfo)))
	//		return;
	//}


	//m_fRunningTime = 0.2f;

	//RELEASE_INSTANCE(CGameInstance);



}

void CHatKid::Create_Puff(CHierarchyNode* pStuckSocket, _float fTimeDelta, _uint iType, _uint iMoveType, _float fLifeTime, _float fAngle)
{
	//m_fRunningTime -= fTimeDelta;

	//if (m_fRunningTime <= 0.f)
	//{
	//}
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");
	CHierarchyNode* m_pStuckSocket = pStuckSocket;
	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ m_pStuckSocket->Get_OffsetMatrix() *
		m_pStuckSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 m_CombinedWorldMatrix;
	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

	CEffect::EFFECTINFO EffectInfo;
	EffectInfo.iTypeNum = iType;
	EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
	//XMStoreFloat3(&EffectInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	XMStoreFloat3(&EffectInfo.vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	EffectInfo.iMoveType = iMoveType;
	EffectInfo.fAngle = fAngle;
	EffectInfo.fLifeTime = fLifeTime;
	EffectInfo.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	//EffectInfo.pSocket = m_pStuckSocket;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &EffectInfo)))
		return;



	RELEASE_INSTANCE(CGameInstance);


}

void CHatKid::Create_JumpPuff(_uint iType)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");

	_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ m_pStuckSocket->Get_OffsetMatrix() *
		m_pStuckSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 m_CombinedWorldMatrix;
	XMStoreFloat4x4(&m_CombinedWorldMatrix, /*m_pTransformCom->Get_WorldMatrix() * */SocketMatrix);

	CEffect::EFFECTINFO EffectInfo;
	EffectInfo.iTypeNum = iType;
	EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
	//EffectInfo.pSocket = m_pStuckSocket;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &EffectInfo)))
		return;

	m_fRunningTime = 0.2f;

	RELEASE_INSTANCE(CGameInstance);

}

void CHatKid::Create_HitImpact()
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
	EffectInfo.iTypeNum = 1;
	EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];
	EffectInfo.iMoveType = 0;
	EffectInfo.fAngle = 0.f;
	EffectInfo.fLifeTime = 0.f;


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HitEffect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &EffectInfo)))
		return;
	EffectInfo.iTypeNum = 0;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HitEffect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &EffectInfo)))
		return;

	Safe_Release(pGameInstance);
}

void CHatKid::Play_Sound_Lower(LOWER_STATE eLowerState)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_tchar	szFullPath[MAX_PATH];// = TEXT("footstep_grass_%d.wav");
	_uint iNum = 0;
	/*wcscpy_s(szFullPath, TEXT("jump_%d.wav"));
			wsprintf(szFullPath, szFullPath, iNum);*/
	_float fVolume = 0.5f;

	switch (eLowerState)
	{
	case LOWER_STATE::IDLE:
		break;
	case LOWER_STATE::RUN:
		iNum = rand() % 6;
		wcscpy_s(szFullPath, TEXT("run_carpet0%d.ogg"));
		wsprintf(szFullPath, szFullPath, iNum);
		fVolume = 0.1f;
		break;
	case LOWER_STATE::SPRINT:
		wcscpy_s(szFullPath, TEXT("running_loop.ogg"));
		fVolume = 0.1f;
		break;
	case LOWER_STATE::JUMP:
		wcscpy_s(szFullPath, TEXT("jump_carpet02.ogg"));
		break;
	case LOWER_STATE::DOUBLE_JUMP:
		wcscpy_s(szFullPath, TEXT("DoubleJump.ogg"));
		break;
	case LOWER_STATE::ATTACK:
		wcscpy_s(szFullPath, TEXT("Whip.ogg"));
		break;
	case LOWER_STATE::THROW:
		iNum = rand() % 2;
		wcscpy_s(szFullPath, TEXT("Item_Throw_Swoosh_%d.ogg")); 
		wsprintf(szFullPath, szFullPath, iNum);
		break;
	case LOWER_STATE::DIVE:
		wcscpy_s(szFullPath, TEXT("Dive_Start.ogg"));
		break;
	case LOWER_STATE::HURT:

		break;
	}
	pGameInstance->PlaySounds(szFullPath, SOUND_HATKID_LOWER, fVolume);
	RELEASE_INSTANCE(CGameInstance);
}

void CHatKid::Play_Sound_HatKid(SOUND_STATE eSoundState)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_tchar	szFullPath[MAX_PATH];// = TEXT("footstep_grass_%d.wav");
	_uint iNum = 0;
	_uint iNum2 = 0;
	_float fVolume = 0.7f;

	switch (eSoundState)
	{
	case SOUND_STATE::ATTACK:
		iNum = rand() % 5;
		iNum2 = rand() % 2 + 1;
		switch (iNum)
		{
		case 0:
			wcscpy_s(szFullPath, TEXT("attack_heh0%d.ogg"));
			break;
		case 1:
			wcscpy_s(szFullPath, TEXT("attack_heyah0%d.ogg"));
			break;
		case 2:
			wcscpy_s(szFullPath, TEXT("attack_ho0%d.ogg"));
			break;
		case 3:
			wcscpy_s(szFullPath, TEXT("attack_ya0%d.ogg"));
			break;
		case 4:
			wcscpy_s(szFullPath, TEXT("attack_yeah0%d.ogg"));
			break;
		}
		wsprintf(szFullPath, szFullPath, iNum2);
		break;
	case SOUND_STATE::THROW:
		iNum = rand() % 5;
		switch (iNum)
		{
		case 0:
			wcscpy_s(szFullPath, TEXT("HatKid_Ahhhahaha.ogg"));
			break;
		case 1:
			wcscpy_s(szFullPath, TEXT("HatKid_Bam.ogg"));
			break;
		case 2:
			wcscpy_s(szFullPath, TEXT("HatKid_Blammo.ogg"));
			break;
		case 3:
			wcscpy_s(szFullPath, TEXT("HatKid_Buuuurn.ogg"));
			break;
		case 4:
			wcscpy_s(szFullPath, TEXT("HatKid_BamBam.ogg"));
			break;
		}
		break;
	case SOUND_STATE::HMM:	
		wcscpy_s(szFullPath, TEXT("hmm_b.ogg"));
		break;
	case SOUND_STATE::HURT:
		iNum = rand() % 4;
		switch (iNum)
		{
		case 0:
			wcscpy_s(szFullPath, TEXT("hurt_ah01.ogg"));
			break;
		case 1:
			wcscpy_s(szFullPath, TEXT("hurt_ow01.ogg"));
			break;
		case 2:
			wcscpy_s(szFullPath, TEXT("hurt_short01.ogg"));
			break;
		case 3:
			wcscpy_s(szFullPath, TEXT("hurt_short02.ogg"));
			break;
		}
		break;
	case SOUND_STATE::JUMP:
		//fVolume = 1.f;
		iNum = rand() % 5;
		iNum2 = rand() % 2 + 1;
		switch (iNum)
		{
		case 0:
			wcscpy_s(szFullPath, TEXT("jump_hah0%d.ogg"));
			break;
		case 1:
			wcscpy_s(szFullPath, TEXT("jump_ah0%d.ogg"));
			break;
		case 2:
			wcscpy_s(szFullPath, TEXT("jump_huh0%d.ogg"));
			break;
		case 3:
			wcscpy_s(szFullPath, TEXT("jump_hut0%d.ogg"));
			break;
		case 4:
			wcscpy_s(szFullPath, TEXT("jump_ya0%d.ogg"));
			break;
		}
		wsprintf(szFullPath, szFullPath, iNum2);
		break;
	}
	pGameInstance->PlaySounds(szFullPath, SOUND_HATKID, fVolume);
	RELEASE_INSTANCE(CGameInstance);
}

_bool CHatKid::Change_Level()
{
	CGameInstance* pGame = GET_INSTANCE(CGameInstance);
	
	if (pGame->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		m_bLoading = true;
		if (pGame->Get_DestinationLevel() == LEVEL_GAMEPLAY)
		{
			
				
		}
		
	}
	else if (pGame->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY_PLATFORM)
	{
		_float3 Current_Pos;
		XMStoreFloat3(&Current_Pos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float3 vPos = _float3(-0.1f, 1.6f, -6.5f);
		if (fabs(Current_Pos.x - vPos.x) <= 0.4f &&
			fabs(Current_Pos.z - vPos.z) <= 0.4f)
		{
			m_bLoading = true;
			
		}
	}
	else if (pGame->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
	{
		if (!m_bChanged)
		{
			Init_Position();
			m_bChanged = true;
		}
		m_bLoading = false;
	}
	RELEASE_INSTANCE(CGameInstance);

	return m_bLoading;
}

void CHatKid::IsShoot_Flask(_bool bShoot)
{
	//if (!m_bIsShoot)
	//	return;

	if (m_eWeaponType != WEAPON_TYPE::WEAPON_NONE)
	{
		if (bShoot == false)
		{
			//m_pEquipments->Delete_Part(PARTS_WEAPON, WEAPON_FLASK);
			m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_NONE);
			m_eWeaponType = WEAPON_TYPE::WEAPON_NONE;
			return;
		}
		else
		{
			//m_pEquipments->Delete_Part(PARTS_WEAPON, WEAPON_FLASK);
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
}



HRESULT CHatKid::Move_Front(_float fTimeDelta)
{
	Move(fTimeDelta);
	//임시용
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(pGameInstance->Get_CurrentLevelIndex(), L"Layer_Camera", L"Com_Transform");

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
	if (m_eCurrentLowerState != LOWER_STATE::SPRINT)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, 0);
	}
	else
	{
		m_pTransformCom->Go_Straight(fTimeDelta*1.7f, m_pNavigationCom, 0);
	}

	m_eCurKeyState = KEY_END;

	RELEASE_INSTANCE(CGameInstance);

	//m_pTransformCom->Go_Straight(fTimeDelta);

	return S_OK;
}

HRESULT CHatKid::Move_Back(_float fTimeDelta)
{
	Move(fTimeDelta);
	//임시
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(pGameInstance->Get_CurrentLevelIndex(), L"Layer_Camera", L"Com_Transform");

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
	if (m_eCurrentLowerState != LOWER_STATE::SPRINT)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, 0);
	}
	else
	{
		m_pTransformCom->Go_Straight(fTimeDelta*1.7f, m_pNavigationCom, 0);
	}
	m_eCurKeyState = KEY_END;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Move_Left(_float fTimeDelta)
{
	Move(fTimeDelta);

	//임시용
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(pGameInstance->Get_CurrentLevelIndex(), L"Layer_Camera", L"Com_Transform");

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
	if (m_eCurrentLowerState != LOWER_STATE::SPRINT)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, 0);
	}
	else
	{
		m_pTransformCom->Go_Straight(fTimeDelta*1.7f, m_pNavigationCom, 0);
	}
	m_ePreKeyState = m_eCurKeyState;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Move_Right(_float fTimeDelta)
{
	Move(fTimeDelta);
	//임시
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(pGameInstance->Get_CurrentLevelIndex(), L"Layer_Camera", L"Com_Transform");

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
	if (m_eCurrentLowerState != LOWER_STATE::SPRINT)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, 0);
	}
	else
	{
		m_pTransformCom->Go_Straight(fTimeDelta*1.7f, m_pNavigationCom, 0);
	}
	m_ePreKeyState = m_eCurKeyState;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Move_RightFront(_float fTimeDelta)
{
	Move(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(pGameInstance->Get_CurrentLevelIndex(), L"Layer_Camera", L"Com_Transform");

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
	if (m_eCurrentLowerState != LOWER_STATE::SPRINT)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, 0);
	}
	else
	{
		m_pTransformCom->Go_Straight(fTimeDelta*1.7f, m_pNavigationCom, 0);
	}
	m_ePreKeyState = m_eCurKeyState;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Move_RightBack(_float fTimeDelta)
{
	Move(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(pGameInstance->Get_CurrentLevelIndex(), L"Layer_Camera", L"Com_Transform");

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
	if (m_eCurrentLowerState != LOWER_STATE::SPRINT)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, 0);
	}
	else
	{
		m_pTransformCom->Go_Straight(fTimeDelta*1.7f, m_pNavigationCom, 0);
	}
	m_ePreKeyState = m_eCurKeyState;

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CHatKid::Move_LeftFront(_float fTimeDelta)
{
	Move(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(pGameInstance->Get_CurrentLevelIndex(), L"Layer_Camera", L"Com_Transform");

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
	if (m_eCurrentLowerState != LOWER_STATE::SPRINT)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, 0);
	}
	else
	{
		m_pTransformCom->Go_Straight(fTimeDelta*1.7f, m_pNavigationCom, 0);
	}
	m_ePreKeyState = m_eCurKeyState;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CHatKid::Move_LeftBack(_float fTimeDelta)
{
	Move(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Component(pGameInstance->Get_CurrentLevelIndex(), L"Layer_Camera", L"Com_Transform");

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
	if (m_eCurrentLowerState != LOWER_STATE::SPRINT)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, 0);
	}
	else
	{
		m_pTransformCom->Go_Straight(fTimeDelta*1.7f, m_pNavigationCom, 0);
	}
	m_ePreKeyState = m_eCurKeyState;

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CHatKid::Jump(_float fTimeDelta)
{
	//기본 점프 : 공격중인가, 이단 점프가 끝난 상태인가?
	//이단점프 : 이미 점프를 한 상태인가? 다이브 상태인가? 물건을 들고있는가? 스킬을 쓴상태인건가?
	if (m_bAttacked || m_bHurt || m_bDived ||
		m_eCurrentLowerState == LOWER_STATE::THROW)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->StopSound(SOUND_HATKID_LOWER);
	RELEASE_INSTANCE(CGameInstance);

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
		
		_float fAngle = 0.f;
		for (_uint i = 0; i < 8; ++i)
		{
			fAngle = i *35.f;

			fAngle = XMConvertToRadians(fAngle);
			CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");
			Create_Puff(m_pStuckSocket, fTimeDelta, 0, 1, 2.f, fAngle);
		}
		CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");
		Create_Puff(m_pStuckSocket, fTimeDelta, 2);
		Play_Sound_Lower(LOWER_STATE::JUMP);
		Play_Sound_HatKid(SOUND_STATE::JUMP);
	}
	else if (m_bJump && m_eCurrentLowerState != LOWER_STATE::DOUBLE_JUMP)
	{//더블 점프 만들기.
		if (m_bIsPickup)
			return S_OK;

		Reset_LowerAnim();
		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::DOUBLE_JUMP;
		m_LowerStates.push(JUMP_DOUBLE);
		m_LowerStates.push(JUMP_LEDGE);

		Push_UpperState(UPPER_STATE::DOUBLE_JUMP);

		m_bDoubleJump = true;
		m_fJumpPower += 1.f;
		m_fJumpFinishPower = -1.f *m_fJumpPower;

		Play_Sound_Lower(LOWER_STATE::DOUBLE_JUMP);
		Play_Sound_HatKid(SOUND_STATE::JUMP);
		_float fAngle = 0.f;
		for (_uint i = 0; i < 8; ++i)
		{
			fAngle = i *35.f;

			fAngle = XMConvertToRadians(fAngle);
			CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");
			Create_Puff(m_pStuckSocket, fTimeDelta, 0, 1, 2.f, fAngle);
		}

		//m_pTransformCom->Jump(fTimeDelta, m_fJumpPower);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");

		_matrix		SocketMatrix = /*m_pTransformCom->Get_WorldMatrix() **/ m_pStuckSocket->Get_OffsetMatrix() *
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

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Puff"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &EffectInfo)))
			return E_FAIL;

		RELEASE_INSTANCE(CGameInstance);



	}


	return S_OK;
}

HRESULT CHatKid::Action_1(_float fTimeDelta)
{// Sliding

 //예외처리
	if (m_bAttacked || m_bIsPickup || m_bHurt)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->StopSound(SOUND_HATKID_LOWER);
	RELEASE_INSTANCE(CGameInstance);

	if (m_eCurrentLowerState == LOWER_STATE::DIVE)
	{//다이빙중 다시 ctrl키를 눌렀을 때 더블점프시행한다.
		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::DOUBLE_JUMP;
		Reset_LowerAnim();
		m_LowerStates.push(JUMP_DOUBLE);
		m_LowerStates.push(JUMP_LEDGE);
		Push_UpperState(UPPER_STATE::DOUBLE_JUMP);
		m_bDived = false;
		m_bCanMove = true;
		m_bDoubleJump = true;
		m_fJumpPower = 1.f;

		_float fAngle = 0.f;
		for (_uint i = 0; i < 8; ++i)
		{
			fAngle = i *35.f;

			fAngle = XMConvertToRadians(fAngle);
			CHierarchyNode* m_pStuckSocket = m_pModelCom->Get_BonePtr("bip_footL01");
			Create_Puff(m_pStuckSocket, fTimeDelta, 0, 1, 2.f, fAngle);
		}
		Play_Sound_Lower(LOWER_STATE::DOUBLE_JUMP);
		Play_Sound_HatKid(SOUND_STATE::JUMP);
		//m_LowerStates.push(JUMP_LEDGE);
	}
	else if (m_eCurrentLowerState != LOWER_STATE::DIVE &&
		m_ePreLowerState != LOWER_STATE::DIVE)
	{
		Reset_LowerAnim();
		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::DIVE;
		m_bDived = true;
		m_bDoubleJump = false;
		m_bCanMove = false;
		if (m_bJump || m_bDoubleJump)
		{//점프 중일 때의 다이빙 구현
			m_LowerStates.push(DIVE_IDLE);
			//다이빙하다가 바닥에 닿았을 시에 대한 처리 필요.
			Push_UpperState(UPPER_STATE::DIVE);
			Play_Sound_Lower(LOWER_STATE::DIVE);
		}
		else
		{//지상에 있을 때의 다이빙 구현.
			Sliding();
		}
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
			/*CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("bip_ItemPalmR01");
			if (nullptr == pSocket)
				return E_FAIL;

			EquipDesc.pSocket = pSocket;
			EquipDesc.szName = TEXT("Flask");
			EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
			EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
			Safe_AddRef(pSocket);

			if (FAILED(m_pEquipments->Add_Parts(PARTS_WEAPON, WEAPON_FLASK, &EquipDesc)))
				return E_FAIL;

			m_eWeaponType = WEAPON_FLASK;*/
			m_pEquipments->Change_Part(PARTS_SUBWEAPON, SUBWEAPON_NONE);
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

	if (!m_bCanPick)
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

			//TEXT("InteractionObject"
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CApple* pTarget = dynamic_cast<CApple*>(pGameInstance->Get_FirstObject(LEVEL_GAMEPLAY, TEXT("InteractionObject")));
			CApple::APPLEDESC AppleDesc;

			CHierarchyNode* pSocket = m_pModelCom->Get_BonePtr("bip_ItemPalmR01");
			if (nullptr == pSocket)
				return S_OK;

			AppleDesc.pSocket = pSocket;
			AppleDesc.szName = TEXT("Apple");
			AppleDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
			AppleDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
			AppleDesc.bIsDepartment = true;
			AppleDesc.fOwnerMatrix = m_pTransformCom->Get_World4x4();

			pTarget->Set_AppleDesc(&AppleDesc);
			pTarget->Set_Picked(true);
			RELEASE_INSTANCE(CGameInstance);
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

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CApple* pTarget = dynamic_cast<CApple*>(pGameInstance->Get_FirstObject(LEVEL_GAMEPLAY, TEXT("InteractionObject")));
			pTarget->Set_Picked(false);
			pTarget->Set_Throw(true);
			RELEASE_INSTANCE(CGameInstance);

			return S_OK;
		}


		m_eCurrentLowerState = LOWER_STATE::THROW;
		Reset_LowerAnim();
		m_LowerStates.push(ITEM_THROW);

		Push_UpperState(UPPER_STATE::THROW);
		//m_bIsPickup = false;
		m_bCanMove = false;
		//m_pController->Set_LockKeys(true);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CApple* pTarget = dynamic_cast<CApple*>(pGameInstance->Get_FirstObject(LEVEL_GAMEPLAY, TEXT("InteractionObject")));
		pTarget->Set_Picked(false);
		pTarget->Set_Throw(true);
		RELEASE_INSTANCE(CGameInstance);
	}


	return S_OK;
}

HRESULT CHatKid::Action_5(_float fTimeDelta)
{//키를 땠을 때의 스킬 발동. 
 //후에 모자상태에 따른 switch값 분류.
	if (m_bDived || m_bAttacked || m_bHurt)
		return S_OK;

	switch (m_eHatType)
	{
	case HAT_TYPE::HAT_WITCH:
		if (!m_bSkillUsing)
			return S_OK;

		Reset_LowerAnim();
		Play_Sound_Lower(LOWER_STATE::THROW);
		m_bIsShoot = true;
		IsShoot_Flask(m_bIsShoot);

		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::THROW;
		m_LowerStates.push(ITEM_THROW_ONEHAND);
		Push_UpperState(UPPER_STATE::THROW);
		m_bSkillUsing = false;

		//Push_UpperState()
		m_bCanMove = false;
		//m_pController->Set_LockKeys(true);
		m_eWeaponType = WEAPON_PUNCH;
		m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_PUNCH);
		m_pEquipments->Change_Part(PARTS_SUBWEAPON, SUBWEAPON_PUNCH);
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

	if (m_bJump || m_bHurt || m_bIsPickup || m_bDived || m_bDoubleJump)
		return S_OK;

	if (m_eCurrentLowerState != LOWER_STATE::ATTACK)
	{
		m_bCanMove = false;
		m_bAttacked = true;
		m_fAtkDelay = 0.f;
		m_iAtkCount = 0;
		m_pEquipments->Set_CanAttack(true);
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
		else if (m_eWeaponType == WEAPON_TYPE::WEAPON_PUNCH)
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
		else if (m_eWeaponType == WEAPON_TYPE::WEAPON_PUNCH)
		{
			m_LowerStates.push(PUNCH_A);
		}

		Push_UpperState(UPPER_STATE::ATTACK);
	}

	return S_OK;
}

HRESULT CHatKid::Idle(_float fTimeDelta)
{
	if (m_bJump || m_bDoubleJump || m_bDived || m_bHurt)
		return S_OK;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (m_eCurrentLowerState != LOWER_STATE::IDLE)
	{
		if (m_LowerStates.empty()
			|| m_AnimLoopStates[m_LowerStates.front()] == true
			)
		{
			pGameInstance->StopSound(SOUND_HATKID_LOWER);

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
	RELEASE_INSTANCE(CGameInstance);
	//나중에 점프가 바닥에 닿았을 때는 어떻게 처리할건데? 바닥에닿으면 clear해야지.

	//m_pModelCom->Set_NextAnimIndex_Upper(ITEM_CARRY_LARGE);
	//m_pModelCom->Set_NextAnimIndex_Lower(IDLE);
	return S_OK;
}

_float CHatKid::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (m_bSuper || m_bThrowing)
		return fDamage;

	if (fDamage == 0.f)
		return fDamage;

	if (!m_bHurt)
	{
		Reset_LowerAnim();
		m_bHurt = true;
		m_bCanInputKey = false;
		if (m_bSkillUsing)
		{
			m_bSkillUsing = false;
			if (m_eHatType == HAT_TYPE::HAT_WITCH)
				IsShoot_Flask(false);
		}
		Play_Sound_HatKid(SOUND_STATE::HURT);
		m_ePreLowerState = m_eCurrentLowerState;
		m_eCurrentLowerState = LOWER_STATE::HURT;
		m_LowerStates.push(HURT);
		Push_UpperState(UPPER_STATE::HURT);
		Create_HitImpact();

		if (m_iLife > 2)
		{
			--m_iLife;			
			CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
			//std::function<void(CMonster&)> func = &CMonster::Release_Super;
			pDeligate->UI_Decrease();
			RELEASE_INSTANCE(CDeligate_Monster);
		}
		else
		{

		}

	}


	return fDamage;
}

void CHatKid::TestFunc(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down('1'))
	{
		m_pEquipments->Change_Part(PARTS_HAT, HAT_DEFAULT);
		m_eHatType = HAT_TYPE::HAT_DEFAULT;
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("BadgeSwapStart.ogg"), SOUND_UI, fVolume);

	}
	else if (pGameInstance->Key_Down('2'))
	{
		m_pEquipments->Change_Part(PARTS_HAT, HAT_WITCH);
		m_eHatType = HAT_TYPE::HAT_WITCH;
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("BadgeSwapStart.ogg"), SOUND_UI, fVolume);
	}
	else if (pGameInstance->Key_Down('3'))
	{
		m_pEquipments->Change_Part(PARTS_HAT, HAT_SPRINT);
		m_eHatType = HAT_TYPE::HAT_SPRINT;
		_float fVolume = 0.7f;
		pGameInstance->PlaySounds(TEXT("BadgeSwapStart.ogg"), SOUND_UI, fVolume);
	}
	else if (pGameInstance->Key_Down('4'))
	{
		if (m_eWeaponType != WEAPON_TYPE::WEAPON_PUNCH)
		{
			m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_PUNCH);
			m_pEquipments->Change_Part(PARTS_SUBWEAPON, SUBWEAPON_PUNCH);
			m_eWeaponType = WEAPON_TYPE::WEAPON_PUNCH;
			_float fVolume = 0.7f;
			pGameInstance->PlaySounds(TEXT("BadgeSwapStart.ogg"), SOUND_UI, fVolume);
		}
		else
		{
			m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_UMBRELLA);
			m_pEquipments->Change_Part(PARTS_SUBWEAPON, SUBWEAPON_NONE);
			m_eWeaponType = WEAPON_TYPE::WEAPON_UMBRELLA;
			_float fVolume = 0.7f;
			pGameInstance->PlaySounds(TEXT("BadgeSwapStart.ogg"), SOUND_UI, fVolume);
		}
	}

	else if (pGameInstance->Key_Down('7'))
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;

		CDeligate_Monster* pDeligate = GET_INSTANCE(CDeligate_Monster);
		//std::function<void(CMonster&)> func = &CMonster::Release_Super;
		pDeligate->Clear();
		pGameInstance->StopAll();
		RELEASE_INSTANCE(CDeligate_Monster);
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

	//0번째는 장비착용 X
	if (FAILED(m_pEquipments->Add_Parts(PARTS_SUBWEAPON, SUBWEAPON_NONE)))
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

	//Test_Weapon_Punch
	pSocket = m_pModelCom->Get_BonePtr("bip_ItemPalmR01");
	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Punch");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts(PARTS_WEAPON, WEAPON_PUNCH, &EquipDesc)))
		return E_FAIL;

	//Test_SubWeapon_Punch
	pSocket = m_pModelCom->Get_BonePtr("bip_handL_middle01");
	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Punch");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts(PARTS_SUBWEAPON, SUBWEAPON_PUNCH, &EquipDesc)))
		return E_FAIL;

	//Test_SubWeapon_Punch
	pSocket = m_pModelCom->Get_BonePtr("bip_ItemPalmR01");
	if (nullptr == pSocket)
		return E_FAIL;

	EquipDesc.pSocket = pSocket;
	EquipDesc.szName = TEXT("Flask");
	EquipDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	EquipDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	Safe_AddRef(pSocket);

	if (FAILED(m_pEquipments->Add_Parts(PARTS_WEAPON, WEAPON_FLASK, &EquipDesc)))
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

	m_pEquipments->Change_Part(PARTS_SUBWEAPON, WEAPON_NONE);
	m_pEquipments->Change_Part(PARTS_WEAPON, WEAPON_UMBRELLA);
	m_pEquipments->Change_Part(PARTS_HAT, HAT_NONE);
	//Test
	m_eWeaponType = WEAPON_TYPE::WEAPON_UMBRELLA;

	//Test Dizzy
	//Create_Dizzy();



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
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	CModel::BONEINDEXDESC BoneIndexDesc;
	ZeroMemory(&BoneIndexDesc, sizeof(CModel::BONEINDEXDESC));
	/* For.Com_Model*/
	BoneIndexDesc.iUpperIndex = 0;
	BoneIndexDesc.iLowerIndex = 91;
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_HatKid"), (CComponent**)&m_pModelCom, &BoneIndexDesc)))
		return E_FAIL;
	/*if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HatKid"), (CComponent**)&m_pModelCom)))
	return E_FAIL;*/

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_SPHERE */
	m_pColliderBone = m_pModelCom->Get_BonePtr("Root");
	Safe_AddRef(m_pColliderBone);
	ColliderDesc.vScale = _float3(0.8f, 0.8f, 0.8f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;

	//LevelIndex에 따라 네비게이션 컴포넌트 설정할 것
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Navigation_Platform"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
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
	Safe_Release(m_pColliderBone);
	Safe_Release(m_pEquipments);

	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pController);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
