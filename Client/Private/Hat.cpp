#include "stdafx.h"
#include "..\Public\Hat.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CHat::CHat(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEquipment(pDevice, pContext)
{

}

CHat::CHat(const CHat & rhs)
	: CEquipment(rhs)
{

}

HRESULT CHat::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CHat::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (nullptr != pArg)
		memcpy(&m_EquipDesc, pArg, sizeof(EQUIPDESC));

	//Safe_AddRef(m_EquipDesc.pSocket);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, 0.1f);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, 0.1f);
	m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, 0.1f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.0f));

	return S_OK;
}

void CHat::Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = /*m_EquipDesc.pSocket->Get_OffsetMatrix() **/
		m_EquipDesc.pSocket->Get_CombinedTransformationMatrix() *
		XMLoadFloat4x4(&m_EquipDesc.SocketPivotMatrix) * XMLoadFloat4x4(m_EquipDesc.pParentWorldMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);



	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix);

}

void CHat::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CHat::Render()
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

	return S_OK;
}

HRESULT CHat::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(Select_HatType()))
		return E_FAIL;


	return S_OK;
}

HRESULT CHat::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

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

HRESULT CHat::Select_HatType(void)
{
	switch (m_EquipDesc.iTypeNum)
	{
	case 1:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Default_Hat"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Witch_Hat"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 3:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sprint_Hat"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}

	return S_OK;
}

CHat * CHat::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHat*	pInstance = new CHat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHat::Clone(void * pArg)
{
	CHat*	pInstance = new CHat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHat::Free()
{
	__super::Free();

	Safe_Release(m_EquipDesc.pSocket);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
