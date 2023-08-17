#include "stdafx.h"
#include "..\Public\Trigger_Box.h"
#include "GameInstance.h"
#include "DataManager.h"
#include "Interactive_Manager.h"
#include "HatKid.h"
CTrigger_Box::CTrigger_Box(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CActor(pDevice, pContext)
{
}

CTrigger_Box::CTrigger_Box(const CTrigger_Box & rhs)
	: CActor(rhs)
{
}

HRESULT CTrigger_Box::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrigger_Box::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	CDataManager* pDataMgr = GET_INSTANCE(CDataManager);
	_float3 vPos = pDataMgr->Get_TriggerPoints().front();
	RELEASE_INSTANCE(CDataManager);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f));
	m_pTransformCom->Go_Up(0.05f, nullptr, -1);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_Group(CCollider_Manager::GROUP_TYPE::TYPE_OBJECT, this, m_pAABBCom);
	Safe_AddRef(this);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Particle_Point"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &vPos)))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);

	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Particle_Point"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
	//	return E_FAIL;

	return S_OK;
}

void CTrigger_Box::Tick(_float fTimeDelta)
{
	if (m_bDelete)
		return;

	if (m_bCollider)
	{
		m_fTimer += fTimeDelta;
		if (m_fTimer >= 0.5f)
		{
			tagCloneInfo Info;

			CDataManager* pDataMgr = GET_INSTANCE(CDataManager);
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			Info = pDataMgr->Get_Tu3_Data()[0];
			Info.fRot_Value = pDataMgr->Get_ArrivePoints()[m_iGateCount];
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mafia"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &Info)))
				return;

			Info = pDataMgr->Get_Tu3_Data()[1];
			Info.fRot_Value = pDataMgr->Get_ArrivePoints()[m_iGateCount+3];
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mafia"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &Info)))
				return;

			
			RELEASE_INSTANCE(CDataManager);
			RELEASE_INSTANCE(CGameInstance);
			printf_s("%d \n", m_iGateCount);
			if (m_iGateCount == 2)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				_float fVolume = 0.6f;
				pGameInstance->StopSound(0);
				pGameInstance->PlayBGM(TEXT("Movie_Rating_Screen_Music.ogg"), fVolume);
				pGameInstance->Out_Group(CCollider_Manager::GROUP_TYPE::TYPE_OBJECT, this);
				RELEASE_INSTANCE(CGameInstance);
				Set_DeleteObject();
				printf_s("Finished\n");
			}
				

			m_iGateCount++;
			m_fTimer = 0.f;
		}
	
		
	}
}

void CTrigger_Box::Late_Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr == pGameInstance)
		return;

	/*CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_SPHERE"));
	if (nullptr == pTargetCollider)
		return;*/

	if (!m_bCollider &&pGameInstance->Collider_Group(CCollider_Manager::GROUP_TYPE::TYPE_PLAYER, this, m_pAABBCom, 0.f))
	{

		CInteractive_Manager* pInter = GET_INSTANCE(CInteractive_Manager);
		CHatKid* Kid = pInter->Get_HatKid();

		Kid->Set_CanMove(false);

		RELEASE_INSTANCE(CInteractive_Manager);

		m_bCollider = true;
		//printf_s("Trigger_On\n");
	}
	RELEASE_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
	{
		//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER, this);

#ifdef _DEBUG
		//m_pRendererCom->Add_Debug(m_pAABBCom);
#endif

	}
}

HRESULT CTrigger_Box::Render()
{

#ifdef _DEBUG
	//m_pAABBCom->Render();
	//m_pOBBCom->Render();
	//m_pSPHERECom->Render();
#endif

	return S_OK;
}

HRESULT CTrigger_Box::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(20.f, 20.f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_AABB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger_Box::SetUp_ShaderResources()
{
	return S_OK;
}

CTrigger_Box * CTrigger_Box::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrigger_Box*	pInstance = new CTrigger_Box(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTrigger_Box"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTrigger_Box::Clone(void * pArg)
{
	CTrigger_Box*	pInstance = new CTrigger_Box(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTrigger_Box"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger_Box::Free()
{
	__super::Free();
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
}
