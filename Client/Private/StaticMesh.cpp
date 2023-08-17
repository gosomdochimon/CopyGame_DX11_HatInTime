#include "stdafx.h"
#include "..\Public\StaticMesh.h"
#include "GameInstance.h"

CStaticMesh::CStaticMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CActor(pDevice, pContext)
{
}

CStaticMesh::CStaticMesh(const CStaticMesh & rhs)
	:CActor(rhs)
{
}

HRESULT CStaticMesh::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CStaticMesh::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CStaticMesh::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CStaticMesh::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		////m_pRendererCom->Add_Debug(m_pSPHERECom);
#endif

	}

}

HRESULT CStaticMesh::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

#ifdef _DEBUG
	//m_pSPHERECom->Render();
#endif

	return S_OK;
}

HRESULT CStaticMesh::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_pTransformCom->Get_World4x4())));

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

HRESULT CStaticMesh::Ready_Components()
{
	return S_OK;
}

CStaticMesh * CStaticMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStaticMesh*	pInstance = new CStaticMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CStaticMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStaticMesh::Clone(void * pArg)
{
	CStaticMesh*	pInstance = new CStaticMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CStaticMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticMesh::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
