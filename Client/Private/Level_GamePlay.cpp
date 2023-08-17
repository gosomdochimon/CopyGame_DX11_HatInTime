#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "DataManager.h"
#include "Effect.h"
CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;*/



	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_InteractionObject(TEXT("InteractionObject"))))
		return E_FAIL;


	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;	

	//if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
	//	return E_FAIL;



	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;

	/* For.Directional*/
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	///* For.Point */
	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(10.f, 3.f, 10.f, 1.f);
	//LightDesc.fRange = 7.f;	
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HatKid"), LEVEL_STATIC, pLayerTag, nullptr)))
		return E_FAIL;

	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//for (_uint i = 0; i < 10; ++i)
	{
		/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mafia"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
			return E_FAIL;*/
	}
	//MafiaBoss
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MafiaBoss"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
			return E_FAIL;
	}


	//for (_uint i = 0; i < 3; ++i)
	{
		/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MadCrow"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
			return E_FAIL;*/
	}

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sandback"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
	//	return E_FAIL;

	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	CDataManager*			pDataManager = CDataManager::Get_Instance();
	Safe_AddRef(pDataManager);

	list<CActor::tagCloneInfo> Info = pDataManager->Get_TutorialAllTypeData();

	for (auto& iter = Info.begin(); iter != Info.end();)
	{
		switch(iter->iModelNum)
		{
		case 0:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mafia_HQ"), LEVEL_GAMEPLAY, pLayerTag, &(*iter))))
				return E_FAIL;
			break;
		case 1:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mafia_HQ_Dome"), LEVEL_GAMEPLAY, pLayerTag, &(*iter))))
				return E_FAIL;
			break;
		case 2:
			/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mafia"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &(*iter))))
				return E_FAIL;*/
			break;

		}
		iter++;
	}

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Platform"), LEVEL_GAMEPLAY, pLayerTag)))
	//	return E_FAIL;
	

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
	//	return E_FAIL;

	Safe_Release(pGameInstance);
	Safe_Release(pDataManager);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
		return E_FAIL;*/

		//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
		//	return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*for (_uint i = 0; i < 20; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Effect"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
			return E_FAIL;
	}*/
	
	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Particle_Point"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HitEffect"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
	//	return E_FAIL;
	//Prototype_GameObject_Ring

	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Ring"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_StarEffect"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;*/
	CEffect::EFFECTINFO EffectInfo;
	EffectInfo.iTypeNum = 1;
	EffectInfo.iMoveType = 0;
	EffectInfo.fAngle = 0.f;
	EffectInfo.fLifeTime = 0.f;
	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_RotationEffect"), LEVEL_GAMEPLAY, pLayerTag, &EffectInfo)))
	//	return E_FAIL;
	EffectInfo.iTypeNum = 0;
	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_RotationEffect"), LEVEL_GAMEPLAY, pLayerTag, &EffectInfo)))
		return E_FAIL; */

	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DeadSphere"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
		return E_FAIL;*/

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera_Dynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera_Dynamic::CAMERADESC_DERIVED));

	CameraDesc.iTest = 10;

	CameraDesc.CameraDesc.vEye = _float4(0.f, 10.0f, -10.f, 1.f);
	CameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	CameraDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 500.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	/*Health_UI*/
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_PlayerHealth"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_TextLife"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Arrow"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_InteractionObject(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Apple"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;*/

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dweller"), LEVEL_GAMEPLAY, TEXT("Layer_Test"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SubConEye"), LEVEL_GAMEPLAY, TEXT("Layer_SubConEye"))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Trigger_Box"), LEVEL_GAMEPLAY, TEXT("Layer_TriggerBox"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Throne"), LEVEL_GAMEPLAY, TEXT("Layer_StaticMesh"))))
		return E_FAIL;
	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();


}
