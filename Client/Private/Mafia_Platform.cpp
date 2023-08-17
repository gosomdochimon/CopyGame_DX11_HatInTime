#include "stdafx.h"
#include "..\Public\Mafia_Platform.h"
#include "GameInstance.h"

CMafia_Platform::CMafia_Platform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CStaticMesh(pDevice, pContext)
{
}

CMafia_Platform::CMafia_Platform(const CMafia_Platform & rhs)
	:CStaticMesh(rhs)
{
}

HRESULT CMafia_Platform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMafia_Platform::Initialize(void * pArg)
{

	if (pArg != nullptr)
	{
		memcpy(&m_CloneInfo, (CLONEINFO*)pArg, sizeof(CLONEINFO));
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;
	

	return S_OK;
}

void CMafia_Platform::Tick(_float fTimeDelta)
{
	if (Get_IsDeleted())
		return;

	__super::Tick(fTimeDelta);

}

void CMafia_Platform::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMafia_Platform::Render()
{
	__super::Render();

#ifdef _DEBUG
	//m_pNavigationCom->Render_Navigation();
#endif

	return S_OK;
}

HRESULT CMafia_Platform::Ready_Components()
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

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	m_pTransformCom->Set_WorldMatrix(m_CloneInfo.WorldMat);
	/* For.Com_Model*/
	switch (m_CloneInfo.iModelNum)
	{
	case 4:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_Mafia_Platform"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 5:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_Mafia_Poly"), (CComponent**)&m_pModelCom)))
			return E_FAIL;	
		m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, 0.03f);
		m_pTransformCom->Set_Scale(CTransform::STATE_UP, 0.04f);
		m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, 0.03f);
		break;
	case 6:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_Carpet"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 8:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Model_ParmTrees"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}



	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = -1;
	XMStoreFloat3(&NaviDesc.fCurrent_Position, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY_PLATFORM, TEXT("Prototype_Component_Navigation_Platform"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	//CCollider::COLLIDERDESC		ColliderDesc;
	///* For.Com_OBB*/
	//ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	//ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMafia_Platform::SetUp_ShaderResources()
{
	__super::SetUp_ShaderResources();

	return S_OK;
}

CMafia_Platform * CMafia_Platform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMafia_Platform*	pInstance = new CMafia_Platform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMafia_Platform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMafia_Platform::Clone(void * pArg)
{
	CMafia_Platform*	pInstance = new CMafia_Platform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMafia_Platform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMafia_Platform::Free()
{
	__super::Free();
	Safe_Release(m_pNavigationCom);
}
