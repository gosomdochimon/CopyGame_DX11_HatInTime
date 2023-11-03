#include "stdafx.h"
#include "..\Public\Level_Platform.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "DataManager.h"
#include "Effect.h"



CLevel_Platform::CLevel_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Platform::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;



	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_InteractionObject(TEXT("InteractionObject"))))
	//	return E_FAIL;


	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;	

	//if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
	//	return E_FAIL;

	ShowCursor(false);

	return S_OK;
}

void CLevel_Platform::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


}

void CLevel_Platform::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
}

HRESULT CLevel_Platform::Ready_Lights()
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

HRESULT CLevel_Platform::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HatKid"), LEVEL_STATIC, pLayerTag, nullptr)))
		return E_FAIL;

	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLevel_Platform::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//for (_uint i = 0; i < 10; ++i)
	{
		/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mafia"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
			return E_FAIL;*/
	}
	//MafiaBoss
	{
		/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MafiaBoss"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
			return E_FAIL;*/
	}


	//for (_uint i = 0; i < 3; ++i)
	{
		/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MadCrow"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
			return E_FAIL;*/
	}

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sandback"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
	//	return E_FAIL;

	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLevel_Platform::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	CDataManager*			pDataManager = CDataManager::Get_Instance();
	Safe_AddRef(pDataManager);

	list<CActor::tagCloneInfo> Info = pDataManager->Get_PlatformAllTypeData();

	for (auto& iter = Info.begin(); iter != Info.end();)
	{
		switch(iter->iModelNum)
		{
		case 2:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MafiaGuard"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_Monster"), &(*iter))))
			return E_FAIL;
			break;

		case 4: //map
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Platform"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, &(*iter))))
				return E_FAIL;
			break;
		case 5://poly
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Platform"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, &(*iter))))
				return E_FAIL;
			break;
		case 6://carpet
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Platform"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, &(*iter))))
				return E_FAIL;
			break;
		case 7: // Button
			/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Button"), LEVEL_GAMEPLAY_PLATFORM, TEXT("InteractionObject"), &(*iter))))
				return E_FAIL;*/
			break;
		case 8: //ParmTree
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Platform"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, &(*iter))))
				return E_FAIL;
			break;
		case 9: //Clerk
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Clerk"), LEVEL_GAMEPLAY_PLATFORM, TEXT("InteractionObject"), &(*iter))))
				return E_FAIL;
			break;
		case 10: //Bars
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bars"), LEVEL_GAMEPLAY_PLATFORM, TEXT("InteractionObject"), &(*iter))))
				return E_FAIL;
			break;
		}
		iter++;
	}

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Platform"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag)))
		return E_FAIL;
	

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
	//	return E_FAIL;

	Safe_Release(pGameInstance);
	Safe_Release(pDataManager);
	return S_OK;
}

HRESULT CLevel_Platform::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
		return E_FAIL;*/

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
			return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Platform::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*for (_uint i = 0; i < 20; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Effect"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
			return E_FAIL;
	}*/
	
	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Particle_Point"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HitEffect"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
	//	return E_FAIL;
	//Prototype_GameObject_Ring

	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Ring"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_StarEffect"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag)))
		return E_FAIL;*/
	CEffect::EFFECTINFO EffectInfo;
	EffectInfo.iTypeNum = 1;
	EffectInfo.iMoveType = 0;
	EffectInfo.fAngle = 0.f;
	EffectInfo.fLifeTime = 0.f;
	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_RotationEffect"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, &EffectInfo)))
	//	return E_FAIL;
	EffectInfo.iTypeNum = 0;
	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_RotationEffect"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, &EffectInfo)))
		return E_FAIL; */

	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DeadSphere"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, nullptr)))
		return E_FAIL;*/

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Platform::Ready_Layer_Camera(const _tchar * pLayerTag)
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

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Platform::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	/*Health_UI*/
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_PlayerHealth"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_TextLife"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Arrow"), LEVEL_STATIC, pLayerTag)))
	//	return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Platform::Ready_Layer_InteractionObject(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Apple"), LEVEL_GAMEPLAY_PLATFORM, pLayerTag)))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dweller"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_Test"))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SubConEye"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_SubConEye"))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Trigger_Box"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_TriggerBox"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Throne"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Layer_StaticMesh"))))
		return E_FAIL;
	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Platform * CLevel_Platform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Platform*	pInstance = new CLevel_Platform(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_Platform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Platform::Free()
{
	__super::Free();


}
