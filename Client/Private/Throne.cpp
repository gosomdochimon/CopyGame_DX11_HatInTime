#include "stdafx.h"
#include "..\Public\Throne.h"
#include "HierarchyNode.h"
#include "GameInstance.h"
#include "Deligate_Monster.h"
CThrone::CThrone(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CStaticMesh(pDevice, pContext)
{
}

CThrone::CThrone(const CThrone & rhs)
	: CStaticMesh(rhs)
{
}

HRESULT CThrone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CThrone::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CThrone::Tick(_float fTimeDelta)
{
	if (Get_IsDeleted())
		return;

	__super::Tick(fTimeDelta);


	Set_Height();

}

void CThrone::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CThrone::Render()
{
	__super::Render();

#ifdef _DEBUG
	m_pSPHERECom->Render();
#endif

	return S_OK;
}

void CThrone::Set_Throw(_bool bThrow)
{
	if (bThrow)
	{
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix));
	}
	m_bThrow = bThrow;
}

void CThrone::Set_Picked(_bool bPicked)
{
	{
		if (!m_bPicked)
		{
			m_pTransformCom->ResetWorld4x4();
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), 180.f);
			m_pTransformCom->Go_Straight(-0.07f);
			m_pTransformCom->Go_Left(0.1f);
			m_pTransformCom->Go_Down(0.05f);
			//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		}
		m_bPicked = bPicked;
	}
}

HRESULT CThrone::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Throne"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;
	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;



	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;
	NaviDesc.fCurrent_Position = { -14.2f, -3.f, 136.1f };

	_vector vPos = XMLoadFloat3(&NaviDesc.fCurrent_Position);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CThrone::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	if (m_bPicked == true)
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));
	}
	else
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_pTransformCom->Get_World4x4())));
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CThrone::Set_Height()
{
	if (m_bPicked || m_bThrow)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_pNavigationCom->isMove(vPos);
		return;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 fPos;
	XMStoreFloat3(&fPos, vPos);

	fPos.y = m_pNavigationCom->Get_NavigationHeight(fPos);
	vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
}

CThrone * CThrone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CThrone*	pInstance = new CThrone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CThrone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CThrone::Clone(void * pArg)
{
	CThrone*	pInstance = new CThrone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CThrone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThrone::Free()
{
	__super::Free();
	Safe_Release(m_pSPHERECom);
	Safe_Release(m_pNavigationCom);
}
