#include "stdafx.h"
#include "..\Public\Camera_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
{
}

CCamera_Dynamic * CCamera_Manager::Get_Camera()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr == pGameInstance)
		return nullptr;

	CCamera_Dynamic* pTarget = dynamic_cast<CCamera_Dynamic*>(pGameInstance->Get_FirstObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera")));

	RELEASE_INSTANCE(CGameInstance);

	return pTarget;
}

void CCamera_Manager::Set_CameraState(_uint iCameraState)
{
	Get_Camera()->Set_CameState((CCamera_Dynamic::CAM_STATE) iCameraState);
}

void CCamera_Manager::Free(void)
{
}
