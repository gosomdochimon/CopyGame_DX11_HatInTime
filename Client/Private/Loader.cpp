#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"


#include "Camera_Dynamic.h"
#include "BackGround.h"
#include "Terrain.h"
#include "HatKid.h"
#include "Umbrella.h"
#include "Flask.h"
#include "Hat.h"
#include "Mafia.h"
#include "MadCrow.h"
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
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;
	
	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 로딩 중."));
	

	/* 셰이더 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중."));
	

	

	// CShader::Create(m_pDevice, m_pContext, TEXT(""), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	

	

	/* 객체 원형 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("객체 생성 중."));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

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
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Brush */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), 1))))
		return E_FAIL;

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

	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 로딩 중."));

	/*For.Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	_matrix			PivotMatrix = XMMatrixIdentity();

	/////*For.Prototype_Component_Model_Fiona*/
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Fiona/Fiona.fbx", PivotMatrix))))
	//	return E_FAIL;

	/*For.Prototype_Component_Model_Hatkid*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HatKid"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/HatKid/HatKidAnim.fbx", PivotMatrix))))
		return E_FAIL;

	///*For.Prototype_Component_Model_ForkLift*/
	//_float3 scale = _float3(0.01f, 0.01f, 0.01f);
	//PivotMatrix = /*XMMatrixTransformation(XMLoadFloat3(&scale),)*/XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/ForkLift/ForkLift.fbx", PivotMatrix))))
	//	return E_FAIL;

	/*For.Prototype_Component_Model_Umbrella*/
	_matrix RotationXMatrix = XMMatrixRotationX(XMConvertToRadians(90.0f));
	_matrix RotationYMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
	_matrix RotationZMatrix = XMMatrixRotationZ(XMConvertToRadians(0.0f));
	_matrix RotationMatrix = RotationXMatrix * RotationYMatrix * RotationZMatrix;
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) *RotationMatrix;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Umbrella"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Equipments/Umbrella/Umbrella.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Flask*/
	RotationXMatrix = XMMatrixRotationX(XMConvertToRadians(0.0f));
	RotationYMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
	RotationZMatrix = XMMatrixRotationZ(XMConvertToRadians(270.0f));
	RotationMatrix = RotationXMatrix * RotationYMatrix * RotationZMatrix;
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) *RotationMatrix;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Flask"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Equipments/tube/tube.fbx", PivotMatrix))))
		return E_FAIL;



	/*For.Prototype_Component_Model_Default_Hat*/
	PivotMatrix = /*XMMatrixTransformation(XMLoadFloat3(&scale),)*/XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixTranslation(0.f, 1.f, 0.f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Default_Hat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Equipments/hats/hatkid_hat.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Witch_Hat*/
	PivotMatrix = /*XMMatrixTransformation(XMLoadFloat3(&scale),)*/XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixTranslation(0.f, 3.f, 0.f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Witch_Hat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Equipments/hats/witch_hat.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Sprint_Hat*/
	PivotMatrix = /*XMMatrixTransformation(XMLoadFloat3(&scale),)*/XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixTranslation(0.f, 3.f, 0.f)/* * XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sprint_Hat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Equipments/hats/sprint_hat.fbx", PivotMatrix))))
		return E_FAIL;

	/*Monster 모델 생성*/
	/*For.Prototype_Component_Model_Mafia*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mafia"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Enemy/Mafia/Mafia.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Mad_Crow*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MadCrow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Enemy/Mad_Crow/Mad_Crow.fbx", PivotMatrix))))
		return E_FAIL;

	///*For.Prototype_Component_VIBuffer_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;
	//

	/* 셰이더 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중."));

	///* For.Prototype_Component_Shader_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Cube"), 
	//	CShader::Create(m_pGraphic_Device, TEXT("../Bin/Shaderfiles/Shader_Cube.hlsl")))))
	//	return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"), 
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	
	/* For.Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* 콜라이더 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("콜라이더 생성 중."));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* 객체 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("객체 생성 중."));

	///*For.Prototype_GameObject_UI */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
	//	CUI::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///*For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///*For.Prototype_GameObject_Player*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
	//	CPlayer::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

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

	/*For.Prototype_GameObject_Hat*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hat"),
		CHat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///*For.Prototype_GameObject_Monster */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	/*Monster 객체 생성*/
	/*For.Prototype_GameObject_Mafia */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mafia"),
		CMafia::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MadCrow"),
		CMadCrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//
	///*For.Prototype_GameObject_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pGraphic_Device))))
	//	return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;


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
