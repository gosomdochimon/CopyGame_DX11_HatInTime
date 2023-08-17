#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"


#include "Camera_Dynamic.h"
#include "BackGround.h"
#include "Terrain.h"
#include "HatKid.h"
#include "Umbrella.h"
#include "Flask.h"
#include "Punch.h"
#include "Throwing_Object.h"
#include "Blade.h"
#include "Hat.h"

#include "Clerk.h"
#include "Button.h"
#include "Bars.h"

#include "Mafia.h"
#include "MafiaElite.h"
#include "MadCrow.h"
#include "MafiaBoss.h"
#include "Mafia_HQ.h"
#include "Mafia_HQ_Dome.h"
#include "Apple.h"
#include "Sandback.h"
#include "Boss_Bullet.h"

#include "Dweller.h"
#include "SubConEye.h"
#include "Throne.h"
#include "Trigger_Box.h"

#include "Mafia_Platform.h"
#include "Mafia_Guard.h"
//Effect
#include "Particle_Point.h"
#include "Puff.h"
#include "Ring.h"
#include "Crown.h"
#include "Rotation_Effect.h"
#include "DeadSphere.h"

#include "HitImpact.h"
#include "Dizzy_Effect.h"
#include "Star.h"
#include "Talk.h"
#include "Sky.h"
//UI
#include "UI_Base.h"
#include "UI_Arrow.h"
#include "UI_PlayerHealth.h"
#include "UI_TextLife.h"
#include "LogoTexture.h"
#include "Start_UI.h"
//#include "Effect.h"
//#include "Sky.h"
//#include "UI.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY Thread_Main(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY_PLATFORM:
		pLoader->Loading_ForGamePlay_Platform();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	CoInitializeEx(nullptr, 0);

	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/* 텍스쳐 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중."));

	if (FAILED(Loading_ForStatic_Texture()))
		return E_FAIL;


	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 로딩 중."));

	if (FAILED(Loading_ForStatic_Model()))
		return E_FAIL;

	/* 셰이더 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중."));

	if (FAILED(Loading_ForStatic_Shader()))
		return E_FAIL;

	// CShader::Create(m_pDevice, m_pContext, TEXT(""), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);


	/*For.Prototype_Component_Texture_Explosion */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	/* 객체 원형 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("객체 생성 중."));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Loading_ForStatic_Object
	if (FAILED(Loading_ForPrototype_Object()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay_Platform()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 로딩 중."));

	_matrix			PivotMatrix = XMMatrixIdentity();

	/*For.Prototype_Component_Model_Mafia_Platform*/
	PivotMatrix = XMMatrixIdentity();
	//PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_Mafia_Platform"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Platform/Mafia_Platform.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Mafia_Poly*/
	PivotMatrix = XMMatrixIdentity();
	//PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_Mafia_Poly"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Platform/Mafia_hq_Poly.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Carpet*/
	PivotMatrix = XMMatrixIdentity();
	//PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_Carpet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Platform/Carpet.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_ParmTrees*/
	PivotMatrix = XMMatrixIdentity();
	//PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_ParmTrees"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Platform/ParmTrees.fbx", PivotMatrix))))
		return E_FAIL;

	///*For.Prototype_Component_Model_Statue*/
	//PivotMatrix = XMMatrixIdentity();
	////PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_Statue"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Platform/Statue.fbx", PivotMatrix))))
	//	return E_FAIL;

	/*For.Prototype_Component_Model_Clerk*/
	PivotMatrix = XMMatrixIdentity();
	//PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_Clerk"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Anim/Clerk/CoffeeClerk.fbx", PivotMatrix))))
		return E_FAIL;

	/* 텍스쳐 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중."));


	lstrcpy(m_szLoadingText, TEXT("네비게이션 생성 ㅈ2ㅜㅇ."));
	/* For.Prototype_Component_Navigation */

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Navigation_Platform"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navi/Platform_Navi.dat")))))
		return E_FAIL;

	/*For.Prototype_GameObject_MafiaGuard*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MafiaGuard"),
		CMafia_Guard::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

	/*For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//pGameInstance->Get_CurrentLevelIndex()
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;


	return S_OK;
}



HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	

	/* 텍스쳐 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중."));

	///*For.Prototype_Component_Texture_UI */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	//	return E_FAIL;

	/*For.Prototype_Component_Texture_Terrain*/
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
	return E_FAIL;*/

	/*For.Prototype_Component_Texture_Brush */
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
	return E_FAIL;*/

	/*For.Prototype_Component_Texture_Filter */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), 1))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.dds"), 1))))
	//	return E_FAIL;

	///*For.Prototype_Component_Texture_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	///*For.Prototype_Component_Texture_Explosion */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;	
	//

	/*For.Prototype_GameObject_Effect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HitEffect"),
		CHitImpact::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/*For.Prototype_Component_Texture_Snow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/snow.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_checkpoint_stars_fg */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_chkpt_stars_fg"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ui/hit/checkpoint_stars_fg.dds"), 1))))
		return E_FAIL;

	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 로딩 중."));

	///*For.Prototype_Component_VIBuffer_Terrain*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
	//	return E_FAIL;

	/////*For.Prototype_Component_Model_Fiona*/
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Fiona/Fiona.fbx", PivotMatrix))))
	//	return E_FAIL;

	if (FAILED(Loading_ForModel_Play1()))
		return E_FAIL;

	_matrix			PivotMatrix = XMMatrixIdentity();

	///*For.Prototype_Component_Model_ForkLift*/
	//_float3 scale = _float3(0.01f, 0.01f, 0.01f);
	//PivotMatrix = /*XMMatrixTransformation(XMLoadFloat3(&scale),)*/XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/ForkLift/ForkLift.fbx", PivotMatrix))))
	//	return E_FAIL;

	/*Monster 모델 생성*/
	/*For.Prototype_Component_Model_MafiaBoss*/

	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MafiaBoss"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Enemy/MafiaBoss/MafiaBoss.fbx", PivotMatrix))))
		return E_FAIL;
	PivotMatrix = XMMatrixIdentity();

	/*For.Prototype_Component_Model_MafiaElite*/
	PivotMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MafiaElite"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Enemy/Mafia_Elite/Mafia_Elite.fbx", PivotMatrix))))
		return E_FAIL;

	

	/*For.Prototype_Component_Model_Sandback*/
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f))/** XMMatrixTranslation(0.f, 0.7f, 0.f)*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sandback"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Anim/Sandback/Sandback.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_BossBullet1*/
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*//** XMMatrixTranslation(0.f, 0.7f, 0.f)*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossBullet1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Thunder_Shock/Thunder_Shock1.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_BossBullet2*/
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*//** XMMatrixTranslation(0.f, 0.7f, 0.f)*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossBullet2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Thunder_Shock/Thunder_Shock2.fbx", PivotMatrix))))
		return E_FAIL;

	/*NPC Model Create*/
	/*For.Prototype_Component_Model_ForestDweller*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*//** XMMatrixTranslation(0.f, 0.7f, 0.f)*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForestDweller"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Anim/forestdweller/ForestDweller.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_SubConEye*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*//** XMMatrixTranslation(0.f, 0.7f, 0.f)*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SubConEye"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Anim/SubconEye/Idle.fbx", PivotMatrix))))
		return E_FAIL;

	/*Map 모델 생성*/
	/*For.Prototype_Component_Model_MafiaBossHQ*/
	PivotMatrix = XMMatrixIdentity();
	//PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mafia_HQ"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Map/Mafia_HQ.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Mafia_HQ_Dome*/
	//PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * /*XMMatrixRotationY(XMConvertToRadians(180.0f))**/ XMMatrixTranslation(0.f, 0.0f, 0.f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mafia_HQ_Dome"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Map/Mafia_HQ_Dome.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Throne*/
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * /*XMMatrixRotationY(XMConvertToRadians(180.0f))**/ XMMatrixTranslation(0.f, 0.0f, 0.f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Throne"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Throne/Mafia_Throne.fbx", PivotMatrix))))
		return E_FAIL;



	

	/* 셰이더 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중."));

	

	lstrcpy(m_szLoadingText, TEXT("네비게이션 생성 ㅈ2ㅜㅇ."));
	/* For.Prototype_Component_Navigation */

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navi/Mafia_HQ_Navigation_Data.dat")))))
		return E_FAIL;

	/* 객체 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("객체 생성 중."));

	/*For.Prototype_GameObject_MafiaBosss*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MafiaBoss"),
		CMafiaBoss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Sandback*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sandback"),
		CSandback::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*Map Object 생성*/
	/*For.Prototype_GameObject_Mafia_HQ*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mafia_HQ"),
		CMafia_HQ::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mafia_HQ_Dome"),
		CMafia_HQ_Dome::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	///*For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///*For.Prototype_GameObject_Player*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
	//	CPlayer::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	
	///*For.Prototype_GameObject_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pGraphic_Device))))
	//	return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay2Level()
{
	return E_NOTIMPL;
}

HRESULT CLoader::Loading_ForStatic_Model()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	_matrix			PivotMatrix = XMMatrixIdentity();

	/*For.Prototype_Component_Model_Hatkid*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_HatKid"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/HatKid/HatKidAnim.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Umbrella*/
	_matrix RotationXMatrix = XMMatrixRotationX(XMConvertToRadians(90.0f));
	_matrix RotationYMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
	_matrix RotationZMatrix = XMMatrixRotationZ(XMConvertToRadians(0.0f));
	_matrix RotationMatrix = RotationXMatrix * RotationYMatrix * RotationZMatrix;
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) *RotationMatrix;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Umbrella"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Equipments/Umbrella/Umbrella.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Flask*/
	RotationXMatrix = XMMatrixRotationX(XMConvertToRadians(0.0f));
	RotationYMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
	RotationZMatrix = XMMatrixRotationZ(XMConvertToRadians(270.0f));
	RotationMatrix = RotationXMatrix * RotationYMatrix * RotationZMatrix;
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) *RotationMatrix;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Flask"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Equipments/tube/tube.fbx", PivotMatrix))))
		return E_FAIL;

	RotationXMatrix = XMMatrixRotationX(XMConvertToRadians(0.0f));
	RotationYMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));
	RotationZMatrix = XMMatrixRotationZ(XMConvertToRadians(0.0f));
	RotationMatrix = RotationXMatrix * RotationYMatrix * RotationZMatrix;
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) *RotationMatrix;
	/*For.Prototype_Component_Model_Blade*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Blade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/Blade/Blade.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Orange*/
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Orange"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/ThrowingObject/Orange.fbx", PivotMatrix))))
		return E_FAIL;


	/*For.Prototype_Component_Model_Default_Hat*/
	PivotMatrix = /*XMMatrixTransformation(XMLoadFloat3(&scale),)*/XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixTranslation(0.f, 1.f, 0.f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Default_Hat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Equipments/hats/hatkid_hat.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Witch_Hat*/
	PivotMatrix = /*XMMatrixTransformation(XMLoadFloat3(&scale),)*/XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixTranslation(0.f, 3.f, 0.f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Witch_Hat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Equipments/hats/witch_hat.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Sprint_Hat*/
	PivotMatrix = /*XMMatrixTransformation(XMLoadFloat3(&scale),)*/XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixTranslation(0.f, 3.f, 0.f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sprint_Hat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Equipments/hats/sprint_hat.fbx", PivotMatrix))))
		return E_FAIL;


	/*Monster Model*/
	PivotMatrix = XMMatrixIdentity();
	/*For.Prototype_Component_Model_Mafia*/
	PivotMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Mafia"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Enemy/Mafia/Mafia.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Mad_Crow*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MadCrow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Enemy/Mad_Crow/Mad_Crow.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Apple*/
	PivotMatrix = XMMatrixScaling(0.08f, 0.08f, 0.08f) * /*XMMatrixRotationY(XMConvertToRadians(180.0f))**/ XMMatrixTranslation(0.f, 0.7f, 0.f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Apple"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/apple/fruit.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Effect Modle*/
	/*For.Prototype_Component_Model_RunningPuff*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RunningPuff"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/puff/running_puff.fbx", PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_SprintPuff*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SprintPuff"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/puff/running_spike_puff.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Spike_Puff*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SpikePuff"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/puff/Spike_Puff.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Ring*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ring"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/AttackEffect/Ring_0.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Crown_0*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.1f, 0.05f, 0.1f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Crown_0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/AttackEffect/Effect_Crown_0.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Crown_1*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.015f, 0.03f, 0.015f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Crown_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/AttackEffect/Effect_Crown_1.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Rotation*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rotation"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/AttackEffect/Effect_Rotation_0.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_DeadSphere*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DeadSphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/BossDeadEffect.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Bars*/
	PivotMatrix = XMMatrixIdentity();
	//PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bars"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/NonAnim/BigFence/Bars.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Button*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Button"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Button/Button.fbx", PivotMatrix))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForStatic_Shader()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE_DECLARATION::Elements, VTXRECTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE_DECLARATION::Elements, VTXPOINTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeTexture */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCubeTexture"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxCubeTexture.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	///* For.Prototype_Component_Shader_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Cube"), 
	//	CShader::Create(m_pGraphic_Device, TEXT("../Bin/Shaderfiles/Shader_Cube.hlsl")))))
	//	return E_FAIL;

	/* 콜라이더 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("콜라이더 생성 중."));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForStatic_Texture()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGroundchap2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/chapter_2.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_playerhealth_new*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_playerhealth_new"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ui/health/playerhealth_new_%d.dds"), 5))))
		return E_FAIL;

	/*Prototype_Component_Texture_playerhealth_arrow*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_playerhealth_arrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ui/health/playerhealth_arrow_%d.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_text_life*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_text_life"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ui/health/text_life_%d.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Star*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ui/hit/star_%d.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Star_Outline*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Outline"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ui/hit/star_outline_tex_%d.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Star_Particle_Mask*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Particle_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ui/hit/star_particle_mask_%d.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Dissolve*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Dissolve/Dissolve_%d.dds")))))
		return E_FAIL;

	/*Prototype_Component_HitImpact_ramp*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_HitImpact_ramp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ui/hit/hitimpact_ramp_%d.dds")))))
		return E_FAIL;

	/*Prototype_Component_HitImpact*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_HitImpact"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/ui/hit/hitimpact_%d.dds")))))
		return E_FAIL;

	/*Prototype_Component_Logo*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Start"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Start.png")))))
		return E_FAIL;

	/*Prototype_Component_Logo*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Quest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Ui/quest/St1_Quest_1.dds")))))
		return E_FAIL;

	/*Prototype_Component_Logo*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Talk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Talk/MafiaTalk_%d.dds"), 3))))
		return E_FAIL;


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_ForPrototype_Object()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/*For.Prototype_GameObject_Particle_Point */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Point"),
		CParticle_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. HatKid Objects*/
	/*For.Prototype_GameObject_Hatkid*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HatKid"),
		CHatKid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Umbrella*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Umbrella"),
		CUmbrella::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Flask*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Flask"),
		CFlask::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Punch*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Punch"),
		CPunch::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Hat*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hat"),
		CHat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Blade*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blade"),
		CBlade::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Throne*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Throne"),
		CThrone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///*For.Prototype_GameObject_Monster */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	/*For.Prototype_GameObject_Dweller*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dweller"),
		CDweller::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_SubConEye*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SubConEye"),
		CSubConEye::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Clerk*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Clerk"),
		CClerk::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*Monster 객체 생성*/
	

	/*For.Prototype_GameObject_BossBullet*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossBullet"),
		CBoss_Bullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Mafia */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mafia"),
		CMafia::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Mafia */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MafiaElite"),
		CMafiaElite::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MadCrow"),
		CMadCrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*Monster`s Object 생성*/
	/*For.Prototype_GameObject_ThrowingObject*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThrowingObject"),
		CThrowing_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	/*For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*StaticMesh 생성*/
	/*For.Prototype_GameObject_Apple*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Apple"),
		CApple::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*TriggerBox*/
	/*For.Prototype_GameObject_TriggerBox*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger_Box"),
		CTrigger_Box::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Effect Prototype*/

	/*For.Prototype_GameObject_Puff*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Puff"),
		CPuff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Puff*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DizzyEffect"),
		CDizzy_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrownEffect"),
		CCrown::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotationEffect"),
		CRotation_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Ring*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ring"),
		CRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_DeadSphere*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeadSphere"),
		CDeadSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_StarEffect*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StarEffect"),
		CStar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Talk"),
		CTalk::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*UI*/

	///*For.Prototype_GameObject_UI */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Base"),
	//	CUI_Base::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/*For.Prototype_GameObject_UI_TextLife */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TextLife"),
		CUI_TextLife::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_UI_PlayerHealth */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PlayerHealth"),
		CUI_PlayerHealth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_UI_Arrow */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Arrow"),
		CUI_Arrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_UI_Arrow */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Start"),
		CUI_Start::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_UI_Arrow */


	/* For.Instance*/

	/*For.Prototype_Component_VIBuffer_Point_Instance_Up*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_Up"),
		CPoint_Instance_Up::Create(m_pDevice, m_pContext, 50))))
		return E_FAIL;

	/*For.Prototype_Component_VIBuffer_Point_Instance_Turn*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_Turn"),
		CPoint_Instance_Turn::Create(m_pDevice, m_pContext, 10))))
		return E_FAIL;

	/*For.Prototype_GameObject_Platform */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Platform"),
		CMafia_Platform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Bars */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bars"),
		CBars::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Bars */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Button"),
		CButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_ForModel_Play1()
{
	_matrix			PivotMatrix = XMMatrixIdentity();




	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		ERR_MSG(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
