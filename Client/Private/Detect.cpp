#include "stdafx.h"
#include "..\Public\Detect.h"
#include "GameInstance.h"

CDetect::CDetect()
{
}

HRESULT CDetect::Initialize(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	memcpy(&m_DetectDesc, pArg, sizeof(DETECTDESC));
	
	Ready_Components();

	return S_OK;
}

void CDetect::Update_Position(_matrix WorldMatrix)
{
	m_pMinSphereCom->Update(WorldMatrix);
	m_pMaxSphereCom->Update(WorldMatrix);
}

_bool CDetect::Detect_MinRange(CCollider * TargetCollider)
{
	return m_pMinSphereCom->Collision(TargetCollider);
}

_bool CDetect::Detect_MaxRange(CCollider * TargetCollider)
{
	return m_pMaxSphereCom->Collision(TargetCollider);
}



HRESULT CDetect::Ready_Components(void * pArg)
{
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_MaxSPHERE */
	ColliderDesc.vScale = m_DetectDesc.MaxDetect_Collider.vScale;
	ColliderDesc.vRotation = m_DetectDesc.MaxDetect_Collider.vRotation;
	ColliderDesc.vPosition = m_DetectDesc.MaxDetect_Collider.vPosition;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pMinSphereCom, &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	/* For.Com_MinSPHERE */
	ColliderDesc.vScale = m_DetectDesc.MinDetect_Collider.vScale;
	ColliderDesc.vRotation = m_DetectDesc.MinDetect_Collider.vRotation;
	ColliderDesc.vPosition = m_DetectDesc.MinDetect_Collider.vPosition;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pMaxSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDetect::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent ** ppOut, void * pArg)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CComponent*		pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	*ppOut = pComponent;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CDetect::Render()
{
	if (m_pMaxSphereCom == nullptr ||
		m_pMinSphereCom == nullptr)
		return E_FAIL;

#ifdef _DEBUG
	m_pMaxSphereCom->Render();
	m_pMinSphereCom->Render();

#endif

	return S_OK;
}



void CDetect::Free()
{
	Safe_Release(m_pMinSphereCom);
	Safe_Release(m_pMaxSphereCom);
}

