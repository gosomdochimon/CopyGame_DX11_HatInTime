#include "stdafx.h"
#include "..\Public\PlayerController.h"
#include "Player.h"
#include "GameInstance.h"
CPlayerController::CPlayerController()
{
}

HRESULT CPlayerController::Initialize(CPlayer * pPlayer)
{
	if (pPlayer == nullptr)
		return E_FAIL;

	m_pOwner = pPlayer;

	if (FAILED(Setting_Key()))
		return E_FAIL;

	return S_OK;
}

void CPlayerController::Set_LockKeys(_bool bIsLock)
{
	if (bIsLock)
	{
		for (auto& iter = m_KeySets.begin(); iter != m_KeySets.end(); iter++)
		{
			iter->second.bCanUse = false;
		}
	}
	else
	{
		for (auto& iter = m_KeySets.begin(); iter != m_KeySets.end(); iter++)
		{
			iter->second.bCanUse = true;
		}
	}
}

HRESULT CPlayerController::Setting_Key(void)
{
	pair<ACTIONKEY, COMMANDDESC> KeyMoveFront;
	KeyMoveFront.first = ACTIONKEY::MOVE_FRONT;
	KeyMoveFront.second.Key = 'W';
	KeyMoveFront.second.pCommand = Front_Command = new CFront_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyMoveBack;
	KeyMoveBack.first = ACTIONKEY::MOVE_BACK;
	KeyMoveBack.second.Key = 'S';
	KeyMoveBack.second.pCommand = Back_Command = new CBack_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyMoveLeft;
	KeyMoveLeft.first = ACTIONKEY::MOVE_LEFT;
	KeyMoveLeft.second.Key = 'A';
	KeyMoveLeft.second.pCommand = Left_Command = new CLeft_Command();
	
	pair<ACTIONKEY, COMMANDDESC> KeyMoveRight;
	KeyMoveRight.first = ACTIONKEY::MOVE_RIGHT;
	KeyMoveRight.second.Key = 'D';
	KeyMoveRight.second.pCommand = Right_Command = new CRight_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyMoveJump;
	KeyMoveJump.first = ACTIONKEY::JUMP;
	KeyMoveJump.second.Key = VK_SPACE;
	KeyMoveJump.second.pCommand = Jump_Command = new CJump_Command();

	pair<ACTIONKEY, COMMANDDESC> KeySlide;
	KeySlide.first = ACTIONKEY::SLIDING;
	KeySlide.second.Key = VK_CONTROL;
	KeySlide.second.pCommand = Sliding_Command = new CAction1_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyPressSkill;
	KeyPressSkill.first = ACTIONKEY::SKILLDOWN;
	KeyPressSkill.second.Key = VK_SHIFT;
	KeyPressSkill.second.pCommand = PressSkill_Command = new CAction2_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyUpSkill;
	KeyUpSkill.first = ACTIONKEY::SKILLUP;
	KeyUpSkill.second.Key = VK_SHIFT;
	KeyUpSkill.second.pCommand = UpSkill_Command = new CAction5_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyInventory;
	KeyInventory.first = ACTIONKEY::INVENTORY;
	KeyInventory.second.Key = VK_RBUTTON;
	KeyInventory.second.pCommand = Inventory_Command = new CAction3_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyInteraction;
	KeyInteraction.first = ACTIONKEY::INTERACTION;
	KeyInteraction.second.Key = 'G';
	KeyInteraction.second.pCommand = Interaction_Command = new CAction4_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyTest;
	KeyTest.first = ACTIONKEY::LBUTTON;
	KeyTest.second.Key = VK_LBUTTON;
	KeyTest.second.pCommand = Test_Command = new CAction6_Command();

	m_KeySets.insert(KeyMoveFront);
	m_KeySets.insert(KeyMoveBack);
	m_KeySets.insert(KeyMoveRight);
	m_KeySets.insert(KeyMoveLeft);
	m_KeySets.insert(KeyMoveJump);
	m_KeySets.insert(KeySlide);
	m_KeySets.insert(KeyPressSkill);
	m_KeySets.insert(KeyUpSkill);
	m_KeySets.insert(KeyInteraction);
	m_KeySets.insert(KeyInventory);
	m_KeySets.insert(KeyTest);
	return S_OK;
}

void CPlayerController::Input_Controller(_float fTImeDelta)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pOwner == nullptr)
		return;

	if (m_pOwner->Get_CanInputKey())//전체적인 액션키 봉쇄.
	{
		if (pGameInstance->Key_Down(m_KeySets[ACTIONKEY::SLIDING].Key)
			&& m_KeySets[ACTIONKEY::SLIDING].bCanUse == true)
		{
			m_KeySets[ACTIONKEY::SLIDING].pCommand->Excute(m_pOwner, fTImeDelta);
		}
		else if (pGameInstance->Key_Down(m_KeySets[ACTIONKEY::LBUTTON].Key)
			&& m_KeySets[ACTIONKEY::LBUTTON].bCanUse == true)
		{
			m_KeySets[ACTIONKEY::LBUTTON].pCommand->Excute(m_pOwner, fTImeDelta);
		}


		if (pGameInstance->Key_Down(m_KeySets[ACTIONKEY::INTERACTION].Key)
			&& m_KeySets[ACTIONKEY::INTERACTION].bCanUse == true)
		{
			m_KeySets[ACTIONKEY::INTERACTION].pCommand->Excute(m_pOwner, fTImeDelta);
		}

		if (pGameInstance->Key_Up(m_KeySets[ACTIONKEY::SKILLUP].Key)
			&& m_KeySets[ACTIONKEY::SKILLUP].bCanUse == true)
		{
			m_KeySets[ACTIONKEY::SKILLUP].pCommand->Excute(m_pOwner, fTImeDelta);
		}
		else if (pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::SKILLDOWN].Key)
			&& m_KeySets[ACTIONKEY::SKILLDOWN].bCanUse == true)
		{

			m_KeySets[ACTIONKEY::SKILLDOWN].pCommand->Excute(m_pOwner, fTImeDelta);
		}

		if (pGameInstance->Key_Down(m_KeySets[ACTIONKEY::JUMP].Key)
			&& m_KeySets[ACTIONKEY::JUMP].bCanUse == true)
		{
			m_KeySets[ACTIONKEY::JUMP].pCommand->Excute(m_pOwner, fTImeDelta);
		}

		if (m_pOwner->Get_CanMove()) //움직임 봉쇄용.
		{
			if (pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::MOVE_FRONT].Key)
				&& m_KeySets[ACTIONKEY::MOVE_FRONT].bCanUse == true)
			{
				if (pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::MOVE_LEFT].Key))
					m_pOwner->Move_LeftFront(fTImeDelta);
				else if(pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::MOVE_RIGHT].Key))
					m_pOwner->Move_RightFront(fTImeDelta);
				else
 					m_KeySets[ACTIONKEY::MOVE_FRONT].pCommand->Excute(m_pOwner, fTImeDelta);
			}
			else if (pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::MOVE_BACK].Key)
				&& m_KeySets[ACTIONKEY::MOVE_BACK].bCanUse == true)
			{
				if (pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::MOVE_LEFT].Key))
					m_pOwner->Move_LeftBack(fTImeDelta);
				else if (pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::MOVE_RIGHT].Key))
					m_pOwner->Move_RightBack(fTImeDelta);
				else
					m_KeySets[ACTIONKEY::MOVE_BACK].pCommand->Excute(m_pOwner, fTImeDelta);
			}
			else if (pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::MOVE_LEFT].Key)
				&& m_KeySets[ACTIONKEY::MOVE_LEFT].bCanUse == true)
			{
				m_KeySets[ACTIONKEY::MOVE_LEFT].pCommand->Excute(m_pOwner, fTImeDelta);
			}
			else if (pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::MOVE_RIGHT].Key)
				&& m_KeySets[ACTIONKEY::MOVE_RIGHT].bCanUse == true)
			{
				m_KeySets[ACTIONKEY::MOVE_RIGHT].pCommand->Excute(m_pOwner, fTImeDelta);
			}
			else
			{
				m_pOwner->Idle(fTImeDelta);
			}
		}
		else
		{
			m_pOwner->Idle(fTImeDelta);
		}

	}
	
	
	//Menu&Inventory키는 항상 작동해야함.
	if (pGameInstance->Key_Down(m_KeySets[ACTIONKEY::INVENTORY].Key)
		&& m_KeySets[ACTIONKEY::INVENTORY].bCanUse == true)
	{
		m_KeySets[ACTIONKEY::INVENTORY].pCommand->Excute(m_pOwner, fTImeDelta);
	}

	Safe_Release(pGameInstance);
}

//void CPlayerController::Check_PlayerState()
//{
//}

void CPlayerController::Free()
{
	for (auto& iter : m_KeySets)
	{
		Safe_Release(iter.second.pCommand);
	}
	m_KeySets.clear();
}

