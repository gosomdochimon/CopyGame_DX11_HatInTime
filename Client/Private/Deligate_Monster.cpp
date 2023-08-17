#include "stdafx.h"
#include "..\Public\Deligate_Monster.h"
#include "Interactive_Manager.h"
#include "HatKid.h"
#include "GameInstance.h"
#include "DataManager.h"
#include "MafiaBoss.h"

IMPLEMENT_SINGLETON(CDeligate_Monster)

CDeligate_Monster::CDeligate_Monster()
{
}

void CDeligate_Monster::Add_SuperEvent(NOTIFY_TYPE eType, CMonster * pMonster, void(CMonster::*pFunc)(void))
{//계속 다른값이 들어온다. 뭘까 

	m_vSupers.push_back(pMonster);
}

HRESULT CDeligate_Monster::Notify_SuperEvent()
{
	for (auto& iter = m_vSupers.begin(); iter != m_vSupers.end();)
	{
		if (*iter != nullptr)
		{
			(*iter)->Release_Super();
		}
		iter++;
	}
	m_vSupers.clear();
	return S_OK;
}

void CDeligate_Monster::Add_BossEvent(NOTIFY_TYPE eType, CMonster * pMonster)
{
	m_Boss.push_back(pMonster);

}

void CDeligate_Monster::Delete_BossEvent(CMonster * pMonster)
{
	m_Boss.clear();
}

HRESULT CDeligate_Monster::Notify_Boss1Event()
{
	m_Boss[0]->Set_FightMode(true);

	return S_OK;
}

void CDeligate_Monster::Add_StaticMeshEvent(CStaticMesh * pStatic)
{
	m_StaticMeshs.push_back(pStatic);
}

void CDeligate_Monster::Delete_StaticEvent(CStaticMesh * pStatic)
{
	m_StaticMeshs.clear();
}

HRESULT CDeligate_Monster::Notify_StaticEvent()
{
	m_StaticMeshs[0]->Notify();
	return S_OK;
}

void CDeligate_Monster::Add_NPC(CPawn * pPawn)
{
	m_NPC.push_back(pPawn);
}

void CDeligate_Monster::Delete_NPC()
{
	m_NPC.clear();
}

void CDeligate_Monster::Add_Dizzys(CEffect * pEffect)
{
	m_Effects.push_back(pEffect);
}

void CDeligate_Monster::Delete_Dizzys()
{


}

void CDeligate_Monster::Notify_DizzyEffect()
{
	for (auto& iter = m_Effects.begin(); iter != m_Effects.end();)
	{
		if (*iter != nullptr)
		{
			(*iter)->Set_DeleteObject();
		}
		iter++;
	}
	m_Effects.clear();
}

void CDeligate_Monster::Add_ArriveEvent()
{
	++m_iArriveCnt;
	//printf_s("%d \n", m_iArriveCnt);
}

void CDeligate_Monster::Notify_MafiaArrvie()
{
	++m_iCurrentArr_Cnt;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
	{
		if (m_iArriveCnt <= m_iCurrentArr_Cnt)
		{
			CInteractive_Manager* pInter = GET_INSTANCE(CInteractive_Manager);

			CHatKid* Kid = pInter->Get_HatKid();
			CNavigation*	pNavi = (CNavigation*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Navigation_GamePlay"));
			if (nullptr == pNavi)
			{
				RELEASE_INSTANCE(CGameInstance);
				RELEASE_INSTANCE(CInteractive_Manager);
				return;
			}


			pNavi->Change_CellType_Group(0, 41, 1);

			Kid->Set_CanMove(true);

			//RELEASE_INSTANCE(CGameInstance);
			RELEASE_INSTANCE(CInteractive_Manager);
			//printf_s("Can Move \n");
			m_iCurrentArr_Cnt = 0;
			//m_iArriveCnt = 0;
		}
	}
	else
	{
		if (m_iArriveCnt <= m_iCurrentArr_Cnt)
		{

		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CDeligate_Monster::Notify_MafiaDead()
{
	--m_iArriveCnt;
	if (m_iArriveCnt <= 0)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


		if (pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY_PLATFORM)
		{
			Notify_NPC();
		}
		else
		{
			Notify_EliteMafia_Open();
		}


		RELEASE_INSTANCE(CGameInstance);
	}
}

void CDeligate_Monster::Notify_EliteMafia_Open(_uint iTypeNum)
{
	CActor::CLONEINFO Info;
	m_iArriveCnt += 2; 
	CDataManager* pDataMgr = GET_INSTANCE(CDataManager);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	Info = pDataMgr->Get_Tu3_Data()[0];
	Info.fRot_Value = pDataMgr->Get_ArrivePoints()[6];
	Info.iTypeNum = iTypeNum;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MafiaElite"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &Info)))
		return;

	Info = pDataMgr->Get_Tu3_Data()[1];
	Info.fRot_Value = pDataMgr->Get_ArrivePoints()[7];
	Info.iTypeNum = iTypeNum;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MafiaElite"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &Info)))
		return;

	RELEASE_INSTANCE(CDataManager);
	RELEASE_INSTANCE(CGameInstance);

}

void CDeligate_Monster::Notify_MafiaEliteDead()
{
	--m_iArriveCnt;
	if (m_iArriveCnt <= 0)
	{
		Notify_Boss1Event();
	}
}

void CDeligate_Monster::UI_Decrease()
{
	for (auto& iter = m_UIs.begin(); iter != m_UIs.end();)
	{
		if (*iter != nullptr)
		{
			(*iter)->Decrease();
		}
		iter++;
	}

}

void CDeligate_Monster::Clear()
{
	void	Delete_NPC();
	m_iArriveCnt = 0;
	m_iCurrentArr_Cnt = 0;
}

void CDeligate_Monster::Notify_NPC()
{
	for (auto& iter = m_NPC.begin(); iter != m_NPC.end();)
	{
		if (*iter != nullptr)
		{
			(*iter)->Functor();
		}
		iter++;
	}
	//m_NPC.clear();
}

CPawn * CDeligate_Monster::Get_Npc()
{
	return m_NPC[0];
}

void CDeligate_Monster::Add_EnemyColider(CMonster * pOwner, CCollider * pColider)
{
	Deligate_ColDesc Desc;
	Desc.pMonster = pOwner;
	Desc.pCollider = pColider;
	m_ColDesc.push_back(Desc);
	Safe_AddRef(pOwner);

}





void CDeligate_Monster::Free(void)
{
	m_Effects.clear();
	Delete_BossEvent();
}

