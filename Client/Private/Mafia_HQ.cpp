#include "stdafx.h"
#include "..\Public\Mafia_HQ.h"
#include "GameInstance.h"

CMafia_HQ::CMafia_HQ(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CStaticMesh(pDevice, pContext)
{
}

CMafia_HQ::CMafia_HQ(const CMafia_HQ & rhs)
	:CStaticMesh(rhs)
{
}

HRESULT CMafia_HQ::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMafia_HQ::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	memcpy(&m_CloneInfo, (CLONEINFO*)pArg, sizeof(CLONEINFO));

	m_pTransformCom->Set_WorldMatrix(m_CloneInfo.WorldMat);
	return S_OK;
}

void CMafia_HQ::Tick(_float fTimeDelta)
{
	if (Get_IsDeleted())
		return;

	__super::Tick(fTimeDelta);

}

void CMafia_HQ::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMafia_HQ::Render()
{
	__super::Render();

#ifdef _DEBUG
	//m_pNavigationCom->Render_Navigation();
#endif

	return S_OK;
}

HRESULT CMafia_HQ::Ready_Components()
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

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mafia_HQ"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom)))
		return E_FAIL;

	//CCollider::COLLIDERDESC		ColliderDesc;
	///* For.Com_OBB*/
	//ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	//ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMafia_HQ::SetUp_ShaderResources()
{
	__super::SetUp_ShaderResources();

	return S_OK;
}

CMafia_HQ * CMafia_HQ::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMafia_HQ*	pInstance = new CMafia_HQ(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMafia_HQ"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMafia_HQ::Clone(void * pArg)
{
	CMafia_HQ*	pInstance = new CMafia_HQ(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMafia_HQ"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMafia_HQ::Free()
{
	__super::Free();
	Safe_Release(m_pNavigationCom);
}
