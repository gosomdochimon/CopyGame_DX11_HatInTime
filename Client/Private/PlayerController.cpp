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

HRESULT CPlayerController::Setting_Key(void)
{
	pair<ACTIONKEY, COMMANDDESC> KeyMoveFront;
	KeyMoveFront.first = ACTIONKEY::MOVE_FRONT;
	KeyMoveFront.second.Key = VK_UP;
	KeyMoveFront.second.pCommand = Front_Command = new CFront_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyMoveBack;
	KeyMoveBack.first = ACTIONKEY::MOVE_BACK;
	KeyMoveBack.second.Key = VK_DOWN;
	KeyMoveBack.second.pCommand = Back_Command = new CBack_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyMoveLeft;
	KeyMoveLeft.first = ACTIONKEY::MOVE_LEFT;
	KeyMoveLeft.second.Key = VK_LEFT;
	KeyMoveLeft.second.pCommand = Left_Command = new CLeft_Command();
	
	pair<ACTIONKEY, COMMANDDESC> KeyMoveRight;
	KeyMoveRight.first = ACTIONKEY::MOVE_RIGHT;
	KeyMoveRight.second.Key = VK_RIGHT;
	KeyMoveRight.second.pCommand = Right_Command = new CRight_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyMoveJump;
	KeyMoveJump.first = ACTIONKEY::JUMP;
	KeyMoveJump.second.Key = VK_SPACE;
	KeyMoveJump.second.pCommand = Jump_Command = new CJump_Command();

	pair<ACTIONKEY, COMMANDDESC> KeySlide;
	KeySlide.first = ACTIONKEY::SLIDING;
	KeySlide.second.Key = VK_CONTROL;
	KeySlide.second.pCommand = Sliding_Command = new CAction1_Command();

	pair<ACTIONKEY, COMMANDDESC> KeySkill;
	KeySkill.first = ACTIONKEY::SKILL1;
	KeySkill.second.Key = VK_SHIFT;
	KeySkill.second.pCommand = Skill_Command = new CAction2_Command();

	pair<ACTIONKEY, COMMANDDESC> KeyInventory;
	KeyInventory.first = ACTIONKEY::INVENTORY;
	KeyInventory.second.Key = VK_RBUTTON;
	KeyInventory.second.pCommand = Inventory_Command = new CAction3_Command();

	m_KeySets.insert(KeyMoveFront);
	m_KeySets.insert(KeyMoveBack);
	m_KeySets.insert(KeyMoveRight);
	m_KeySets.insert(KeyMoveLeft);
	m_KeySets.insert(KeyMoveJump);
	m_KeySets.insert(KeySlide);
	m_KeySets.insert(KeySkill);
	m_KeySets.insert(KeyInventory);
	return S_OK;
}

void CPlayerController::Input_Controller(_float fTImeDelta)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pOwner == nullptr)
		return;

	if (pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::MOVE_FRONT].Key) 
		&& m_KeySets[ACTIONKEY::MOVE_FRONT].bCanUse == true)
	{
		m_KeySets[ACTIONKEY::MOVE_FRONT].pCommand->Excute(m_pOwner, fTImeDelta);
	}
	else if (pGameInstance->Key_Pressing(m_KeySets[ACTIONKEY::MOVE_BACK].Key)
		&& m_KeySets[ACTIONKEY::MOVE_BACK].bCanUse == true)
	{
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
	Safe_Release(pGameInstance);

}

void CPlayerController::Check_PlayerState()
{
}

void CPlayerController::Free()
{
	for (auto& iter : m_KeySets)
	{
		Safe_Release(iter.second.pCommand);
	}
	m_KeySets.clear();
}

