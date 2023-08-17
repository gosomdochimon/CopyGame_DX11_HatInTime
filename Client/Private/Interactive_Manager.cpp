#include "stdafx.h"
#include "..\Public\Interactive_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CInteractive_Manager)

CInteractive_Manager::CInteractive_Manager()
{
}

list<CGameObject*>* CInteractive_Manager::Get_Interactive_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*>* pObj = (pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("InteractionObject")));

	RELEASE_INSTANCE(CGameInstance);
	
	return pObj;
}

CHatKid * CInteractive_Manager::Get_HatKid()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr == pGameInstance)
		return nullptr;

	CHatKid* pTarget = dynamic_cast<CHatKid*>(pGameInstance->Get_FirstObject(LEVEL_STATIC, TEXT("Layer_Player")));

	RELEASE_INSTANCE(CGameInstance);

	return pTarget;
}

_vector CInteractive_Manager::Get_HatKid_Pos()
{
	return Get_HatKid()->Get_State(CTransform::STATE_TRANSLATION);
}

void CInteractive_Manager::Set_Bossptr(CGameObject * pBoss)
{
	CMafiaBoss* pTarget = dynamic_cast<CMafiaBoss*>(pBoss);
	m_pBoss = pTarget;
}

CMafiaBoss * CInteractive_Manager::Get_Boss()
{
	return m_pBoss;
}

_vector CInteractive_Manager::Get_Boss_Pos(_uint iPosType)
{
	if (iPosType == 0)
		return m_pBoss->Get_State(CTransform::STATE_TRANSLATION);
	else if (iPosType == 1)
	{
		_float4x4 BoneMat = m_pBoss->Get_BonePtrMatrix("Boss01");
		//EffectInfo.vPosition = (_float3)&m_CombinedWorldMatrix.m[3][0];

		return XMLoadFloat3(&((_float3)&BoneMat.m[3][0]));
	}
}

void CInteractive_Manager::Free(void)
{
}
